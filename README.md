# VideoLow
VideoLow is a lightweight, free tool for Windows based on [ffmpeg](https://github.com/FFmpeg/FFmpeg) that allows you to compress, reencode and trim mp4 videos easily.
This is the perfect tool if you want to create a clip from a video to send your friends. 
Avoid large file sizes by selecting a smaller bitrate and speed up the process by utilizing your GPU.


It currently supports h.264 and hevc (h.265) codecs.
The GUI is made with [QT 5](https://github.com/qt/qt5).

# Download
[Latest Github Release](https://github.com/andreasGBL/VideoLow/releases/latest)

# Features
- select a video per drag & drop
- review videos with a built-in video player
- select start and end position for the trimmed part during playback
- choose to export the video with specific settings
- quick export options for easier access
- trim only function that trims the video without reencoding which results in an almost instantly trimmed video
- supports AMD (amf) and Nvidia (nvenc) hardware accelerated codecs


![Main Window](https://user-images.githubusercontent.com/29144928/179365038-718c2bde-14a4-483e-94a4-a938b62b65c9.png)

![Trim Window](https://user-images.githubusercontent.com/29144928/179365071-6dcb87a8-bb8b-4992-8292-5f58e82a7aa5.png)


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
- Select "Trim Video" if not already selected
- Choose "Only Trim Video" to export the Clip without reencoding the Clip (very fast) or export the clip with the Export configuration or use a quick export option (slower, but more customizable)
- Your file will be right next to your input video, but with the ending "Export.mp4"

## Reencode a Video
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

# Known problems
- no linux support
- videos with special characters in the file name aren't supported
- qt can't handle every video in the built-in video player
- when the hardware accelerated codecs fail, try updating your drivers - if that doesn't work try different parameters and maybe try to change some parameters back to "As Input"

# License
This open source software is licensed under [GPLv3](LICENSE).
