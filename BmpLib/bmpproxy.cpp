#include "bmpproxy.h"
#include "bmpdefs.h"
#include "bmputils.h"
#include "bmpexceptions.h"
#include "dynamicbitset.h"
#include "filehandler.h"

#include <stdexcept>
#include <vector>
#include <thread>
#include <chrono>
#include <cassert>
#include <cstring>
#include <memory.h>


namespace {

constexpr std::size_t    INFO_HEADER_OFFSET       = sizeof(BmpZipper::BmpHeader);
constexpr std::uint16_t  UNCOMPRESSED_SIGNATURE   = 0x4D42;
constexpr std::uint16_t  COMPRESSED_SIGNATURE     = 0x4142;
constexpr std::uint8_t   WHITE_PIXEL              = 0xFF;
constexpr std::uint8_t   BLACK_PIXEL              = 0x00;
constexpr std::uint32_t  WHITE_4PIXELS            = 0xFFFFFFFF;
constexpr std::uint32_t  BLACK_4PIXELS            = 0x00000000;

// RowIndex encodes each exact row as separate bit [0:n-1]
// The bit is set to 1 when row contains only white pixels
// The bit is set to 0 when row contains not only white pixels
struct RowIndex
{
    explicit RowIndex(const std::size_t height)
        : m_index(height / BmpZipper::DynamicBitset::BITS_PER_BLOCK, 0x00)
    {
    }

    explicit RowIndex(std::vector<std::uint8_t> && source)
        : m_index(std::move(source))
    {
    }

    void setRowIsEmpty(const std::size_t row, const bool value)
    {
        m_index.set(row, value);
    }

    [[nodiscard]]
    bool testRowIsEmpty(const std::size_t row) const
    {
        return m_index.test(row);
    }

    [[nodiscard]]
    std::size_t getIndexSizeInBytes() const
    {
        return m_index.numBlocks();
    }

    [[nodiscard]]
    const std::uint8_t* getData() const
    {
        return m_index.data();
    }

    [[nodiscard]]
    std::uint8_t* getData()
    {
        return m_index.data();
    }

    static std::vector<std::uint8_t> getWhiteRowPattern(const int width)
    {
        const int padding = BmpZipper::RawImageData::calculatePadding(width);
        std::vector whiteRowPattern(width + padding, WHITE_PIXEL);
        for(int i = width; i < width + padding; ++i)
            whiteRowPattern[i] = BLACK_PIXEL; // Fill whiteRowPattern with zero padding if needed

        return whiteRowPattern;
    }

    static RowIndex createFromRawImageData(
        const BmpZipper::RawImageData raw,
        BmpZipper::IProgressNotifier* progressNotifier = nullptr)
    {
        const auto whiteRowPattern = getWhiteRowPattern(raw.Width);

        RowIndex index(raw.getActualHeight());
        const std::uint8_t * rowStartPtr = raw.Data;
        for(int rowIndex = 0; rowIndex < raw.getActualHeight(); ++rowIndex)
        {
            index.setRowIsEmpty(rowIndex, memcmp(rowStartPtr, whiteRowPattern.data(), whiteRowPattern.size()) == 0);

            rowStartPtr += raw.getActualWidth();
            if(progressNotifier)
            {
                using namespace std::chrono_literals;
                std::this_thread::sleep_for(1ms); // For progress bar demonstration
                progressNotifier->notifyProgress(rowIndex);
            }
        }

        return index;
    }

    BmpZipper::DynamicBitset m_index;
};

bool RollbackFile(const std::string& filePath, BmpZipper::FileHandler& file)
{
    file.close();
    return remove(filePath.c_str()) == 0;
}

} // namespace

namespace BmpZipper {

struct BmpProxy::ProxyImpl
{
    static std::unique_ptr<ProxyImpl> readBmp(const std::string& filePath)
    {
        auto impl = std::make_unique<ProxyImpl>();
        impl->m_filePath = filePath;
        impl->m_fileHandler = FileHandler(filePath, "rb");

        auto& fileHandler = impl->m_fileHandler;

        fileHandler.seek(SEEK_END);
        const std::size_t actualFileSize = fileHandler.tell();
        fileHandler.seek(SEEK_SET);

        // BMP Header validation
        const bool headerReadCorrectly = fileHandler.read(&impl->m_header, sizeof(m_header)) == 1;
        if(!headerReadCorrectly)
            throw InvalidBmpHeaderError();
        if(impl->m_header.Signature != UNCOMPRESSED_SIGNATURE) // Check Signature = 'BM'
            throw InvalidBmpHeaderError(std::string("Unexpected signature: ") + std::to_string(impl->m_header.Signature));
        if(impl->m_header.FileSize != actualFileSize)
            throw InvalidBmpHeaderError(std::string("File size mismatch: ") +
                "actual[" + std::to_string(actualFileSize) + "] != expected[" + std::to_string(impl->m_header.FileSize) + "]"
            );
        if(impl->m_header.DataOffset < INFO_HEADER_OFFSET + sizeof(BmpInfoHeader))
            throw InvalidBmpHeaderError(std::string("Invalid Data Offset: ") + std::to_string(impl->m_header.DataOffset));

        // BMP Info Header validation
        const bool infoHeaderReadCorrectly = fileHandler.read(&impl->m_infoHeader, sizeof(m_infoHeader)) == 1;
        if(!infoHeaderReadCorrectly)
            throw InvalidInfoHeaderError();
        if(impl->m_infoHeader.Size < sizeof(BmpInfoHeader)) // Can be > sizeof(BmpInfoHeader) in new bmp versions
            throw InvalidInfoHeaderError(std::string("Incorrect InfoHeader Size: " + std::to_string(impl->m_infoHeader.Size)));
        if(impl->m_infoHeader.BitsPerPixel != 8)
            throw InvalidInfoHeaderError(std::string("Only 8bit Bmp pictures are supported"));
        if(impl->m_infoHeader.ImageSize != 0 && impl->m_infoHeader.Height * impl->m_infoHeader.Width > impl->m_infoHeader.ImageSize)
            throw InvalidInfoHeaderError(std::string("Unexpected Image Size: " + std::to_string(impl->m_infoHeader.ImageSize)));

        const std::size_t colorTableOffset = INFO_HEADER_OFFSET + impl->m_infoHeader.Size;
        constexpr std::size_t bmpColorInfoSize = sizeof(std::uint32_t); // Bmp Color Structure

        if(impl->m_header.DataOffset < colorTableOffset + impl->m_infoHeader.ColorsUsed * bmpColorInfoSize)
            throw InvalidBmpHeaderError(std::string("Invalid Data Offset: ") + std::to_string(impl->m_header.DataOffset));

        const int padding = RawImageData::calculatePadding(static_cast<int>(impl->m_infoHeader.Width));

        // Read Pixel Data
        const std::size_t realPixelDataSize = impl->m_infoHeader.ImageSize
            ? impl->m_infoHeader.ImageSize
            : impl->m_infoHeader.Height * (impl->m_infoHeader.Width + padding);

        impl->m_pixelData.resize(realPixelDataSize, 0x00);
        fileHandler.seek(SEEK_SET, impl->m_header.DataOffset);
        const bool pixelDataReadCorrectly = fileHandler.read(impl->m_pixelData.data(), impl->m_pixelData.size()) == 1;
        if(!pixelDataReadCorrectly)
            throw InvalidPixelDataError("Unable to read Pixel Data");

        return impl;
    }

    static std::unique_ptr<ProxyImpl> readBarch(const std::string& filePath)
    {
        auto impl = std::make_unique<ProxyImpl>();
        impl->m_filePath = filePath;
        impl->m_fileHandler = FileHandler( filePath, "rb" );

        auto& fileHandler = impl->m_fileHandler;

        fileHandler.seek(SEEK_END);
        const std::size_t actualFileSize = fileHandler.tell();
        fileHandler.seek(SEEK_SET);

        // BMP Header validation
        const bool headerReadCorrectly = fileHandler.read(&impl->m_header, sizeof(m_header)) == 1;
        if(!headerReadCorrectly)
            throw InvalidBmpHeaderError();
        if(impl->m_header.Signature != COMPRESSED_SIGNATURE) // Check Signature = 'BA'
            throw InvalidBmpHeaderError(std::string("Unexpected signature: ") + std::to_string(impl->m_header.Signature));
        if(impl->m_header.FileSize != actualFileSize)
            throw InvalidBmpHeaderError(std::string("File size mismatch: ") +
                "actual[" + std::to_string(actualFileSize) + "] != expected[" + std::to_string(impl->m_header.FileSize) + "]"
            );
        if(impl->m_header.IndexOffset < INFO_HEADER_OFFSET + sizeof(BmpInfoHeader))
            throw InvalidBmpHeaderError(std::string("Invalid Index Offset: ") + std::to_string(impl->m_header.IndexOffset));
        if(impl->m_header.DataOffset < impl->m_header.IndexOffset)
            throw InvalidBmpHeaderError(std::string("Invalid Data Offset: ") + std::to_string(impl->m_header.DataOffset));

        // BMP Info Header validation
        const bool infoHeaderReadCorrectly = fileHandler.read(&impl->m_infoHeader, sizeof(m_infoHeader)) == 1;
        if(!infoHeaderReadCorrectly)
            throw InvalidInfoHeaderError();
        if(impl->m_infoHeader.Size < sizeof(BmpInfoHeader)) // Can be > sizeof(BmpInfoHeader) in new bmp versions
            throw InvalidInfoHeaderError(std::string("Incorrect InfoHeader Size: " + std::to_string(impl->m_infoHeader.Size)));
        if(impl->m_infoHeader.BitsPerPixel != 8)
            throw InvalidInfoHeaderError(std::string("Only 8bit Bmp pictures are supported"));

        const std::size_t colorTableOffset = INFO_HEADER_OFFSET + impl->m_infoHeader.Size;
        constexpr std::size_t bmpColorInfoSize = sizeof(std::uint32_t); // Bmp Color Structure

        if(impl->m_header.IndexOffset < colorTableOffset + impl->m_infoHeader.ColorsUsed * bmpColorInfoSize)
            throw InvalidBmpHeaderError(std::string("Invalid Index Offset: ") + std::to_string(impl->m_header.IndexOffset));

        // Read Index Data
        std::vector<std::uint8_t> indexData(impl->m_infoHeader.Height / DynamicBitset::BITS_PER_BLOCK, 0x00);
        fileHandler.seek(SEEK_SET, impl->m_header.IndexOffset);
        if(fileHandler.read(indexData.data(), indexData.size()) != 1)
            throw InvalidPixelDataError("Unable to read Index Data");
        impl->m_index = std::make_unique<RowIndex>(std::move(indexData));

        // Read Pixel Data Compressed
        impl->m_pixelData.resize(impl->m_infoHeader.ImageSize, 0x00);
        fileHandler.seek(SEEK_SET, impl->m_header.DataOffset);
        const bool pixelDataReadCorrectly = fileHandler.read(impl->m_pixelData.data(), impl->m_pixelData.size()) == 1;
        if(!pixelDataReadCorrectly)
            throw InvalidPixelDataError("Unable to read Pixel Data");

        return impl;
    }

    [[nodiscard]]
    const std::string& getFilePath() const
    {
        return m_filePath;
    }

    [[nodiscard]]
    const BmpHeader* getBmpHeader() const
    {
        return &m_header;
    }

    [[nodiscard]]
    const BmpInfoHeader* getInfoHeader() const
    {
        return &m_infoHeader;
    }

    [[nodiscard]]
    std::uint8_t* getPixelData()
    {
        return m_pixelData.data();
    }

    [[nodiscard]]
    const std::uint8_t* getPixelData() const
    {
        return m_pixelData.data();
    }

    [[nodiscard]]
    const RowIndex * getRowIndex() const
    {
        return m_index.get();
    }

    [[nodiscard]]
    bool copyUpToOffset(FileHandler& dest, const long offset)
    {
        m_fileHandler.seek(SEEK_SET);
        std::vector<std::uint8_t> dataCopied(offset, 0x00);
        if(m_fileHandler.read(dataCopied.data(), dataCopied.size()) == 1)
            if(dest.write(dataCopied.data(), dataCopied.size()) == 1)
                return true;

        return false;
    }

    std::string m_filePath;
    FileHandler m_fileHandler;
    BmpHeader m_header{};
    BmpInfoHeader m_infoHeader{};
    std::unique_ptr<RowIndex> m_index;
    std::vector<std::uint8_t> m_pixelData;
};

BmpProxy::BmpProxy(std::unique_ptr<ProxyImpl> pImpl)
    : m_pImpl(std::move(pImpl))
{
}

BmpProxy BmpProxy::createFromBmp(const std::string& filePath)
{
    return BmpProxy{ProxyImpl::readBmp(filePath)};
}

BmpProxy BmpProxy::createFromBarch(const std::string& filePath)
{
    return BmpProxy{ProxyImpl::readBarch(filePath)};
}

BmpProxy::BmpProxy(BmpProxy && other) noexcept
    : m_pImpl(std::move(other.m_pImpl))
{
}

BmpProxy::~BmpProxy() noexcept = default;

const std::string& BmpProxy::getFilePath() const
{
    return m_pImpl->getFilePath();
}

const BmpHeader& BmpProxy::getHeader() const
{
    return *m_pImpl->getBmpHeader();
}

const BmpInfoHeader& BmpProxy::getInfoHeader() const
{
    return *m_pImpl->getInfoHeader();
}

bool BmpProxy::isCompressed() const
{
    return m_pImpl->m_header.Signature == COMPRESSED_SIGNATURE;
}

std::size_t BmpProxy::getWidth() const
{
    return getInfoHeader().Width;
}

std::size_t BmpProxy::getHeight() const
{
    return getInfoHeader().Height;
}

std::uint8_t * BmpProxy::getPixelData()
{
    return m_pImpl->getPixelData();
}

const std::uint8_t * BmpProxy::getPixelData() const
{
    return m_pImpl->getPixelData();
}

bool BmpProxy::provideRawImageData(RawImageData & out) const
{
    if(isCompressed())
        return false;

    const auto* pixelData = getPixelData();
    if(!pixelData)
        return false;

    out.Width = static_cast<int>(getWidth());
    out.Height = static_cast<int>(getHeight());
    out.Data = pixelData;

    return true;
}

bool BmpProxy::compress(const std::string& outputFilePath, IProgressNotifier * progressNotifier) const
{
    if(isCompressed())
        throw std::logic_error("File is already compressed");

    FileHandler resultFile {outputFilePath, "wb"};

    auto rollbackFile = [&resultFile, &outputFilePath] {
        const bool cantRollback = RollbackFile(outputFilePath, resultFile);
        assert(cantRollback && "Unable to rollback file correctly");
        return false;
    };

    BmpHeader header = getHeader();
    header.Signature = COMPRESSED_SIGNATURE; // Specify 'BA' signature
    header.IndexOffset = header.DataOffset; // Specify Index Offset at DataOffset

    RawImageData rawImageData{};
    if(!provideRawImageData(rawImageData))
        return rollbackFile();

    try
    {
        if(progressNotifier)
            progressNotifier->init(0, rawImageData.getActualHeight() << 1);

        RowIndex index = RowIndex::createFromRawImageData(rawImageData, progressNotifier);
        header.DataOffset = header.IndexOffset + index.getIndexSizeInBytes();

        BmpInfoHeader infoHeader = getInfoHeader();
        DynamicBitset compressedPixelData(infoHeader.ImageSize, 0x00);

        std::size_t currentBitPos = 0;
        for(int rowIndex = 0; rowIndex < rawImageData.getActualHeight(); ++rowIndex)
        {
            const auto * rawPixels = reinterpret_cast<const std::uint32_t*>(
                rawImageData.Data + rowIndex * rawImageData.getActualWidth()
            );
            if(!index.testRowIsEmpty(rowIndex))
            {
                for(int blockIndex = 0; blockIndex < rawImageData.getActualWidth() / sizeof(std::uint32_t); ++blockIndex)
                {
                    switch(const std::uint32_t blockValue = *rawPixels)
                    {
                        case BLACK_4PIXELS:
                            compressedPixelData.set(currentBitPos++, true);
                            compressedPixelData.set(currentBitPos++, false);
                            break;

                        case WHITE_4PIXELS:
                            compressedPixelData.set(currentBitPos++, false);
                            break;

                        default:
                            compressedPixelData.set(currentBitPos++, true);
                            compressedPixelData.set(currentBitPos++, true);

                            for(int bitIndex = 0; bitIndex < sizeof(std::uint32_t) * DynamicBitset::BITS_PER_BLOCK; ++bitIndex)
                            {
                                const bool bitValue = (blockValue & 1 << bitIndex) != 0;
                                compressedPixelData.set(currentBitPos++, bitValue);
                            }
                            break;
                    }
                    ++rawPixels;
                }
            }

            if(progressNotifier)
            {
                using namespace std::chrono_literals;
                std::this_thread::sleep_for(1ms); // For progress bar demonstration
                progressNotifier->notifyProgress(rawImageData.getActualHeight() + rowIndex);
            }
        }

        if(!m_pImpl->copyUpToOffset(resultFile, header.IndexOffset))
            return rollbackFile();

        compressedPixelData.shrinkToFit();
        infoHeader.ImageSize = compressedPixelData.numBlocks();
        header.FileSize = resultFile.tell() + index.getIndexSizeInBytes() + compressedPixelData.numBlocks();

        if(resultFile.write(index.getData(), index.getIndexSizeInBytes()) != 1)
            return rollbackFile();
        if(resultFile.write(compressedPixelData.data(), compressedPixelData.numBlocks()) != 1)
            return rollbackFile();
        if(resultFile.tell() != header.FileSize)
            return rollbackFile();

        resultFile.seek(SEEK_SET);
        if(resultFile.write(&header, sizeof(BmpHeader)) != 1)
            return rollbackFile();
        if(resultFile.write(&infoHeader, sizeof(BmpInfoHeader)) != 1)
            return rollbackFile();
    }
    catch( ... )
    {
        return rollbackFile();
    }

    return true;
}

bool BmpProxy::decompress(const std::string& outputFilePath, IProgressNotifier * progressNotifier) const
{
    if(!isCompressed())
        throw std::logic_error("File is already uncompressed");

    FileHandler resultFile {outputFilePath, "wb"};

    auto rollbackFile = [&resultFile, &outputFilePath] {
        const bool cantRollback = RollbackFile(outputFilePath, resultFile);
        assert(cantRollback && "Unable to rollback file correctly");
        return false;
    };

    BmpHeader header = getHeader();
    header.Signature = UNCOMPRESSED_SIGNATURE; // Specify 'BM' signature
    header.DataOffset = header.IndexOffset; // Revert Data Offset to position of Index
    header.IndexOffset = 0; // Specify Index Offset to 0 as RSVD for BMP files

    try
    {
        BmpInfoHeader infoHeader = getInfoHeader();
        const std::uint32_t compressedImageSize = infoHeader.ImageSize;

        const int padding = RawImageData::calculatePadding(static_cast<int>(infoHeader.Width));
        const auto whiteRowPattern = RowIndex::getWhiteRowPattern(static_cast<int>(infoHeader.Width));

        DynamicBitset pixelDataCompressed(getPixelData(), compressedImageSize);

        const std::size_t resultImageSize = infoHeader.Height * (infoHeader.Width + padding);
        std::vector<std::uint8_t> resultPixelData(resultImageSize, 0x00);
        std::uint8_t * currentRowPtr = resultPixelData.data();
        std::uint32_t currentBitPos = 0;

        if(progressNotifier)
            progressNotifier->init(0, static_cast<int>(infoHeader.Height));

        for(int rowIndex = 0; rowIndex < infoHeader.Height; ++rowIndex)
        {
            if(m_pImpl->getRowIndex()->testRowIsEmpty(rowIndex))
            {
                std::memcpy(currentRowPtr, whiteRowPattern.data(), whiteRowPattern.size());
                currentRowPtr += whiteRowPattern.size();
            }
            else
            {
                int numBytesRestored = 0;
                while(numBytesRestored < infoHeader.Width + padding)
                {
                    std::uint32_t block = BLACK_4PIXELS;
                    const bool b0 = pixelDataCompressed.test(currentBitPos++);
                    if(b0 == false)
                    {
                        block = WHITE_4PIXELS;
                    }
                    else
                    {
                        const bool b1 = pixelDataCompressed.test(currentBitPos++);
                        if(b1 == true)
                        {
                            for(int i = 0; i < sizeof(std::uint32_t) * DynamicBitset::BITS_PER_BLOCK; ++i)
                            {
                                if(pixelDataCompressed.test(currentBitPos++))
                                    block |= 1 << i;
                            }
                        }
                    }

                    std::memcpy(currentRowPtr, &block, sizeof(block));
                    currentRowPtr += sizeof(block);
                    numBytesRestored += sizeof(block);
                }
            }

            if(progressNotifier)
            {
                using namespace std::chrono_literals;
                std::this_thread::sleep_for(2ms); // For progress bar demonstration
                progressNotifier->notifyProgress(rowIndex);
            }
        }

        pixelDataCompressed.clear();

        if(!m_pImpl->copyUpToOffset(resultFile, header.DataOffset))
            return rollbackFile();

        infoHeader.ImageSize = resultImageSize;
        header.FileSize = resultFile.tell() + resultImageSize;

        // Write Pixel Data uncompressed
        if(resultFile.write(resultPixelData.data(), resultPixelData.size()) != 1)
            return rollbackFile();
        if(resultFile.tell() != header.FileSize)
            return rollbackFile();

        resultFile.seek(SEEK_SET);
        if(resultFile.write(&header, sizeof(BmpHeader)) != 1)
            return rollbackFile();
        if(resultFile.write(&infoHeader, sizeof(BmpInfoHeader)) != 1)
            return rollbackFile();
    } catch ( ... )
    {
        return rollbackFile();
    }
    return true;
}

}
