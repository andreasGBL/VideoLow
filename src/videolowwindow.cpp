#include "include/videolowwindow.h"
#include "./ui_videolowwindow.h"

#include "include/VideoCutWindow/videocutwindow.h"
#include "include/ffmpegwrapper.h"

#include "include/structs.h"

#include <QString>
#include <iostream>

//TODO: "as Input" checkbox for bitrate

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
	ui->DropWidget->setPreviewLabelPointer(ui->PreviewLabel);
	ui->PreviewLabel->setDropWidgetPointer(ui->DropWidget);
	connectSlots();
	setWindowFlags(Qt::Window | Qt::MSWindowsFixedSizeDialogHint);
	this->statusBar()->setSizeGripEnabled(false);
	//fill Comboboxes with content
	for (int i = 0; i < AUDIO_CODEC::UNSUPPORTED_AUDIO; i++) {
		ui->AudioCodecComboBox->addItem(AUDIO_CODEC_STRINGS[i]);
	}
	for (int i = 0; i < VIDEO_CODEC::UNSUPPORTED_VIDEO; i++) {
		ui->CodecComboBox->addItem(VIDEO_CODEC_STRINGS[i]);
	}
	//load default codec settings for codec 0
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

	QObject::connect(ui->AudioOnlyCheckBox, &QCheckBox::toggled, this, &VideoLowWindow::uncheckRemoveAudio);
	QObject::connect(ui->RemoveAudioCheckBox, &QCheckBox::toggled, this, &VideoLowWindow::uncheckAudioOnly);

}

CodecConfig VideoLowWindow::getCodecConfig()
{
	CodecConfig codec = DefaultCodecs[VIDEO_CODEC(ui->CodecComboBox->currentIndex())][HARDWARE_ACCELERATION(ui->HardwareAccelerationComboBox->currentIndex())];
	codec.profile = ui->ProfileComboBox->currentIndex();
	codec.audioCodec = ui->AudioCodecComboBox->currentIndex();
	codec.audioCodecName = AUDIO_CODEC_STRINGS[codec.audioCodec];
	codec.audioEncoderName = AUDIO_ENCODER_STRINGS[codec.audioCodec];
	codec.audioBitrateIdx = ui->audioBitrateComboBox->currentIndex();
	return codec;
}

void VideoLowWindow::quickH264(double MBitRate)
{
	if (currentVideo) {
		CodecConfig h264 = DefaultCodecs[VIDEO_CODEC::H264][HARDWARE_ACCELERATION(ui->HardwareAccelerationQuickComboBox->currentIndex())];
		h264.profile = h264.mainProfile;
		ExportSettings exp = {
			MBitRate,
			FRAMERATES[ui->FramerateQuickComboBox->currentIndex()],
			false,
			false,
			false,
			false
		};
		handleExportExitCode(
			ffmpeg->exportFile(
				*currentVideo,
				getTrimSettings(),
				RESOLUTIONS[RESOLUTION_IDX::RESOLUTION_AS_INPUT],
				h264,
				exp
			),
			ui->HardwareAccelerationQuickComboBox->currentIndex() != 0
		);
	}
}

void VideoLowWindow::quickHEVC(double MBitRate)
{
	if (currentVideo) {
		CodecConfig hevc = DefaultCodecs[VIDEO_CODEC::HEVC][HARDWARE_ACCELERATION(ui->HardwareAccelerationQuickComboBox->currentIndex())];
		hevc.profile = hevc.mainProfile;
		ExportSettings exp = {
			MBitRate,
			FRAMERATES[ui->FramerateQuickComboBox->currentIndex()],
			false,
			false,
			false,
			false
		};
		handleExportExitCode(
			ffmpeg->exportFile(
				*currentVideo,
				getTrimSettings(),
				RESOLUTIONS[RESOLUTION_IDX::RESOLUTION_AS_INPUT],
				hevc,
				exp
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

void VideoLowWindow::setQuickExportsEnabled(bool enabled)
{
	ui->H264_2->setEnabled(enabled);
	ui->H264_4->setEnabled(enabled);
	ui->H264_8->setEnabled(enabled);
	ui->H264_16->setEnabled(enabled);

	ui->HEVC_2->setEnabled(enabled);
	ui->HEVC_4->setEnabled(enabled);
	ui->HEVC_8->setEnabled(enabled);
	ui->HEVC_16->setEnabled(enabled);
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
		ExportSettings exp = {
			ui->BitrateDoubleSpinBox->value(),
			FRAMERATES[ui->FramerateComboBox->currentIndex()],
			false,
			ui->verticalVideoCheckbox->isChecked(),
			ui->AudioOnlyCheckBox->isChecked(),
			ui->RemoveAudioCheckBox->isChecked()
		};

		handleExportExitCode(
			ffmpeg->exportFile(
				*currentVideo,
				getTrimSettings(),
				RESOLUTIONS[ui->ResolutionComboBox->currentIndex()],
				codec,
				exp
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
		CodecConfig codec = DefaultCodecs[VIDEO_CODEC::H264][HARDWARE_ACCELERATION::NONE]; //default parameters as it doesnt matter for this trimming
		ExportSettings exp = {
			ui->BitrateDoubleSpinBox->value(),
			FRAMERATES[0],
			true,
			false,
			false,
			false
		};
		handleExportExitCode(
			ffmpeg->exportFile(
				*currentVideo,
				getTrimSettings(),
				RESOLUTIONS[RESOLUTION_IDX::RESOLUTION_AS_INPUT],
				codec,
				exp
			),
			ui->HardwareAccelerationQuickComboBox->currentIndex() != 0
		);
	}
}

void VideoLowWindow::codecConfigChanged(int)
{
	auto codec = DefaultCodecs[VIDEO_CODEC(ui->CodecComboBox->currentIndex())][HARDWARE_ACCELERATION(ui->HardwareAccelerationComboBox->currentIndex())];
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

void VideoLowWindow::uncheckRemoveAudio(bool isAudioOnlyChecked)
{
	if(isAudioOnlyChecked)
		ui->RemoveAudioCheckBox->setChecked(false);
}

void VideoLowWindow::uncheckAudioOnly(bool isRemoveAudioChecked)
{
	if (isRemoveAudioChecked)
		ui->AudioOnlyCheckBox->setChecked(false);
}

void VideoLowWindow::newVideoFile(Video const& vid)
{
	ui->SettingsTab->setCurrentIndex(0);
	ui->AudioTab->setEnabled(true);
	ui->VideoTab->setEnabled(true);
	ui->VideoInfoLayout->setEnabled(true);
	ui->AudioInfoLayout->setEnabled(true);
	setQuickExportsEnabled(true);

	ui->RemoveAudioCheckBox->setChecked(false);
	ui->RemoveAudioCheckBox->setEnabled(true);
	ui->AudioOnlyCheckBox->setChecked(false);
	ui->AudioOnlyCheckBox->setEnabled(true);

	if (!currentVideo) {
		currentVideo = new Video(vid);
	}
	else
		*currentVideo = vid;
	bool unsupportedVideo = vid.codec.videoCodec == VIDEO_CODEC::UNSUPPORTED_VIDEO;
	bool unsupportedAudio = vid.codec.audioCodec == AUDIO_CODEC::UNSUPPORTED_AUDIO;

	QTime zero(0, 0);
	ui->reviewVideoButton->setDisabled(false);
	ui->trimVideoCheckBox->setChecked(false);
	ui->startTimeEdit->setTime(zero);
	ui->EndTimeEdit->setTime(vid.length);
	if (vid.length.msec() == -1 || unsupportedVideo) {
		ui->videoLengthTimeEdit->setTime(zero);
	}
	else {
		ui->videoLengthTimeEdit->setTime(vid.length);
	}
	if (vid.resolution.width && vid.resolution.height && !unsupportedVideo)
		ui->videoResolutionLabel->setText(QString::number(vid.resolution.width) + "x" + QString::number(vid.resolution.height));
	else
		ui->videoResolutionLabel->setText("-");
	if (vid.framerate && !unsupportedVideo)
		ui->videoFramerateLabel->setText(QString::number(vid.framerate, 'f', 2) + " fps");
	else
		ui->videoFramerateLabel->setText("-");
	//TODO: detect vertical video
	ui->videoCodecLabel->setText(vid.codec.name + " (" + vid.codec.profiles[vid.codec.profile] + ")");
	ui->videoBitrateLabel->setText((vid.bitrate == 0. || unsupportedVideo) ? QString("-") : QString::number(vid.bitrate, 'f', 2) + " MBit/s");

	ui->audioCodecLabel->setText(vid.codec.audioCodecName);

	if (unsupportedAudio) {
		//disable audio tab
		ui->AudioTab->setEnabled(false);
		ui->audioBitrateLabel->setText("-");
		ui->audioLengthTimeEdit->setTime(zero);
	}
	else {
		ui->audioBitrateLabel->setText(QString::number(vid.audioBitrate * 1000.0, 'f', 2) + " KBit/s");
		ui->audioLengthTimeEdit->setTime(vid.audioLength);
	}

	if (unsupportedVideo) {
		//disable video tab
		ui->VideoTab->setEnabled(false);
		ui->SettingsTab->setCurrentIndex(1);
		//disable all quick export except trimming
		setQuickExportsEnabled(false);
		//disable remove audio and audio only while selecting audio only
		ui->RemoveAudioCheckBox->setChecked(false);
		ui->RemoveAudioCheckBox->setEnabled(false);
		ui->AudioOnlyCheckBox->setChecked(true);
		ui->AudioOnlyCheckBox->setEnabled(false);
	}
}


