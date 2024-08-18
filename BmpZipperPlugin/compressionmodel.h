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
    explicit CompressionModel(QObject* _parent = nullptr);

    Q_INVOKABLE void compress(const QString& _filePath);
    Q_INVOKABLE void decompress(const QString& _filePath);

signals:
    void errorOccured(const QString& _text);

private:
    void setProgressModel(ProgressModel* _model);
    ProgressModel* getProgressModel();

private:
    ProgressModel* m_progressModel = nullptr;
};

}
