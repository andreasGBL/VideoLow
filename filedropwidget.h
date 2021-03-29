#pragma once

#include <QLabel>
#include "videolowwindow.h"

class QMimeData;

namespace acceptedFileTypes {
    QString const MP4 = "mp4";
    QString const ALL[1] = {MP4};
}

class FileDropWidget : public QLabel
{
    Q_OBJECT
public:
    explicit FileDropWidget(QWidget *parent = nullptr);
    void setVideoLowWindowPointer(VideoLowWindow const * vlw);
public slots:
    void clear();

signals:
    void newVideoFileDropped(const QString filePath);

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dragLeaveEvent(QDragLeaveEvent *event) override;
    void dropEvent(QDropEvent *event) override;
private:
    bool hasDrop = false;
    bool acceptMimeType(QMimeData const * mimeData);
    QString breakLines(QString str, int maxChars);

    VideoLowWindow const * videoLowWindow;
    void connectSlots();
};

