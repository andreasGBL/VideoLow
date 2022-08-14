#include "include/videolowwindow.h"
#include "./ui_videolowwindow.h"

#include "include/VideoCutWindow/videocutwindow.h"
#include "include/ffmpegwrapper.h"

#include "include/structs.h"

#include <QString>
#include <iostream>


//TODO: add vertical Video option

VideoLowWindow::VideoLowWindow(QWidget* parent)
	: QMainWindow(parent)
	, ffmpeg(new FFMPEGWrapper())
	, ui(new Ui::VideoLowWindow())
	, cutWindow(new VideoCutWindow())
{

	ui->setupUi(this);
	ui->centralwidget->resize(width() - 10, height() - 10);
	ui->HardwareAccelerationComboBox->setCurrentIndex(HARDWARE_ACCELERATION_DEFAULT);
	ui->HardwareAccelerationQuickComboBox->setCurrentIndex(HARDWARE_ACCELERATION_DEFAULT);
	ui->HardwareAccelerationComboBox->update();
	//ui->DropWidget->setVideoLowWindowPointer(this);
	ui->DropWidget->setPreviewLabelPointer(ui->PreviewLabel);
	ui->PreviewLabel->setDropWidgetPointer(ui->DropWidget);
	connectSlots();
	setWindowFlags(Qt::Window | Qt::MSWindowsFixedSizeDialogHint);
	this->statusBar()->setSizeGripEnabled(false);
	codecConfigChanged(0);
}

VideoLowWindow::~VideoLowWindow()
{
	delete ui;
	if (currentVideo)
		delete currentVideo;

	delete cutWindow;
	delete ffmpeg;
}

void VideoLowWindow::connectSlots()
{
	QObject::connect(ui->trimVideoCheckBox, &QCheckBox::toggled, ui->trimVideoButton, &QPushButton::setEnabled);
	QObject::connect(ui->CodecComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &VideoLowWindow::codecConfigChanged);
	QObject::connect(ui->HardwareAccelerationComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &VideoLowWindow::codecConfigChanged);
	QObject::connect(ui->trimVideoButton, &QPushButton::clicked, this, &VideoLowWindow::quickTrimOnly);
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

	QObject::connect(ui->DropWidget, &FileDropWidget::newVideoFileDropped, this, &VideoLowWindow::newVideoFile);
}

CodecConfig VideoLowWindow::getCodecConfig()
{
	CodecConfig codec = DefaultCodecs[CODEC_IDX(ui->CodecComboBox->currentIndex())][HARDWARE_ACCELERATION(ui->HardwareAccelerationComboBox->currentIndex())];
	codec.profile = ui->ProfileComboBox->currentIndex();
	return codec;
}

void VideoLowWindow::quickH264(double MBitRate)
{
	if (currentVideo) {
		CodecConfig h264 = DefaultCodecs[CODEC_IDX::H264][HARDWARE_ACCELERATION(ui->HardwareAccelerationQuickComboBox->currentIndex())];
		h264.profile = h264.mainProfile;
		handleExportExitCode(
			ffmpeg->exportFile(
				*currentVideo,
				getTrimSettings(),
				MBitRate,
				RESOLUTIONS[RESOLUTION_IDX::RESOLUTION_AS_INPUT],
				h264,
				FRAMERATES[ui->FramerateQuickComboBox->currentIndex()],
				false,
				false
			),
			ui->HardwareAccelerationQuickComboBox->currentIndex() != 0
		);
	}
}

void VideoLowWindow::quickHEVC(double MBitRate)
{
	if (currentVideo) {
		CodecConfig hevc = DefaultCodecs[CODEC_IDX::HEVC][HARDWARE_ACCELERATION(ui->HardwareAccelerationQuickComboBox->currentIndex())];
		hevc.profile = hevc.mainProfile;
		handleExportExitCode(
			ffmpeg->exportFile(
				*currentVideo,
				getTrimSettings(),
				MBitRate,
				RESOLUTIONS[RESOLUTION_IDX::RESOLUTION_AS_INPUT],
				hevc,
				FRAMERATES[ui->FramerateQuickComboBox->currentIndex()],
				false,
				false
			),
			ui->HardwareAccelerationQuickComboBox->currentIndex() != 0
		);

	}
}

void VideoLowWindow::handleExportExitCode(bool success, bool hardwareAcc)
{
	QString text("Error during Export!");
	if (hardwareAcc)
		text += " Update your drivers or try other settings!";
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


//TODO: replace quick Bitrate options with crf quality options
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
		auto codec = getCodecConfig();
		handleExportExitCode(
			ffmpeg->exportFile(
				*currentVideo,
				getTrimSettings(),
				ui->BitrateDoubleSpinBox->value(),
				RESOLUTIONS[ui->ResolutionComboBox->currentIndex()],
				codec,
				FRAMERATES[ui->FramerateComboBox->currentIndex()],
				false,
				ui->verticalVideoCheckbox->isChecked()
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

void VideoLowWindow::quickTrimOnly()
{
	if (currentVideo) {
		CodecConfig codec = DefaultCodecs[CODEC_IDX::H264][HARDWARE_ACCELERATION::NONE]; //default parameters as it doesnt matter for this trimming
		handleExportExitCode(
			ffmpeg->exportFile(
				*currentVideo,
				getTrimSettings(),
				ui->BitrateDoubleSpinBox->value(),
				RESOLUTIONS[RESOLUTION_IDX::RESOLUTION_AS_INPUT],
				codec,
				FRAMERATES[0],
				true,
				false
			),
			ui->HardwareAccelerationQuickComboBox->currentIndex() != 0
		);
	}
}

void VideoLowWindow::codecConfigChanged(int)
{
	auto codec = DefaultCodecs[CODEC_IDX(ui->CodecComboBox->currentIndex())][HARDWARE_ACCELERATION(ui->HardwareAccelerationComboBox->currentIndex())];
	int count = ui->ProfileComboBox->count();
	for (int i = 0; i < count; i++) {
		ui->ProfileComboBox->removeItem(0);
	}
	auto const& profiles = codec.profiles;
	for (auto const& profile : profiles) {
		ui->ProfileComboBox->addItem(profile);
	}
	ui->ProfileComboBox->setCurrentIndex(codec.profile);
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

void VideoLowWindow::newVideoFile(Video const& vid)
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
	if (vid.resolution.width && vid.resolution.height)
		ui->videoResolutionLabel->setText(QString::number(vid.resolution.width) + "x" + QString::number(vid.resolution.height));
	else
		ui->videoResolutionLabel->setText("-");
	if (vid.framerate)
		ui->videoFramerateLabel->setText(QString::number(vid.framerate, 'f', 2) + " fps");
	else
		ui->videoFramerateLabel->setText("-");

	ui->videoCodecLabel->setText(vid.codec.name + " (" + vid.codec.profiles[vid.codec.profile] + ")");
	ui->videoBitrateLabel->setText(vid.bitrate == 0. ? QString("-") : QString::number(vid.bitrate, 'f', 2) + " MBit/s");
}


