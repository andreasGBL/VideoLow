#pragma once

struct Resolution;
struct TrimSettings;
struct CodecConfig;
struct Resolution;
struct Video;

class QTime;
class QString;

class FFMPEGWrapper
{
public:
	FFMPEGWrapper();
	bool exportFile(Video const & video, TrimSettings const & settings, double MBitRate, Resolution const & res, CodecConfig const & codec, int Framerate, bool trimOnly);
private:
	QString getFileName(QString const & filePath);
	QString getExportFileName(QString const & fileName);
	QString getPath(QString const & filePath);
	QString getExportFilePath(QString const & filePath);

	QString getTrimString(QTime const & start, QTime const & end, Video const & video);
	QString getMBitRateString(double MBitRate);
	QString getFramerateFilter(double Framerate, double vidFramerate);
	QString getScaleFilterString(Resolution const & res, Resolution const & vidRes, int HardwareAcceleration);

	QString getVideoCodecString(CodecConfig const & codec, bool trimOnly);
	QString getAudioCodecString(bool canCopy);
	QString getHardwareAccelerationString(int HardwareAcceleration, bool usesScaleFilter);
	QString getInputFileString(QString const & filePath);
	QString getOutputFileString(QString const & filePath);

	QString toTimeString(QTime const & t);
	QString toDigits(int number, int digits);
};

