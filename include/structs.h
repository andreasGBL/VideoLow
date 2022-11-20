#pragma once

#include <QString>
#include <QTime>
#include <vector>

struct Resolution {
	int width;
	int height;
};

struct TrimSettings {
	QTime start;
	QTime end;
	bool trim = false;
};
//TODO: add 8K
enum RESOLUTION_IDX {
	RESOLUTION_AS_INPUT,
	K4,
	p1440,
	p1080,
	p720,
	p480,
	p240
};

enum HARDWARE_ACCELERATION {
	NONE,
	NVIDIA,
	AMD
};

Resolution const RESOLUTIONS[] = {
	{
		0,
		0
	},
	{
		3840,
		2160
	},
	{
		2560,
		1440
	},
	{
		1920,
		1080
	},
	{
		1280,
		720
	},
	{
		854,
		480
	},
	{
		426,
		240
	}
};

float const FRAMERATES[] = {
	0,
	30,
	60,
	120,
	240
};

int const AUDIO_BITRATES[] = {
	0,
	320,
	256,
	192,
	160,
	128,
	64
};

enum VIDEO_CODEC {
	H264,
	HEVC,
	UNSUPPORTED_VIDEO
};

enum AUDIO_CODEC {
	AAC,
	MP3,
	UNSUPPORTED_AUDIO
};

QString const VIDEO_CODEC_STRINGS[] = {
	"H264",
	"HEVC",
	"Unsupported"
};

QString const AUDIO_CODEC_STRINGS[]{
	"AAC",
	"MP3",
	"Unsupported"
};

QString const AUDIO_ENCODER_STRINGS[]{
	"aac",
	"libmp3lame",
	"Unsupported"
};

struct CodecConfig {
	int videoCodec;
	QString name;
	HARDWARE_ACCELERATION hw_acceleration;
	QString videoEncoderName;
	std::vector<QString> profiles;
	int profile; //also default profile
	int mainProfile;
	std::vector<QString> extraOptions;
	int audioBitrateIdx;
	int audioCodec;
	QString audioCodecName;
	QString audioEncoderName;
};

CodecConfig const DefaultCodecs[2][3] = {
	{
		{
			VIDEO_CODEC::H264, VIDEO_CODEC_STRINGS[VIDEO_CODEC::H264], HARDWARE_ACCELERATION::NONE, "libx264", {"Baseline", "Main", "High", "High10", "High444"}, 2, 1, {"", "", "", "", " -pix_fmt yuv444p"}, 0, AUDIO_CODEC::AAC, AUDIO_CODEC_STRINGS[AUDIO_CODEC::AAC], AUDIO_ENCODER_STRINGS[AUDIO_CODEC::AAC]
		},
		{
			VIDEO_CODEC::H264, VIDEO_CODEC_STRINGS[VIDEO_CODEC::H264], HARDWARE_ACCELERATION::NVIDIA, "h264_nvenc", {"Baseline", "Main", "High", "High444p"}, 2, 1, {"", "", "", " -pix_fmt yuv444p"}, 0, AUDIO_CODEC::AAC, AUDIO_CODEC_STRINGS[AUDIO_CODEC::AAC], AUDIO_ENCODER_STRINGS[AUDIO_CODEC::AAC]
		},
		{
			VIDEO_CODEC::H264, VIDEO_CODEC_STRINGS[VIDEO_CODEC::H264], HARDWARE_ACCELERATION::AMD, "h264_amf", {"Constrained_Baseline", "Main", "High", "Constrained_High"}, 2, 1, {" -quality:v 2"}, 0, AUDIO_CODEC::AAC, AUDIO_CODEC_STRINGS[AUDIO_CODEC::AAC], AUDIO_ENCODER_STRINGS[AUDIO_CODEC::AAC]
		}
	},
	{
		{
			VIDEO_CODEC::HEVC, VIDEO_CODEC_STRINGS[VIDEO_CODEC::HEVC], HARDWARE_ACCELERATION::NONE, "libx265", {"Main", "Main10", "Main12"}, 0, 0, {""}, 0, AUDIO_CODEC::AAC, AUDIO_CODEC_STRINGS[AUDIO_CODEC::AAC], AUDIO_ENCODER_STRINGS[AUDIO_CODEC::AAC]
		},
		{
			VIDEO_CODEC::HEVC, VIDEO_CODEC_STRINGS[VIDEO_CODEC::HEVC], HARDWARE_ACCELERATION::NVIDIA, "hevc_nvenc", {"Main", "Main10", "Rext"}, 0, 0, {""}, 0, AUDIO_CODEC::AAC, AUDIO_CODEC_STRINGS[AUDIO_CODEC::AAC], AUDIO_ENCODER_STRINGS[AUDIO_CODEC::AAC]
		},
		{
			VIDEO_CODEC::HEVC, VIDEO_CODEC_STRINGS[VIDEO_CODEC::HEVC], HARDWARE_ACCELERATION::AMD, "hevc_amf", {"Main"}, 0, 0, {" -quality:v 0"} /* TODO: test AMD codec */, 0, AUDIO_CODEC::AAC, AUDIO_CODEC_STRINGS[AUDIO_CODEC::AAC], AUDIO_ENCODER_STRINGS[AUDIO_CODEC::AAC]
		}
	}
};

//CodecConfig getDefaultCodec(VIDEO_CODEC codec, HARDWARE_ACCELERATION acc = HARDWARE_ACCELERATION::NONE) {
//	return DefaultCodecs[codec][acc];
//}

struct Video {
	QString filePath;
	QTime length;
	Resolution resolution;
	double framerate;
	CodecConfig codec;
	double bitrate;
	double audioBitrate;
	QTime audioLength;
};