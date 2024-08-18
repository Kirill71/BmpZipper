#include "bmpexceptions.h"

namespace BmpZipper {

FileError::FileError(const std::string& message)
    : std::runtime_error(message)
{
}

FileDoesntExistError::FileDoesntExistError(const std::string& filePath)
    : FileError("File Error: " + filePath + " doesn't exist")
{
}

FileCreationError::FileCreationError(const std::string& filePath)
    : FileError("File Error: Unable to create " + filePath + " file")
{
}

InvalidBmpHeaderError::InvalidBmpHeaderError(const std::string& message)
    : FileError(message.empty()
        ? std::string("Invalid BMP Header")
        : std::string("Invalid BMP Header: " ) + message
)
{
}

InvalidInfoHeaderError::InvalidInfoHeaderError(const std::string& message)
    : FileError(message.empty()
        ? std::string("Invalid Info Header")
        : std::string("Invalid Info Header: ") + message
)
{
}

InvalidPixelDataError::InvalidPixelDataError(const std::string& message)
    : FileError(message.empty()
         ? std::string("Invalid Pixel Data")
         : std::string("Invalid Pixel Data: ") + message
)
{
}

}