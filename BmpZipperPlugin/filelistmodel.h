#pragma once

#include <QAbstractListModel>
#include <QDir>
#include <qqmlintegration.h>

class QFileSystemWatcher;
#ifdef Q_OS_LINUX
class QTimer;
#endif

namespace BmpZipper::Ui
{

class FileListModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QString folder READ getFolder WRITE setFolder NOTIFY folderChanged)
    QML_ELEMENT

    enum FileRoles
    {
        FileNameRole = Qt::UserRole + 1,
        FilePathRole,
        FileSizeRole
    };

public:
    explicit FileListModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent) const override;

    QVariant data(const QModelIndex& index, int role) const override;

    const QString& getFolder() const;

    void setFolder(const QString& folderPath);

    QHash<int, QByteArray> roleNames() const override;

signals:
    void folderChanged();

private slots:
    void onDirectoryChanged(const QString& path);
#ifdef Q_OS_LINUX
    void onTimerTimeout();
#endif

private:
    QFileInfoList m_fileInfoList;
    QString m_folder;
    QFileSystemWatcher* m_watcher;
#ifdef Q_OS_LINUX
    QTimer* m_timer;
#endif

};
}
