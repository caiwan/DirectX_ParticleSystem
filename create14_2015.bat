@echo off

if not exist msvc md msvc

cd msvc
cmake -DCMAKE_INSTALL_PREFIX=deploy/dev -DBUILD_LIVE_RELEASE=FALSE -DCMAKE_PREFIX_PATH=%CMAKE_PREFIX_PATH% -G "Visual Studio 14 2015 Win64" ../
cd ..
