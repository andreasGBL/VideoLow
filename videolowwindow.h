#pragma once

class QString;
#include "ffmpegwrapper.h"
#include "Structs.h"
#include "VideoCutWindow/videocutwindow.h"
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
    void reviewVideo();
    void gotCutInformation(QTime start, QTime end, bool cancelled);
    void startTimeEdited();
    void endTimeEdited();

public slots:
    void newVideoFile(Video vid);

public:
    VideoLowWindow(QWidget *parent = nullptr);
    ~VideoLowWindow();
    void connectSlots();

private:
    void quickH264(double MBitRate);
    void quickHEVC(double MBitRate);
    void handleExportExitCode(bool success, bool hardwareAcc);

    TrimSettings getTrimSettings();
    FFMPEGWrapper ffmpeg;
    Ui::VideoLowWindow *ui;
    VideoCutWindow *cutWindow;
    Video *currentVideo = nullptr;
};


