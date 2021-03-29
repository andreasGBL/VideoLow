#pragma once

class QString;
struct Resolution;
struct Codec;

class FFMPEGWrapper
{
public:
    FFMPEGWrapper();
    bool exportFile(QString const &filePath, double MBitRate, Resolution const &res, Codec const &codec, int HardwareAccelleration, int Framerate);
private:
    QString getFileName(QString const &filePath);
    QString getExportFileName(QString const &fileName);
    QString getPath(QString const &filePath);
    QString getExportFilePath(QString const &filePath);

    QString getMBitRateString(double MBitRate);
    QString getFramerateFilter(int Framerate);
    QString getScaleFilterString(Resolution const &res, int HardwareAcceleration);
    
    QString getVideoCodecString(Codec const & codec, int HardwareAcceleration);
    QString getAudioCodecString();
    QString getHardwareAccelerationString(int HardwareAcceleration);
    QString getInputFileString(QString const & filePath);
    QString getOutputFileString(QString const & filePath);
};

