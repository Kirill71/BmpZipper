#pragma once

#include "progressmodel.h"

#include <QObject>
#include <qqmlintegration.h>

namespace BmpZipper::Ui {

class CompressionModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(ProgressModel* progressModel READ getProgressModel WRITE setProgressModel)
    QML_ELEMENT

public:
    explicit CompressionModel(QObject* parent = nullptr);

    Q_INVOKABLE void compress(const QString& filePath);
    Q_INVOKABLE void decompress(const QString& filePath);

signals:
    void errorOccured(const QString& text);

private:
    void setProgressModel(ProgressModel* model);
    ProgressModel* getProgressModel();

private:
    ProgressModel* m_progressModel = nullptr;
};

}
