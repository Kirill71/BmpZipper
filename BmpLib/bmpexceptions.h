#pragma once

#include <stdexcept>
#include <string>

namespace BmpZipper
{

class FileError
        : public std::runtime_error
{
public:
    explicit FileError(const std::string& message);
};

class FileDoesntExistError
        : public FileError
{
public:
    explicit FileDoesntExistError(const std::string& filePath);
};

class FileCreationError
        : public FileError
{
public:
    explicit FileCreationError(const std::string& filePath);
};

class InvalidBmpHeaderError
        : public FileError
{
public:
    explicit InvalidBmpHeaderError(const std::string& message = std::string());
};

class InvalidInfoHeaderError
        : public FileError
{
public:
    explicit InvalidInfoHeaderError(const std::string& message = std::string());
};

class InvalidPixelDataError
        : public FileError
{
public:
    explicit InvalidPixelDataError(const std::string& message = std::string());
};

}
