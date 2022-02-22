#include "./ui_videolowwindow.h"
#include <QMainWindow>
#include <QString>


#include "include/structs.h"

#include <iostream>


VideoLowWindow::VideoLowWindow(QWidget * parent)
	: QMainWindow(parent)
	, ffmpeg()
	, ui(new Ui::VideoLowWindow)
{
	ui->setupUi(this);
	ui->HardwareAccelerationComboBox->setCurrentIndex(HARDWARE_ACCELERATION_DEFAULT);
	ui->HardwareAccelerationQuickComboBox->setCurrentIndex(HARDWARE_ACCELERATION_DEFAULT);
	ui->HardwareAccelerationComboBox->update();
	ui->DropWidget->setVideoLowWindowPointer(this);
	ui->DropWidget->setPreviewLabelPointer(ui->PreviewLabel);
	ui->PreviewLabel->setDropWidgetPointer(ui->DropWidget);
	cutWindow = new VideoCutWindow(this);
	connectSlots();
	setWindowFlags(Qt::Window | Qt::MSWindowsFixedSizeDialogHint);
	this->statusBar()->setSizeGripEnabled(false);
}

VideoLowWindow::~VideoLowWindow()
{
	delete ui;
	if (currentVideo)
		delete currentVideo;
	delete cutWindow;
}

void VideoLowWindow::connectSlots()
{
	QObject::connect(ui->H264_2, &QPushButton::clicked, this, &VideoLowWindow::quickH264_2);
	QObject::connect(ui->H264_4, &QPushButton::clicked, this, &VideoLowWindow::quickH264_4);
	QObject::connect(ui->H264_8, &QPushButton::clicked, this, &VideoLowWindow::quickH264_8);
	QObject::connect(ui->H264_16, &QPushButton::clicked, this, &VideoLowWindow::quickH264_16);

	QObject::connect(ui->HEVC_2, &QPushButton::clicked, this, &VideoLowWindow::quickHEVC_2);
	QObject::connect(ui->HEVC_4, &QPushButton::clicked, this, &VideoLowWindow::quickHEVC_4);
	QObject::connect(ui->HEVC_8, &QPushButton::clicked, this, &VideoLowWindow::quickHEVC_8);
	QObject::connect(ui->HEVC_16, &QPushButton::clicked, this, &VideoLowWindow::quickHEVC_16);

	QObject::connect(ui->ExportButton, &QPushButton::clicked, this, &VideoLowWindow::exportVideo);

	QObject::connect(ui->reviewVideoButton, &QPushButton::clicked, this, &VideoLowWindow::reviewVideo);
	QObject::connect(cutWindow, &VideoCutWindow::newCutInformation, this, &VideoLowWindow::gotCutInformation);

	QObject::connect(ui->startTimeEdit, &QTimeEdit::editingFinished, this, &VideoLowWindow::startTimeEdited);
	QObject::connect(ui->EndTimeEdit, &QTimeEdit::editingFinished, this, &VideoLowWindow::endTimeEdited);
}

void VideoLowWindow::quickH264(double MBitRate)
{
	if (currentVideo)
		handleExportExitCode(
			ffmpeg.exportFile(
				*currentVideo,
				getTrimSettings(),
				MBitRate,
				RESOLUTIONS[RESOLUTION_IDX::RESOLUTION_AS_INPUT],
				CODECS[CODEC_IDX::H264],
				ui->HardwareAccelerationQuickComboBox->currentIndex(),
				FRAMERATES[ui->FramerateQuickComboBox->currentIndex()]
			),
			ui->HardwareAccelerationQuickComboBox->currentIndex() != 0
		);
}

void VideoLowWindow::quickHEVC(double MBitRate)
{
	if (currentVideo) {
		handleExportExitCode(
			ffmpeg.exportFile(
				*currentVideo,
				getTrimSettings(),
				MBitRate,
				RESOLUTIONS[RESOLUTION_IDX::RESOLUTION_AS_INPUT],
				CODECS[CODEC_IDX::HEVC],
				ui->HardwareAccelerationQuickComboBox->currentIndex(),
				FRAMERATES[ui->FramerateQuickComboBox->currentIndex()]
			),
			ui->HardwareAccelerationQuickComboBox->currentIndex() != 0
		);

	}
}

void VideoLowWindow::handleExportExitCode(bool success, bool hardwareAcc)
{
	QString text("Error during last Export!");
	if (hardwareAcc)
		text += " Try updating your drivers or try different settings!";
	if (!success) {
		std::cout << "Error during export!" << std::endl;
		ui->ErrorLabel->setText(tr(text.toStdString().c_str()));
	}
	else {
		std::cout << "Successful export!" << std::endl;
		ui->ErrorLabel->clear();
	}
}

TrimSettings VideoLowWindow::getTrimSettings()
{
	QTime start = ui->startTimeEdit->time();
	QTime end = ui->EndTimeEdit->time();
	bool trim = ui->trimVideoCheckBox->isChecked();
	return TrimSettings{ start, end, trim };
}

void VideoLowWindow::quickH264_2()
{
	std::cout << "H264_2" << std::endl;
	quickH264(2.0);
}
void VideoLowWindow::quickH264_4()
{
	std::cout << "H264_4" << std::endl;
	quickH264(4.0);
}
void VideoLowWindow::quickH264_8()
{
	std::cout << "H264_8" << std::endl;
	quickH264(8.0);
}
void VideoLowWindow::quickH264_16()
{
	std::cout << "H264_16" << std::endl;
	quickH264(16.0);
}
void VideoLowWindow::quickHEVC_2()
{
	std::cout << "HEVC_2" << std::endl;
	quickHEVC(2.0);
}
void VideoLowWindow::quickHEVC_4()
{
	std::cout << "HEVC_4" << std::endl;
	quickHEVC(4.0);
}
void VideoLowWindow::quickHEVC_8()
{
	std::cout << "HEVC_8" << std::endl;
	quickHEVC(8.0);
}

void VideoLowWindow::quickHEVC_16()
{
	std::cout << "HEVC_16" << std::endl;
	quickHEVC(16.0);
}

void VideoLowWindow::exportVideo()
{
	std::cout << "export" << std::endl;
	if (currentVideo) {
		handleExportExitCode(
			ffmpeg.exportFile(
				*currentVideo,
				getTrimSettings(),
				ui->BitrateDoubleSpinBox->value(),
				RESOLUTIONS[ui->ResolutionComboBox->currentIndex()],
				CODECS[ui->CodecComboBox->currentIndex()],
				ui->HardwareAccelerationComboBox->currentIndex(),
				FRAMERATES[ui->FramerateComboBox->currentIndex()]
			),
			ui->HardwareAccelerationComboBox->currentIndex() != 0
		);
	}
}

void VideoLowWindow::reviewVideo()
{
	cutWindow->resetAll();
	cutWindow->loadVideo(*this->currentVideo);
	cutWindow->setInitialStartAndEnd(ui->startTimeEdit->time(), ui->EndTimeEdit->time());
	cutWindow->show();
}

void VideoLowWindow::gotCutInformation(QTime start, QTime end, bool cancelled)
{
	if (!cancelled) {
		ui->startTimeEdit->setTime(start);
		ui->EndTimeEdit->setTime(end);
		ui->trimVideoCheckBox->setChecked(true);
	}
}

void VideoLowWindow::startTimeEdited()
{
	QTime current = ui->startTimeEdit->time();
	if (currentVideo) {
		if (current > currentVideo->length)
			current = currentVideo->length;
	}
	if (current > ui->EndTimeEdit->time())
		ui->EndTimeEdit->setTime(current);
	ui->startTimeEdit->setTime(current);
}

void VideoLowWindow::endTimeEdited()
{
	QTime current = ui->EndTimeEdit->time();
	if (currentVideo) {
		if (current > currentVideo->length)
			current = currentVideo->length;
	}
	if (current < ui->startTimeEdit->time())
		ui->startTimeEdit->setTime(current);
	ui->EndTimeEdit->setTime(current);
}

void VideoLowWindow::newVideoFile(Video vid)
{
	if (!currentVideo) {
		currentVideo = new Video(vid);
	}
	else
		*currentVideo = vid;
	QTime zero(0, 0);
	ui->reviewVideoButton->setDisabled(false);
	ui->trimVideoCheckBox->setChecked(false);
	ui->startTimeEdit->setTime(zero);
	ui->EndTimeEdit->setTime(vid.length);
	ui->videoLengthTimeEdit->setTime(vid.length);
}

