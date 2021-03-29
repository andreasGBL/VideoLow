#pragma once

#include <string>

struct Resolution {
public:
    int width;
    int height;
};

struct Codec {
public:
    std::string defaultName;
    std::string NVIDIA;
    std::string AMD;
};

enum RESOLUTION_IDX {
    RESOLUTION_AS_INPUT = 0,
    K4 = 1,
    p1080 = 2,
    p720 = 3,
    p480 = 4
};

enum HARDWARE_ACCELERATION {
    NONE = 0,
    NVIDIA = 1,
    AMD = 2
};

Resolution const RESOLUTIONS[7] = {
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

int const FRAMERATES[5] = {
    0,
    30,
    60,
    120,
    240
};

enum CODEC_IDX {
    H264 = 0,
    HEVC = 1
};

Codec const CODECS[3] = {
    {
        "libx264",
        "h264_nvenc",
        "h264_amf" 
    },
    {
        "libx265",
        "hevc_nvenc",
        "hevc_amf"
    }
};
