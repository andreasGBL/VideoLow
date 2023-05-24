# VideoLow
VideoLow is a lightweight, free tool for Windows based on [ffmpeg](https://github.com/FFmpeg/FFmpeg) that allows you to compress, transcode and trim /clip mp4 videos (and also audio files (mp3 and aac)) easily.
This is the perfect tool if you want to create a clip from a video to send to your friends. 
Avoid large file sizes by selecting a smaller bitrate and speed up the process by utilizing your GPU.


It currently supports h.264 and hevc (h.265) video codecs, as well as mp3 and aac audio codecs.
The GUI is made with [QT 5](https://github.com/qt/qt5).

# Download
[Latest Github Release](https://github.com/andreasGBL/VideoLow/releases/latest)

# Features
- select a video per drag & drop
- review videos / audio with a built-in video player
- trim videos / audio
- select start and end position for the trimmed part during playback
- choose to export the video / audio with specific settings
- remove audio from a video or only export the audio
- quick export options for easier access
- trim only function that trims the video without reencoding which results in an almost instantly trimmed video
- supports AMD (amf) and Nvidia (nvenc) hardware accelerated codecs

![Main Window](https://user-images.githubusercontent.com/29144928/206918013-8584c8d9-c00f-4f2b-b3bf-8848a5af1377.png)

![Trim Window](https://user-images.githubusercontent.com/29144928/206918099-7c68bcff-838a-4ad8-ac36-2995273cbf69.png)


# Build

- requires CMake
- requires QT 5
- requires Visual Studio as Compiler

```
git clone https://github.com/andreasGBL/VideoLow.git
cd VideoLow
./build-all.bat
```
Alternatively just download the latest [Release](https://github.com/andreasGBL/VideoLow/releases/latest).

# Instructions
## Create a Clip / Trim a video
- Import video per drag & drop
- Change start and end time or click "Review Video", watch the video and press "Set Start" and "Set End" whenever you feel like it, afterwards press "Confirm and close"
- Select "Trim Video" Check Box at the bottom of the Window if not already selected
- Choose "Only Trim Video" in Quick Export settings to export the Clip without reencoding the Clip (very fast), export the clip with the Export configuration or use another Quick Export option (slower, but more customizable)
- Your file will be right next to your input video, but with the ending "Export.mp4"

## Transcode a Video
- Import video per drag & drop
- Select bitrate (notice: a bitrate higher than the original video will not improve the quality)
- Select codec (H.264 by default)
- Select output resolution or don't change
- Select output framerate or don't change
- Select hardware acceleration when your PC has a Nvidia or AMD CPU, otherwise select None to use CPU encoding
- Click Export
- Your file will be right next to your input video, but with the ending "Export.mp4"

## Compress a Video
- Import video per drag & drop
- Select low bitrate (2-4 MBit/s is usually well compressed for Full HD videos)
- Select codec (H.264 by default)
- Select output resolution or don't change
- Select output framerate or don't change
- Select Hardware acceleration when your PC has a Nvidia or AMD CPU, otherwise select None to use CPU encoding
- Click Export
- Your file will be right next to your input video, but with the ending "Export.mp4"

## Quick Export
- import video per drag & drop
- When compressing or reencoding a video, you can save some time by selecting one of the quick export presets
- Choose a fitting quick export preset (e.g. H.264 2 Mbit/s for H.264 codec with a 2 MBit/s bitrate)
- notice: The hardware acceleration and output framerate below Quick Export Options apply, the output resolution is equal to the input resolution
- Your file will be right next to your input video, but with the ending "Export.mp4"

# Known problems / limitations
- no linux support
- videos with non ASCII / special characters in the file name aren't supported (e.g. emojis, ...)
- qt can't handle every video / audio in the built-in video player
- when the hardware accelerated codecs fail, try updating your drivers - if that doesn't work try different parameters and maybe try to change some parameters back to "As Input"
- AMD hardware acceleration is just barely tested
- A maximum video length of 23:59:59.999 (up to 24 hours of video) is supported
- No AV1 support yet, although it is already supported by ffmpeg

# License
This open source software is licensed under [GPLv3](LICENSE).

Copyright © 2021 - 2023 [andreasGBL](https://github.com/andreasGBL/). 
