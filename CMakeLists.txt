cmake_minimum_required(VERSION 3.11)

message(STATUS "project name: ${PROJECT_NAME}")

if(NOT DEFINED PROJECT_NAME)
    set(PROJECT_NAME "VideoLow")
endif()

message(STATUS "project name: ${PROJECT_NAME}")

project(${PROJECT_NAME} LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(WITH_CONSOLE "Start with the Windows console" OFF)
set(HARDWARE_ACCELERATION_DEFAULT 0 CACHE STRING "Default value for hardware acceleration (0 is none, 1 is Nvidia, 2 is AMD)")

find_package(Qt5 COMPONENTS Widgets Multimedia MultimediaWidgets REQUIRED)

add_executable(${PROJECT_NAME}
    include/videolowwindow.h
    include/filedropwidget.h
    include/filepreviewwidget.h
    include/ffmpegwrapper.h
    include/structs.h
    include/VideoCutWindow/videocutwindow.h
    include/VideoCutWindow/videoplayerwidget.h
    src/main.cpp
    src/filedropwidget.cpp
    src/ffmpegwrapper.cpp
    src/videolowwindow.cpp
    src/VideoCutWindow/videocutwindow.cpp
    src/videolowwindow.ui
    src/VideoCutWindow/videocutwindow.ui
)


if(WITH_CONSOLE)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:CONSOLE")
else()
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS")
endif()

target_compile_definitions(${PROJECT_NAME} PUBLIC HARDWARE_ACCELERATION_DEFAULT=${HARDWARE_ACCELERATION_DEFAULT})

target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Widgets Qt5::Multimedia Qt5::MultimediaWidgets Qt5::WinMain)

#deploy QT libraries
get_target_property(_qmake_executable Qt5::qmake IMPORTED_LOCATION)
get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)

find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${_qt_bin_dir}")
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E env PATH="${_qt_bin_dir}" "${WINDEPLOYQT_EXECUTABLE}" "$<TARGET_FILE:${PROJECT_NAME}>"
    COMMENT "Deploying Qt library files...")

#copy ffmpeg
configure_file("./ffmpeg.exe" "./ffmpeg.exe" COPYONLY)
configure_file("./ffmpeg.exe" "./Release/ffmpeg.exe" COPYONLY)
configure_file("./ffmpeg.exe" "./Debug/ffmpeg.exe" COPYONLY)

configure_file("./ffprobe.exe" "./ffprobe.exe" COPYONLY)
configure_file("./ffprobe.exe" "./Release/ffprobe.exe" COPYONLY)
configure_file("./ffprobe.exe" "./Debug/ffprobe.exe" COPYONLY)
