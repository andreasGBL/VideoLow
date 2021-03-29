#include "ffmpegwrapper.h"

#include <QStringList>
#include <QString>

#include "Structs.h"

#include <fstream>
#include <string>
#include <iostream>
#include <stdlib.h>


FFMPEGWrapper::FFMPEGWrapper()
{
	std::ifstream f("./ffmpeg.exe");
	if (!f.good()) {
		std::cerr << "Can't find ffmpeg..." << std::endl;
		std::abort();
	}
}

bool FFMPEGWrapper::exportFile(const QString & filePath, double MBitRate, const Resolution & res, const Codec & codec, int HardwareAccelleration, int Framerate)
{
	QString expFilePath = getExportFilePath(filePath);
	std::cout << "Export File Path: " << expFilePath.toStdString().c_str() << std::endl;
	QString cmd = "ffmpeg -y ";
	//options part 1
	QString hwacc = getHardwareAccelerationString(HardwareAccelleration);
	QString inputFile = getInputFileString(filePath);

	//filters
	std::vector<QString> filters;
	QString framerateFilter = getFramerateFilter(Framerate);
	QString scaleFilter = getScaleFilterString(res, HardwareAccelleration);

	//options part2
	QString audioCodec = getAudioCodecString();
	QString videoCodec = getVideoCodecString(codec, HardwareAccelleration);
	QString bitrate = getMBitRateString(MBitRate);
	QString outputFile = getOutputFileString(expFilePath);

	//add options part 1
	if (hwacc.length() > 0)
		cmd += hwacc + " ";
	cmd += inputFile + " ";
	if (framerateFilter.length() > 0)
		filters.push_back(framerateFilter);
	if (scaleFilter.length() > 0)
		filters.push_back(scaleFilter);

	//Add filters:
	QString videoFilterString("-vf \"");
	for (int i = 0; i < filters.size(); i++) {
		videoFilterString += filters[i];
		if (i + 1 < filters.size())
			videoFilterString += ", ";
	}
	videoFilterString += "\" ";
	if (filters.size() > 0)
		cmd += videoFilterString;

	//add options part 2
	cmd += audioCodec + " " + videoCodec + " " + bitrate + " " + outputFile;
	std::cout << cmd.toStdString() << std::endl;
	//execute command
	int exitCode = system(cmd.toStdString().c_str());
    return exitCode == 0;
}

QString FFMPEGWrapper::getFileName(QString const & filePath)
{
	int lastIdx = filePath.lastIndexOf("/");
	return filePath.mid(lastIdx);
}

QString FFMPEGWrapper::getExportFileName(QString const & fileName)
{
	auto endIdx = fileName.lastIndexOf(".");
	auto file = fileName.mid(0, endIdx);
	auto ending = fileName.mid(endIdx + 1);
	return file + "Export." + ending;
}

QString FFMPEGWrapper::getPath(QString const & filePath)
{
	int lastIdx = filePath.lastIndexOf("/");
	return filePath.mid(0, lastIdx);
}

QString FFMPEGWrapper::getExportFilePath(QString const & filePath)
{
	QString fileName = getFileName(filePath);
	return getPath(filePath) + getExportFileName(fileName);
}

QString FFMPEGWrapper::getMBitRateString(double MBitRate)
{
	return "-b:v " + QString::number(MBitRate) + "M";
}

QString FFMPEGWrapper::getFramerateFilter(int Framerate)
{
	if (Framerate <= 0)
		return QString("");
	else
		return QString("fps=fps=") + QString::number(Framerate);
}

QString FFMPEGWrapper::getScaleFilterString(Resolution const & res, int HardwareAcceleration)
{
	if (res.height <= 0 && res.width <= 0) {
		return QString("");
	}
	QString resolutionString = QString::number(res.width) + ":" + QString::number(res.height);
	switch (HardwareAcceleration)
	{
	case(HARDWARE_ACCELERATION::NVIDIA):
		return QString("scale_cuda=") + resolutionString;
	default:
		return QString("scale=") + resolutionString;
	}
}

QString FFMPEGWrapper::getVideoCodecString(Codec const & codec, int HardwareAcceleration)
{
	QString prefix = "-c:v ";
	switch (HardwareAcceleration)
	{
	case(HARDWARE_ACCELERATION::NVIDIA):
		return prefix + codec.NVIDIA.c_str();
	case(HARDWARE_ACCELERATION::AMD):
		return prefix + codec.AMD.c_str();
	default:
		return prefix + codec.defaultName.c_str();
	}
}

QString FFMPEGWrapper::getAudioCodecString()
{
	return QString("-c:a copy");
}

QString FFMPEGWrapper::getHardwareAccelerationString(int HardwareAcceleration)
{
	switch (HardwareAcceleration)
	{
	case(HARDWARE_ACCELERATION::NVIDIA):
		return QString("-hwaccel cuda -hwaccel_output_format cuda");
	default:
		return QString("");
	}
}

QString FFMPEGWrapper::getInputFileString(QString const & filePath)
{
	return "-i  \"" + filePath + "\"";
}

QString FFMPEGWrapper::getOutputFileString(QString const & filePath)
{
	return "\"" + filePath + "\"";
}
