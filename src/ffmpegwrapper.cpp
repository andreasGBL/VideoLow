#include "../include/ffmpegwrapper.h"

#include <include/structs.h>

#include <QStringList>

#include <fstream>
#include <iostream>
#include <cstdlib>


FFMPEGWrapper::FFMPEGWrapper()
{
	std::ifstream f("./ffmpeg.exe");
	if (!f.good()) {
		std::cerr << "Can't find ffmpeg..." << std::endl;
		std::abort();
	}
}

bool FFMPEGWrapper::exportFile(const Video& video, const TrimSettings& settings, double MBitRate, const Resolution& res, const CodecConfig& codec, int Framerate, bool trimOnly, bool vertical)
{
	const QString& filePath = video.filePath;
	QString expFilePath = getExportFilePath(filePath);
	std::cout << "Export File Path: " << expFilePath.toStdString().c_str() << std::endl;
	QString cmd = "ffmpeg -y ";
	//options part 1
	QString trimming = getTrimString(settings.start, settings.end, video);

	QString inputFile = getInputFileString(filePath);

	//filters
	std::vector<QString> filters;
	QString framerateFilter = getFramerateFilter(Framerate, video.framerate);
	QString scaleFilter = getScaleFilterString(res, video.resolution, codec.hw_acceleration, vertical);

	//options part2
	QString audioCodec = getAudioCodecString(settings.start == QTime(0, 0) || !settings.trim || trimOnly);
	QString videoCodec = getVideoCodecString(codec, trimOnly);
	QString bitrate = getMBitRateString(MBitRate);
	QString outputFile = getOutputFileString(expFilePath);

	//options part1 hwac
	QString hwacc = getHardwareAccelerationString(codec.hw_acceleration, scaleFilter.length() > 0);

	//add options part 1
	if (trimming.length() > 0 && settings.trim)
		cmd += trimming + " ";
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

QString FFMPEGWrapper::getFileName(QString const& filePath)
{
	int lastIdx = filePath.lastIndexOf("/");
	return filePath.mid(lastIdx);
}

QString FFMPEGWrapper::getExportFileName(QString const& fileName)
{
	auto endIdx = fileName.lastIndexOf(".");
	auto file = fileName.mid(0, endIdx);
	auto ending = fileName.mid(endIdx + 1);
	return file + "Export." + ending;
}

QString FFMPEGWrapper::getPath(QString const& filePath)
{
	int lastIdx = filePath.lastIndexOf("/");
	return filePath.mid(0, lastIdx);
}

QString FFMPEGWrapper::getExportFilePath(QString const& filePath)
{
	QString fileName = getFileName(filePath);
	return getPath(filePath) + getExportFileName(fileName);
}

QString FFMPEGWrapper::getTrimString(QTime const& start, QTime const& end, Video const& video)
{
	QString trim("");
	if (start > QTime(0, 0)) {
		trim.append("-ss " + toTimeString(start));
	}
	if (end < video.length) {
		trim.append(" -to " + toTimeString(end));
	}
	return trim;
}

QString FFMPEGWrapper::getMBitRateString(double MBitRate)
{
	return "-b:v " + QString::number(MBitRate) + "M";
}

QString FFMPEGWrapper::getFramerateFilter(double Framerate, double vidFramerate)
{
	if (Framerate <= 0. || Framerate == vidFramerate)
		return QString("");
	else
		return QString("fps=fps=") + QString::number(Framerate);
}

QString FFMPEGWrapper::getScaleFilterString(Resolution const& res, Resolution const& vidRes, int HardwareAcceleration, bool vertical)
{
	if (res.height <= 0 && res.width <= 0 || (res.height == vidRes.height && res.width == vidRes.width)) {
		return QString("");
	}
	QString d1 = QString::number(res.width);
	QString d2 = QString::number(res.height);
	if (vertical)
		std::swap(d1, d2);
	QString resolutionString = d1 + ":" + d2;
	switch (HardwareAcceleration)
	{
	case(HARDWARE_ACCELERATION::NVIDIA):
		return QString("scale_cuda=") + resolutionString;
	default:
		return QString("scale=") + resolutionString;
	}
}

QString FFMPEGWrapper::getVideoCodecString(CodecConfig const& codec, bool trimOnly)
{
	QString prefix = "-c:v ";
	if (trimOnly) {
		return prefix + "copy";
	}
	else {
		return prefix + codec.encoderName + " -vprofile " + codec.profiles[codec.profile].toLower() + codec.extraOptions[codec.profile % codec.extraOptions.size()]; // use modulo to never go out of bounds on shorter vectors
	}
}

QString FFMPEGWrapper::getAudioCodecString(bool canCopy)
{
	if (canCopy)
		return QString("-c:a copy");
	else
		return QString("");
}

QString FFMPEGWrapper::getHardwareAccelerationString(int HardwareAcceleration, bool usesScaleFilter)
{
	switch (HardwareAcceleration)
	{
	case(HARDWARE_ACCELERATION::NVIDIA):
		return QString("-hwaccel cuda") + (usesScaleFilter ? QString(" -hwaccel_output_format cuda") : QString(""));
	default:
		return QString("");
	}
}

QString FFMPEGWrapper::getInputFileString(QString const& filePath)
{
	return "-i \"" + filePath + "\"";
}

QString FFMPEGWrapper::getOutputFileString(QString const& filePath)
{
	return "\"" + filePath + "\"";
}

QString FFMPEGWrapper::toTimeString(QTime const& t)
{
	QString time = toDigits(t.hour(), 2) + ":" + toDigits(t.minute(), 2) + ":" + toDigits(t.second(), 2) + "." + toDigits(t.msec(), 3);
	return time;
}

QString FFMPEGWrapper::toDigits(int number, int digits)
{
	if (number < 0)
		throw std::runtime_error("Number must be positive");
	int temp = 10;
	QString string("");
	for (int i = 0; i < digits - 1; i++) {
		if (number < temp) {
			string += "0";
		}
		temp *= 10;
	}
	string += QString::number(number);
	return string;
}

