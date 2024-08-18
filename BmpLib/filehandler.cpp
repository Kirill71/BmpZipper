#include "filehandler.h"
#include "bmpexceptions.h"

namespace BmpZipper {
FileHandler::FileHandler()
    : m_fileHandler{nullptr}
{
}

FileHandler::FileHandler(const std::string& filePath, const char* mode)
    : m_fileHandler{std::fopen(filePath.c_str(), mode)}
{
    if (!m_fileHandler) throw FileDoesntExistError(filePath);
}

FileHandler::FileHandler(FILE* handler)
    : m_fileHandler(handler)
{
    if (!m_fileHandler) throw FileDoesntExistError("");
}

FileHandler::FileHandler(FileHandler&& fileHandler) noexcept
    : m_fileHandler{nullptr}
{
    swap(fileHandler, *this);
}

FileHandler& FileHandler::operator= (FileHandler&& fileHandler) noexcept
{
   FileHandler temp {std::move(fileHandler)};
   swap(temp, *this);
   return *this;
}

FileHandler::~FileHandler()
{
    close();
}

int FileHandler::seek(const int origin, const long offset)
{
    return std::fseek(m_fileHandler, offset, origin);
}

long FileHandler::tell()
{
    return std::ftell(m_fileHandler);
}

bool FileHandler::close()
{
    if (!m_fileHandler) return false;
    const bool closed = std::fclose(m_fileHandler) == 0;
    m_fileHandler = nullptr;
    return closed;
}

std::size_t FileHandler::read(void* buffer, const std::size_t size, const std::size_t count)
{
    return std::fread(buffer, size, count, m_fileHandler);
}

std::size_t FileHandler::write(const void* buffer, const std::size_t size, const std::size_t count)
{
    return std::fwrite(buffer, size, count, m_fileHandler);
}

FileHandler::operator FILE* () // NOLINT
{
    return m_fileHandler;
}

}