#pragma once

#include <QDir>
#include <QString>

namespace VideoLowPaths {
inline QString temporaryDirectory()
{
	return QDir::temp().filePath("VideoLow");
}

inline QString ffmpegExecutable()
{
	return QDir(temporaryDirectory()).filePath("ffmpeg.exe");
}

inline QString ffprobeExecutable()
{
	return QDir(temporaryDirectory()).filePath("ffprobe.exe");
}
}