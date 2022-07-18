#include "include/VideoCutWindow/videocutwindow.h"
#include "./ui_videocutwindow.h"

#include <include/structs.h>

#include <QPushButton>
#include <QTimeEdit>
#include <QCloseEvent>
#include <QMediaPlayer>

VideoCutWindow::VideoCutWindow(QWidget * parent)
	: QWidget(parent)
	, ui(new Ui::VideoCutWindow)
{
	ui->setupUi(this);
	setWindowFlags(Qt::Window | Qt::MSWindowsFixedSizeDialogHint);
	mediaPlayer = new QMediaPlayer(this);
	mediaPlayer->setVideoOutput(ui->PlayerWidget);
	mediaPlayer->setAudioRole(QAudio::VideoRole);
	mediaPlayer->setNotifyInterval(10);
	ui->VideoSlider->setMaximum(videoSliderMax);
	ui->VideoSlider->setTickInterval(0);
	ui->VideoSlider->setPageStep(1);
	ui->VideoSlider->setSingleStep(1);
	ui->VideoSlider->repaint();
	connectSlots();
}

VideoCutWindow::~VideoCutWindow()
{
	delete ui;
}

void VideoCutWindow::resetAll()
{
	QTime zero(0, 0);
	ui->CurrentTime->setTime(zero);
	ui->EndTimeEdit->setTime(zero);
	ui->StartTimeEdit->setTime(zero);
	ui->videoLengthTimeEdit->setTime(zero);
	ui->VideoSlider->setValue(0);
}

void VideoCutWindow::loadVideo(Video const & video)
{
	currentVideo = &video;
	QUrl url(video.filePath);
	QMediaContent content(url);
	mediaPlayer->setMedia(content);
	ui->videoLengthTimeEdit->setTime(video.length);
}

void VideoCutWindow::setInitialStartAndEnd(QTime const & start, QTime const & end)
{
	ui->StartTimeEdit->setTime(start);
	ui->EndTimeEdit->setTime(end);
}

void VideoCutWindow::closeWindow(QTime start, QTime end, bool cancelled)
{
	emit newCutInformation(start, end, cancelled);
	mediaPlayer->pause();
	this->setVisible(false);
}

QTime VideoCutWindow::getTimeForVideoPosition(qint64 pos)
{
	return getTimeFromMS(static_cast<int>(pos));
}

QTime VideoCutWindow::getTimeFromMS(int ms)
{
	QTime zero(0, 0);
	QTime time = zero.addMSecs(ms);
	return time;
}

QTime VideoCutWindow::getCurrentPlaybackTime()
{
	return std::min(currentVideo->length, getTimeForVideoPosition(mediaPlayer->position()));
}

void VideoCutWindow::updateVideoSlider(qint64 playbackMs)
{
	double vidMS = getVidMS();
	playbackMs = std::min(playbackMs, static_cast<qint64>(vidMS));
	ui->VideoSlider->setValue(static_cast<int>(static_cast<double>(playbackMs) / vidMS * static_cast<double>(videoSliderMax)));
}

void VideoCutWindow::startTimeChanged(QTime t)
{
	QTime end = ui->EndTimeEdit->time();
	if (end < t) {
		ui->EndTimeEdit->setTime(t);
	}
}

void VideoCutWindow::endTimeChanged(QTime t)
{
	QTime start = ui->StartTimeEdit->time();
	if (start > t) {
		ui->StartTimeEdit->setTime(t);
	}
}

double VideoCutWindow::getVidMS()
{
	return currentVideo ? static_cast<double>(currentVideo->length.msecsSinceStartOfDay()) : 1e308;
}

void VideoCutWindow::confirm()
{
	closeWindow(ui->StartTimeEdit->time(), ui->EndTimeEdit->time(), false);
}

void VideoCutWindow::cancel()
{
	closeWindow(ui->StartTimeEdit->time(), ui->EndTimeEdit->time(), true);
}

void VideoCutWindow::positionUpdated(qint64 pos)
{
	pos = std::min(static_cast<qint64>(getVidMS()), pos);
	updateVideoSlider(pos);
	QTime now = getTimeForVideoPosition(pos);
	ui->CurrentTime->setTime(now);
}

void VideoCutWindow::currentTimeEdited()
{
	timeChanged(ui->CurrentTime->time());
}

void VideoCutWindow::startTimeEdited()
{
	QTime current = ui->StartTimeEdit->time();
	if (currentVideo) {
		if (current > currentVideo->length)
			current = currentVideo->length;
	}
	if (current > ui->EndTimeEdit->time())
		ui->EndTimeEdit->setTime(current);
	ui->StartTimeEdit->setTime(current);
}

void VideoCutWindow::endTimeEdited()
{
	QTime current = ui->EndTimeEdit->time();
	if (currentVideo) {
		if (current > currentVideo->length)
			current = currentVideo->length;
	}
	if (current < ui->StartTimeEdit->time())
		ui->StartTimeEdit->setTime(current);
	ui->EndTimeEdit->setTime(current);
}

void VideoCutWindow::sliderReleased()
{
	//    sliderPositionChanged(ui->VideoSlider->value());
	if (wasPlayingBeforeSliderWasPressed) {
		mediaPlayer->play();
		wasPlayingBeforeSliderWasPressed = false;
	}
}

void VideoCutWindow::sliderPressed()
{
	if (mediaPlayer->state() == QMediaPlayer::PlayingState) {
		wasPlayingBeforeSliderWasPressed = true;
	}
	mediaPlayer->pause();
}

void VideoCutWindow::sliderMoved(int pos)
{
	sliderPositionChanged(pos);
}

void VideoCutWindow::timeChanged(QTime time)
{
	if (!currentVideo)
		return;
	time = std::min(time, currentVideo->length);

	ui->CurrentTime->setTime(time);
	qint64 pos = time.msecsSinceStartOfDay();
	updateVideoSlider(pos);
	mediaPlayer->setPosition(pos);
}

void VideoCutWindow::sliderPositionChanged(int pos)
{
	double ratio = static_cast<double>(pos) / static_cast<double>(videoSliderMax - 1);
	double ms = ratio * getVidMS();
	QTime t = getTimeFromMS(static_cast<int>(ms));
	timeChanged(t);
}

void VideoCutWindow::setStart()
{
	QTime t = getCurrentPlaybackTime();
	ui->StartTimeEdit->setTime(t);
	startTimeChanged(t);
}

void VideoCutWindow::setEnd()
{
	QTime t = getCurrentPlaybackTime();
	ui->EndTimeEdit->setTime(t);
	endTimeChanged(t);
}

void VideoCutWindow::closeEvent(QCloseEvent * event)
{
	event->accept();
	closeWindow(ui->StartTimeEdit->time(), ui->EndTimeEdit->time(), true);
}

void VideoCutWindow::connectSlots()
{
	QObject::connect(ui->confirmButton, &QPushButton::clicked, this, &VideoCutWindow::confirm);
	QObject::connect(ui->cancelButton, &QPushButton::clicked, this, &VideoCutWindow::cancel);

	QObject::connect(ui->PlayButton, &QPushButton::clicked, mediaPlayer, &QMediaPlayer::play);
	QObject::connect(ui->PauseButton, &QPushButton::clicked, mediaPlayer, &QMediaPlayer::pause);

	QObject::connect(mediaPlayer, &QMediaPlayer::positionChanged, this, &VideoCutWindow::positionUpdated);

	QObject::connect(ui->StartButton, &QPushButton::clicked, this, &VideoCutWindow::setStart);
	QObject::connect(ui->EndButton, &QPushButton::clicked, this, &VideoCutWindow::setEnd);

	QObject::connect(ui->CurrentTime, &QTimeEdit::editingFinished, this, &VideoCutWindow::currentTimeEdited);

	QObject::connect(ui->StartTimeEdit, &QTimeEdit::editingFinished, this, &VideoCutWindow::startTimeEdited);
	QObject::connect(ui->EndTimeEdit, &QTimeEdit::editingFinished, this, &VideoCutWindow::endTimeEdited);

	QObject::connect(ui->VideoSlider, &QSlider::sliderReleased, this, &VideoCutWindow::sliderReleased);
	QObject::connect(ui->VideoSlider, &QSlider::sliderPressed, this, &VideoCutWindow::sliderPressed);
	QObject::connect(ui->VideoSlider, &QSlider::sliderMoved, this, &VideoCutWindow::sliderMoved);

}

