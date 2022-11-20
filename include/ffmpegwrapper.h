#pragma once

struct Resolution;
struct TrimSettings;
struct CodecConfig;
struct Resolution;
struct Video;

class QTime;
class QString;

struct ExportSettings {
	double VideoMBitRate;
	double framerate;
	bool trimOnly;
	bool vertical;
	bool audioOnly;
	bool videoOnly;
};

class FFMPEGWrapper
{
public:
	FFMPEGWrapper();
	bool exportFile(Video const & video, TrimSettings const & settings, Resolution const & res, CodecConfig const & codec, ExportSettings const & exports);
private:
	QString getFileName(QString const & filePath);
	QString getExportFileName(QString const & fileName, bool audioOnly, const QString & audioFileType);
	QString getPath(QString const & filePath);
	QString getExportFilePath(QString const& filePath, bool audioOnly, const QString & audioFileType);

	QString getTrimString(QTime const & start, QTime const & end, Video const & video);
	QString getMBitRateString(double MBitRate);
	QString getFramerateFilter(double Framerate, double vidFramerate);
	QString getScaleFilterString(Resolution const & res, Resolution const & vidRes, int HardwareAcceleration, bool vertical);

	QString getVideoCodecString(CodecConfig const & codec, bool trimOnly);
	QString getAudioCodecString(const Video& video, const TrimSettings& settings, const CodecConfig& codec, const ExportSettings& exports);
	QString getHardwareAccelerationString(int HardwareAcceleration, bool usesScaleFilter);
	QString getInputFileString(QString const & filePath);
	QString getOutputFileString(QString const & filePath);

	QString toTimeString(QTime const & t);
	QString toDigits(int number, int digits);
};

