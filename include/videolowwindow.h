#pragma once


class FFMPEGWrapper;
class VideoCutWindow;

struct CodecConfig;
struct TrimSettings;
struct Video;

class QTime;
class QString;
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
	void quickTrimOnly();
	void codecConfigChanged(int);
	void gotCutInformation(QTime start, QTime end, bool cancelled);
	void startTimeEdited();
	void endTimeEdited();
	void uncheckRemoveAudio(bool isAudioOnlyChecked);
	void uncheckAudioOnly(bool isRemoveAudioChecked);

public slots:
	void newVideoFile(Video const & vid);

public:
	VideoLowWindow(QWidget * parent = nullptr);
	~VideoLowWindow();
	void connectSlots();

private:
	CodecConfig getCodecConfig();
	void quickH264(double MBitRate);
	void quickHEVC(double MBitRate);
	void handleExportExitCode(bool success, bool hardwareAcc);
	void setQuickExportsEnabled(bool enabled);

	TrimSettings getTrimSettings();
	FFMPEGWrapper *ffmpeg = nullptr;
	Ui::VideoLowWindow * ui = nullptr;
	VideoCutWindow * cutWindow = nullptr;
	Video * currentVideo = nullptr;
};


