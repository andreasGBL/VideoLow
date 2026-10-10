#include "../include/ffmpegwrapper.h"
#include "../include/videolowpaths.h"

#include <include/structs.h>

#include <QFile>
#include <QProcess>
#include <QStringList>

#include <iostream>
#include <cstdlib>

namespace {
void appendOptions(QStringList& arguments, const QString& options)
{
	arguments.append(options.split(' ', QString::SkipEmptyParts));
}
}

FFMPEGWrapper::FFMPEGWrapper()
{
	if (!QFile::exists(VideoLowPaths::ffmpegExecutable())) {
		std::cerr << "Can't find ffmpeg..." << std::endl;
		std::abort();
	}
}

bool FFMPEGWrapper::exportFile(const Video& video, const TrimSettings& settings, const Resolution& res, const CodecConfig& codec, const ExportSettings & exports)
{
	const QString& filePath = video.filePath;
	QString expFilePath = getExportFilePath(filePath, exports.audioOnly, codec.audioCodecName);
	std::cout << "Export File Path: " << expFilePath.toStdString().c_str() << std::endl;
	QStringList arguments;
	arguments << "-y";
	//options part 1
	QString trimming = getTrimString(settings.start, settings.end, video);

	//filters
	std::vector<QString> filters;
	QString framerateFilter = getFramerateFilter(exports.framerate, video.framerate);
	QString transformFilter = getTransformFilterString(exports.transformConfig, exports.vertical);
	bool canUseCudaScale = codec.hw_acceleration == HARDWARE_ACCELERATION::NVIDIA &&
		framerateFilter.isEmpty() && transformFilter.isEmpty();
	QString scaleFilter = getScaleFilterString(
		res,
		video.resolution,
		canUseCudaScale ? HARDWARE_ACCELERATION::NVIDIA : HARDWARE_ACCELERATION::NONE,
		exports.vertical);

	//options part2
	QString audioCodec = getAudioCodecString(video, settings, codec, exports);
	QString videoCodec = getVideoCodecString(codec, exports.trimOnly);
	QString bitrate = getMBitRateString(exports.VideoMBitRate);

	//options part1 hwac
	QString hwacc = getHardwareAccelerationString(codec.hw_acceleration, canUseCudaScale && !scaleFilter.isEmpty());

	if (exports.audioOnly) {
		videoCodec = "-vn";
		bitrate = "";
		hwacc = "";
		framerateFilter = "";
		scaleFilter = "";
	}
	//add options part 1
	if (trimming.length() > 0 && settings.trim)
		appendOptions(arguments, trimming);
	if (hwacc.length() > 0)
		appendOptions(arguments, hwacc);
	arguments << "-i" << filePath;
	if (framerateFilter.length() > 0)
		filters.push_back(framerateFilter);
	if (scaleFilter.length() > 0)
		filters.push_back(scaleFilter);
	if (transformFilter.length() > 0)
		filters.push_back(transformFilter);

	//Add filters:
	if (!filters.empty()) {
		QStringList filterList;
		for (const QString& filter : filters)
			filterList << filter;
		arguments << "-vf" << filterList.join(", ");
	}

	//add options part 2
	appendOptions(arguments, audioCodec);
	appendOptions(arguments, videoCodec);
	appendOptions(arguments, bitrate);
	arguments << expFilePath;
	QProcess process;
	process.start(VideoLowPaths::ffmpegExecutable(), arguments);
	return process.waitForStarted() && process.waitForFinished(-1) &&
		process.exitStatus() == QProcess::NormalExit && process.exitCode() == 0;
}

QString FFMPEGWrapper::getFileName(QString const& filePath)
{
	int lastIdx = filePath.lastIndexOf("/");
	return filePath.mid(lastIdx);
}

QString FFMPEGWrapper::getExportFileName(QString const& fileName, bool audioOnly, const QString & audioFileType)
{
	auto endIdx = fileName.lastIndexOf(".");
	auto file = fileName.mid(0, endIdx);
	auto ending = audioOnly ? audioFileType : fileName.mid(endIdx + 1);
	return file + "Export." + ending;
}

QString FFMPEGWrapper::getPath(QString const& filePath)
{
	int lastIdx = filePath.lastIndexOf("/");
	return filePath.mid(0, lastIdx);
}

QString FFMPEGWrapper::getExportFilePath(QString const& filePath, bool audioOnly, const QString & audioFileType)
{
	QString fileName = getFileName(filePath);
	return getPath(filePath) + getExportFileName(fileName, audioOnly, audioFileType);
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
	int w = res.width, h = res.height;
	if (vertical)
		std::swap(w, h);
	if (h <= 0 && w <= 0 || (h == vidRes.height && w == vidRes.width)) {
		return QString("");
	}
	QString d1 = QString::number(w);
	QString d2 = QString::number(h);
	
	QString resolutionString = d1 + ":" + d2;
	switch (HardwareAcceleration)
	{
	case(HARDWARE_ACCELERATION::NVIDIA):
		return QString("scale_cuda=") + resolutionString;
	default:
		return QString("scale=") + resolutionString;
	}
}

QString FFMPEGWrapper::getTransformFilterString(TRANSFORM_CONFIG const& t_config, bool vertical)
{
	QString options[6] = {
		"",
		"transpose=1",
		"transpose=1,transpose=1",
		"transpose=2",
		"hflip",
		"vflip"
	};
	return options[t_config];
}

QString FFMPEGWrapper::getVideoCodecString(CodecConfig const& codec, bool trimOnly)
{
	QString prefix = "-c:v ";
	if (trimOnly) {
		return prefix + "copy";
	}
	else {
		QString profileOption;
		if (!codec.profiles.empty())
			profileOption = " -vprofile " + codec.profiles[codec.profile].toLower();
		QString extraOptions;
		if (!codec.extraOptions.empty())
			extraOptions = codec.extraOptions[codec.profile % codec.extraOptions.size()];
		return prefix + codec.videoEncoderName + profileOption + extraOptions;
	}
}

QString FFMPEGWrapper::getAudioCodecString(const Video& video, const TrimSettings& settings, const CodecConfig& codec, const ExportSettings& exports)
{
	bool canCopy = ((settings.start == QTime(0, 0) || !settings.trim) && codec.audioCodec == video.codec.audioCodec && codec.audioBitrateIdx == 0) || exports.trimOnly;
	int audioBitrate = AUDIO_BITRATES[codec.audioBitrateIdx];
	QString audioBitrateString = codec.audioBitrateIdx == 0 ? "" : " -b:a " + QString::number(audioBitrate) + "K";
	
	if (exports.videoOnly)
		return QString("-an"); 
	else if (canCopy)
		return QString("-c:a copy");
	else
		return QString("-c:a " + codec.audioEncoderName + audioBitrateString);
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
