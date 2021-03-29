cmake_minimum_required(VERSION 3.5)

project(VideoLow LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check http://doc.qt.io/qt-5/deployment-android.html for more information.
# They need to be set before the find_package(Qt5 ...) call.

find_package(Qt5 COMPONENTS Widgets REQUIRED)


  add_executable(VideoLow
    main.cpp
    videolowwindow.cpp
    videolowwindow.h
    videolowwindow.ui
    filedropwidget.h
    filedropwidget.cpp
    ffmpegwrapper.h
    ffmpegwrapper.cpp
    Structs.h
  )

target_link_libraries(VideoLow PRIVATE Qt5::Widgets Qt5::WinMain)

#deploy QT libraries
get_target_property(_qmake_executable Qt5::qmake IMPORTED_LOCATION)
get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)

find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${_qt_bin_dir}")
add_custom_command(TARGET VideoLow POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E env PATH="${_qt_bin_dir}" "${WINDEPLOYQT_EXECUTABLE}" "$<TARGET_FILE:VideoLow>"
    COMMENT "Deploying Qt library files...")

#copy ffmpeg
configure_file("./ffmpeg.exe" "./ffmpeg.exe" COPYONLY)
configure_file("./ffmpeg.exe" "./Release/ffmpeg.exe" COPYONLY)
configure_file("./ffmpeg.exe" "./Debug/ffmpeg.exe" COPYONLY)