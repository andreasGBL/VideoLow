#include "include/videolowwindow.h"
#include "include/videolowpaths.h"

#include <QApplication>
#include <QByteArray>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>
#include <QSaveFile>
#include <QDateTime>

namespace {
bool setModificationTime(const QString& filePath, const QDateTime& modified, QString& error)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadWrite)) {
        error = "Can't open " + filePath + ": " + file.errorString();
        return false;
    }
    if (!file.setFileTime(modified, QFileDevice::FileModificationTime)) {
        error = "Can't set modification time for " + filePath + ": " + file.errorString();
        return false;
    }
    return true;
}

bool extractExecutable(const QString& fileName, QString& error)
{
    const QString compressedPath = QDir(QApplication::applicationDirPath()).filePath(fileName + ".qcomp");
    const QString outputDirectory = VideoLowPaths::temporaryDirectory();
    const QString outputPath = QDir(outputDirectory).filePath(fileName);
    if (!QDir().mkpath(outputDirectory)) {
        error = "Can't create " + outputDirectory;
        return false;
    }

    QFileInfo compressedInfo(compressedPath);
    if (!compressedInfo.isFile()) {
        error = "Can't find " + compressedPath;
        return false;
    }
    const QDateTime compressedModified = compressedInfo.lastModified();
    QFileInfo outputInfo(outputPath);
    if (outputInfo.isFile() && outputInfo.lastModified() >= compressedModified)
        return setModificationTime(outputPath, compressedModified, error);

    QFile source(compressedPath);
    if (!source.open(QIODevice::ReadOnly)) {
        error = "Can't read " + compressedPath + ": " + source.errorString();
        return false;
    }
    QByteArray packed = source.readAll();
    if (source.error() != QFileDevice::NoError) {
        error = "Can't read " + compressedPath + ": " + source.errorString();
        return false;   
    }
    QByteArray contents = qUncompress(packed);
    if (contents.isEmpty()) {
        error = "Can't decompress " + compressedPath;
        return false;
    }

    QSaveFile output(outputPath);
    if (!output.open(QIODevice::WriteOnly)) {
        error = "Can't write " + outputPath + ": " + output.errorString();
        return false;
    }
    if (output.write(contents) != contents.size() || !output.commit()) {
        error = "Can't save " + outputPath + ": " + output.errorString();
        return false;
    }
    return setModificationTime(outputPath, compressedModified, error);
}
}

int main(int argc, char *argv[])
{
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication a(argc, argv);
    QString startupError;
    if (!extractExecutable("ffmpeg.exe", startupError) ||
        !extractExecutable("ffprobe.exe", startupError)) {
        QMessageBox::critical(nullptr, "Startup Error", startupError);
        return 1;
    }
    VideoLowWindow w;
    w.show();
    return a.exec();
}
