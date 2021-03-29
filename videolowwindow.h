#pragma once

class QString;
#include "ffmpegwrapper.h"

#include <QMainWindow>


QT_BEGIN_NAMESPACE
namespace Ui { class VideoLowWindow; }
QT_END_NAMESPACE



class VideoLowWindow : public QMainWindow
{
    Q_OBJECT

private slots:
    void quickH264_2();
    void quickH264_4();
    void quickH264_8();
    void quickH264_16();
    void quickHEVC_2();
    void quickHEVC_4();
    void quickHEVC_8();
    void quickHEVC_16();
    void exportVideo();

public slots:
    void newVideoFile(QString filePath);

public:
    VideoLowWindow(QWidget *parent = nullptr);
    ~VideoLowWindow();
    void connectSlots();

private:
    void quickH264(double MBitRate);
    void quickHEVC(double MBitRate);
    void handleExportExitCode(bool success);

    FFMPEGWrapper ffmpeg;
    Ui::VideoLowWindow *ui;
    QString *currentFile = nullptr;
};


