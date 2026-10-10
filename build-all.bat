mkdir build
cd build

cmake -DPROJECT_NAME="VideoLow_Nvidia" -DHARDWARE_ACCELERATION_DEFAULT=1 -DWITH_CONSOLE=0 ..
if errorlevel 1 exit /b 1
cmake --build . --config Release
if errorlevel 1 exit /b 1

cmake -DPROJECT_NAME="VideoLow_AMD" -DHARDWARE_ACCELERATION_DEFAULT=2 -DWITH_CONSOLE=0 ..
if errorlevel 1 exit /b 1
cmake --build . --config Release
if errorlevel 1 exit /b 1

cmake -DPROJECT_NAME="VideoLow" -DHARDWARE_ACCELERATION_DEFAULT=0 -DWITH_CONSOLE=0 ..
if errorlevel 1 exit /b 1
cmake --build . --config Release
if errorlevel 1 exit /b 1

cmake -DPROJECT_NAME="VideoLow_DEBUG" -DHARDWARE_ACCELERATION_DEFAULT=0 -DWITH_CONSOLE=1 ..
if errorlevel 1 exit /b 1
cmake --build . --config Release
if errorlevel 1 exit /b 1

if not defined CI pause