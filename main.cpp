// Copyright PocketBook - Interview Task

#include <QtQuick/QQuickView>
#include <QGuiApplication>
#include <QQmlEngine>
#include <QQuickStyle>
#include <QQmlContext>
#include <QDir>
#include <QFileInfo>
#include <QCommandLineParser>
#include <QCommandLineOption>

int main(int argc, char *argv[])
{
    const QGuiApplication app(argc, argv);
    QQuickStyle::setStyle("Basic");

    QCommandLineParser parser;
    parser.setApplicationDescription("Description: BmpZipper tool for compressing/decompressing 8bit '*.bmp' files");
    parser.addHelpOption();
    parser.addVersionOption();
    const QCommandLineOption dirOption(
        QStringList() << "d" << "dir"
        ,QCoreApplication::translate("main", "Scan bmp, barch and png files in <directory>.")
        ,QCoreApplication::translate("main", "directory"));
    parser.addOption(dirOption);

    parser.process(app);

    QString directoryToScanPath = QDir::currentPath();
    if(parser.isSet(dirOption))
    {
        const QString targetDir = parser.value(dirOption);
        const QFileInfo dirInfo(targetDir);
        if(dirInfo.exists() || dirInfo.isDir())
            directoryToScanPath = targetDir;
    }

    QQuickView view;
    QQmlContext* context = view.rootContext();
    context->setContextProperty("initialFolder", directoryToScanPath);
#ifdef Q_OS_MACOS
    view.engine()->addImportPath(app.applicationDirPath() + "/../PlugIns");
#endif

    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.loadFromModule("BmpZipperApp", "App");
    view.show();
    return QGuiApplication::exec();
}