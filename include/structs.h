#pragma once

#include <QString>
#include <QTime>
#include <vector>

struct Resolution {
public:
	int width;
	int height;
};

//struct Codec {
//	std::string defaultName;
//	std::string NVIDIA;
//	std::string AMD;
//};

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

enum CODEC_IDX {
	H264,
	HEVC,
	UNSUPPORTED
};

QString const CODEC_IDX_STRINGS[] = {
	"H264",
	"HEVC",
	"Unsupported"
};

struct CodecConfig {
	int id;
	QString name;
	HARDWARE_ACCELERATION hw_acceleration;
	QString encoderName;
	std::vector<QString> profiles;
	int profile; //also default profile
	int mainProfile;
	std::vector<QString> extraOptions;
};

CodecConfig const DefaultCodecs[2][3] = {
	{
		{
			CODEC_IDX::H264, CODEC_IDX_STRINGS[CODEC_IDX::H264], HARDWARE_ACCELERATION::NONE, "libx264", {"Baseline", "Main", "High", "High10", "High444"}, 2, 1, {"", "", "", "", " -pix_fmt yuv444p"}
		},
		{
			CODEC_IDX::H264, CODEC_IDX_STRINGS[CODEC_IDX::H264], HARDWARE_ACCELERATION::NVIDIA, "h264_nvenc", {"Baseline", "Main", "High", "High444p"}, 2, 1, {"", "", "", " -pix_fmt yuv444p"}
		},
		{
			CODEC_IDX::H264, CODEC_IDX_STRINGS[CODEC_IDX::H264], HARDWARE_ACCELERATION::AMD, "h264_amf", {"Constrained_Baseline", "Main", "High", "Constrained_High"}, 2, 1, {" -quality:v 2"}
		}
	},
	{
		{
			CODEC_IDX::HEVC, CODEC_IDX_STRINGS[CODEC_IDX::HEVC], HARDWARE_ACCELERATION::NONE, "libx265", {"Main", "Main10", "Main12"}, 0, 0, {""}
		},
		{
			CODEC_IDX::HEVC, CODEC_IDX_STRINGS[CODEC_IDX::HEVC], HARDWARE_ACCELERATION::NVIDIA, "hevc_nvenc", {"Main", "Main10", "Rext"}, 0, 0, {""}
		},
		{
			CODEC_IDX::HEVC, CODEC_IDX_STRINGS[CODEC_IDX::HEVC], HARDWARE_ACCELERATION::AMD, "hevc_amf", {"Main"}, 0, 0, {" -quality:v 0"} //TODO: test AMD codec
		}
	}
};

//CodecConfig getDefaultCodec(CODEC_IDX codec, HARDWARE_ACCELERATION acc = HARDWARE_ACCELERATION::NONE) {
//	return DefaultCodecs[codec][acc];
//}

struct Video {
	QString filePath;
	QTime length;
	Resolution resolution;
	double framerate;
	CodecConfig codec;
	double bitrate;
};