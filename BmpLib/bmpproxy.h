#pragma once

#include <cstdint>
#include <string>
#include <memory>

namespace BmpZipper {

struct IProgressNotifier;
struct BmpHeader;
struct BmpInfoHeader;
struct RawImageData;

class BmpProxy
{
public:
    [[nodiscard]]
    static BmpProxy createFromBmp(const std::string& filePath);

    [[nodiscard]]
    static BmpProxy createFromBarch(const std::string& filePath);

    BmpProxy(BmpProxy&& other) noexcept;
    BmpProxy& operator= (const BmpProxy& other) = delete;
    ~BmpProxy() noexcept;

    [[nodiscard]]
    const std::string& getFilePath() const;

    [[nodiscard]]
    const BmpHeader& getHeader() const;

    [[nodiscard]]
    const BmpInfoHeader& getInfoHeader() const;

    [[nodiscard]]
    bool isCompressed() const;

    [[nodiscard]]
    std::size_t getWidth() const;

    [[nodiscard]]
    std::size_t getHeight() const;

    [[nodiscard]]
    std::uint8_t* getPixelData();

    [[nodiscard]]
    const std::uint8_t * getPixelData() const;

    [[nodiscard]]
    bool provideRawImageData(RawImageData& out) const;

    [[nodiscard]]
    bool compress(const std::string& outputFilePath, IProgressNotifier* progressNotifier = nullptr) const;

    [[nodiscard]]
    bool decompress(const std::string& outputFilePath, IProgressNotifier * progressNotifier = nullptr) const;

private:
    struct ProxyImpl;
    std::unique_ptr<ProxyImpl> m_pImpl;
    explicit BmpProxy(std::unique_ptr<ProxyImpl> pImpl);
};

}
