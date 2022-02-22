#pragma once
#include "structs.h"
class QString;
struct Resolution;
struct Codec;

class FFMPEGWrapper
{
public:
	FFMPEGWrapper();
	bool exportFile(Video const & video, TrimSettings const & settings, double MBitRate, Resolution const & res, Codec const & codec, int HardwareAccelleration, int Framerate);
private:
	QString getFileName(QString const & filePath);
	QString getExportFileName(QString const & fileName);
	QString getPath(QString const & filePath);
	QString getExportFilePath(QString const & filePath);

	QString getTrimString(QTime const & start, QTime const & end, Video const & video);
	QString getMBitRateString(double MBitRate);
	QString getFramerateFilter(int Framerate);
	QString getScaleFilterString(Resolution const & res, int HardwareAcceleration);

	QString getVideoCodecString(Codec const & codec, int HardwareAcceleration);
	QString getAudioCodecString(bool canCopy);
	QString getHardwareAccelerationString(int HardwareAcceleration, bool usesScaleFilter);
	QString getInputFileString(QString const & filePath);
	QString getOutputFileString(QString const & filePath);

	QString toTimeString(QTime const & t);
	QString toDigits(int number, int digits);
};

