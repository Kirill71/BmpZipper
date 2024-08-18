#pragma once

#include <cstdio>
#include <string>

namespace BmpZipper
{

class FileHandler final
{
    friend void swap(FileHandler& lhs, FileHandler& rhs) noexcept;
public:
    FileHandler();
    FileHandler(const std::string& filePath, const char* mode);
    explicit FileHandler(FILE* handler);

    FileHandler(const FileHandler&) = delete;
    FileHandler& operator= (const FileHandler&) = delete;

    FileHandler(FileHandler&& fileHandler) noexcept;
    FileHandler& operator= (FileHandler&& fileHandler) noexcept;

    ~FileHandler();

    int seek(int origin, long offset = 0L);
    long tell();

    bool close();

    std::size_t read(void* buffer, std::size_t size, std::size_t count = 1 );
    std::size_t write(const void* buffer, std::size_t size, std::size_t count = 1 );

    explicit operator FILE* ();  // NOLINT

private:
    FILE* m_fileHandler;
};

inline void swap(FileHandler &lhs, FileHandler &rhs) noexcept
{
    // enables ADL look up
    using std::swap;
    swap(lhs.m_fileHandler, rhs.m_fileHandler);
}

}
