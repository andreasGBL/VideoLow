#include "./ui_videolowwindow.h"
#include <QMainWindow>
#include <QString>


#include "Structs.h"

#include <iostream>

VideoLowWindow::VideoLowWindow(QWidget * parent)
	: QMainWindow(parent)
	, ffmpeg()
	, ui(new Ui::VideoLowWindow)
{
	ui->setupUi(this);
	ui->DropWidget->setVideoLowWindowPointer(this);
	connectSlots();
	setWindowFlags(Qt::Window | Qt::MSWindowsFixedSizeDialogHint);
	this->statusBar()->setSizeGripEnabled(false);
}

VideoLowWindow::~VideoLowWindow()
{
	delete ui;
	delete currentFile;
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
}

void VideoLowWindow::quickH264(double MBitRate)
{
	if (currentFile)
		handleExportExitCode(
			ffmpeg.exportFile(
				*currentFile,
				MBitRate,
				RESOLUTIONS[RESOLUTION_IDX::RESOLUTION_AS_INPUT],
				CODECS[CODEC_IDX::H264],
				ui->HardwareAccelerationQuickComboBox->currentIndex(),
                FRAMERATES[ui->FramerateQuickComboBox->currentIndex()]
			)
		);
}

void VideoLowWindow::quickHEVC(double MBitRate)
{
	if (currentFile) {
		handleExportExitCode(
			ffmpeg.exportFile(
				*currentFile,
				MBitRate,
				RESOLUTIONS[RESOLUTION_IDX::RESOLUTION_AS_INPUT],
				CODECS[CODEC_IDX::HEVC],
				ui->HardwareAccelerationQuickComboBox->currentIndex(),
				FRAMERATES[ui->FramerateQuickComboBox->currentIndex()]
			)
		);

	}
}

void VideoLowWindow::handleExportExitCode(bool success)
{
	if (!success) {
		std::cout << "Error during export!" << std::endl;
		ui->ErrorLabel->setText(tr("Error during last Export!"));
	}
	else {
		std::cout << "Successful export!" << std::endl;
		ui->ErrorLabel->clear();
	}
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
	if (currentFile) {
		handleExportExitCode(
			ffmpeg.exportFile(
				*currentFile,
				ui->BitrateDoubleSpinBox->value(),
				RESOLUTIONS[ui->ResolutionComboBox->currentIndex()],
				CODECS[ui->CodecComboBox->currentIndex()],
				ui->HardwareAccelerationComboBox->currentIndex(),
				FRAMERATES[ui->FramerateComboBox->currentIndex()]
			)
		);
	}
}

void VideoLowWindow::newVideoFile(QString filePath)
{
	if (!currentFile) {
		currentFile = new QString(filePath);
	}
	else
		*currentFile = filePath;
}

