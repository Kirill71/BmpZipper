#include "compressionmodel.h"
#include "BmpLib/bmpproxy.h"
#include "BmpLib/bmpexceptions.h"

#include <QDir>
#include <thread>

namespace {

QString changeFileExtension(const QString& filePath, const QString& newExtension)
{
    const QFileInfo fileInfo(filePath);
    const QString baseName = fileInfo.completeBaseName();
    const QDir dir = fileInfo.dir();
    const QString newFilePath = dir.absoluteFilePath(baseName + newExtension);

    return newFilePath;
}

bool removeFileIfExists(const QString& filePath)
{
    const QFileInfo fileInfo(filePath);

    if (fileInfo.exists() && fileInfo.isFile())
    {
        QFile file(filePath);
        return file.remove();
    }

    return true;
}

QString getUniqueFilePath(const QString& filePath, const QString& newExtension) {
    const QFileInfo fileInfo(filePath);
    const QString baseName = fileInfo.completeBaseName();
    const QString dir = fileInfo.absolutePath();

    const QString suffix = "_unpacked";

    auto newFilePath = QString("%1/%2%3%4").arg(dir, baseName, suffix, newExtension);

    return newFilePath;
}

}

namespace BmpZipper::Ui {

CompressionModel::CompressionModel(QObject* parent)
    : QObject(parent)
{
}

void CompressionModel::compress(const QString& filePath)
{
    const QString extension = ".barch";
    const auto outFilePath = changeFileExtension(filePath, extension);
    if(!removeFileIfExists(outFilePath))
    {
        emit errorOccured("Unable to complete operation");
        return;
    }

    auto progressModel = m_progressModel;
    assert(progressModel);

    std::thread worker {[this, filePath, outFilePath, progressModel]
    {
        bool compressed = false;
        QString errorMsg = "Unable to compress file";

        try
        {
            progressModel->setText("Compressing");
            const auto bmpImage = BmpProxy::createFromBmp(filePath.toStdString());
            compressed = bmpImage.compress(outFilePath.toStdString(), progressModel);
        }
        catch(const FileError& _err)
        {
            errorMsg = _err.what();
        }
        catch( ... )
        {
            errorMsg = "Unexpected Error";
        }

        if(!compressed)
        {
            emit errorOccured(errorMsg);
        }
    }};
    worker.detach();
}

void CompressionModel::decompress(const QString& filePath)
{
    const QString extension = ".bmp";
    auto outFilePath = getUniqueFilePath(filePath, extension);

    auto progressModel = m_progressModel;
    assert(progressModel);

    std::thread worker {[this, filePath, outFilePath, progressModel]
    {
        bool decompressed = false;
        QString errorMsg = "Unable to decompress file";

        try
        {
            progressModel->setText("Decompressing");
            const auto barchImage = BmpProxy::createFromBarch(filePath.toStdString());
            decompressed = barchImage.decompress(outFilePath.toStdString(), progressModel);
        }
        catch(const FileError& err)
        {
            errorMsg = err.what();
        }
        catch( ... )
        {
            errorMsg = "Unexpected Error";
        }

        if(!decompressed)
        {
            emit errorOccured(errorMsg);
        }
    }};
    worker.detach();
}

ProgressModel* CompressionModel::getProgressModel()
{
    return m_progressModel;
}

void CompressionModel::setProgressModel(ProgressModel* model)
{
    m_progressModel = model;
}

}
