#pragma once

#include <QWidget>
#include <QTime>

class QMediaPlayer;
struct Video;

QT_BEGIN_NAMESPACE
namespace Ui { class VideoCutWindow; }
QT_END_NAMESPACE

class VideoCutWindow : public QWidget
{
	Q_OBJECT

public:
	VideoCutWindow(QWidget * parent = nullptr);
	~VideoCutWindow();

	void resetAll();
	void loadVideo(Video const & video);
	void setInitialStartAndEnd(QTime const & start, QTime const & end);
	void closeEvent(QCloseEvent * event) override;

signals:
	void newCutInformation(QTime start, QTime end, bool cancelled);
private slots:

	void confirm();
	void cancel();

	void positionUpdated(qint64 pos);
	void currentTimeEdited();
	void startTimeEdited();
	void endTimeEdited();

	void sliderReleased();
	void sliderPressed();
	void sliderMoved(int pos);

	void setStart();
	void setEnd();



private:
	void connectSlots();
	void closeWindow(QTime start, QTime end, bool cancelled);

	QTime getTimeForVideoPosition(qint64 pos);
	QTime getTimeFromMS(int ms);
	QTime getCurrentPlaybackTime();

	void timeChanged(QTime time);

	void updateVideoSlider(qint64 playbackMs);
	void sliderPositionChanged(int pos);

	void startTimeChanged(QTime t);
	void endTimeChanged(QTime t);

	double getVidMS();

	Ui::VideoCutWindow * ui;
	QMediaPlayer * mediaPlayer;
	Video const * currentVideo;
	const int videoSliderMax = 1000;
	bool wasPlayingBeforeSliderWasPressed = false;
};