<p align="center">
  <img
    alt="swiftray library logo"
    src="resources/images/icon.png"
    height="300"
    style="margin-top: 20px; margin-bottom: 20px;"
  />
</p>


# Swiftray

Swiftray is a free and open-sourced software for grbl-based laser cutters and engravers. 

**Features**

- Layer controls - cut and engrave simultaneously
- Divide by color - efficient workflows for third party design software
- Hack as you like - you can modify all codes to match you self-built lasers
- Blazing performance - written in C++
- Low memory usage - even runnable on embedded system
- Cross-platform - compiles on Windows and MacOS

## Requirements

- Compilers must support C++17 standards.
- Boost
- Qt Framework 5.15
- Qt Creator
- Qt Framework and Creator can be installed via [online installer](https://www.qt.io/download-open-source)
- OpenCV 4
- Potrace
- libxml2
- libiconv (Windows)
- icu4c (MacOS)
- poppler
- glib
- cairo

NOTE: For Mac M1, you need to build Qt 5.15

See process [here](https://github.com/bobwolff68/fritzing-app/wiki/Building-Qt-5.15-for-Mac-M1)

### Tools
- [msys2 (for Windows)](https://www.msys2.org/)
- [vcpkg dependency manager (for Windows)](https://vcpkg.io/en/getting-started.html)
- [Conan package manager (for Windows)](https://conan.io/downloads.html)


## Setup
### Clone the repo and checkout submodules
```
git clone https://github.com/flux3dp/swiftray.git
cd swiftray
git submodule update --init --recursive
```

### Build sentry-native from source
```
cd third_party/sentry-native
cmake -B build -S . \
  -DCMAKE_BUILD_TYPE=RelWithDebInfo \
  -DSENTRY_BACKEND=crashpad \
  -DSENTRY_INTEGRATION_QT=YES \
  -DCMAKE_PREFIX_PATH=<PATH_TO_QT>/lib/cmake
cmake --build build --config RelWithDebInfo --parallel
cmake --install build --prefix install
```
NOTE: For Windows MSVC(2019), you need to resolve the source code encoding issue of crashpad first.
      Alternatively, you can simply install sentry-native with vcpkg

See discussions [here](https://github.com/microsoft/vcpkg/issues/21888)

### Build libpotrace from source and handle it with Conan (for Windows)
in msys2
install the following packages first
```
pacman -S base-devel
pacman -S mingw-w64-x86_64-gcc
pacman -S mingw-w64-x86_64-autotools
pacman -S mingw-w64-x86_64-make
pacman -S mingw-w64-x86_64-zlib
pacman -S mingw-w64-x86_64-python-conan
```
then
```
$ cd third_party/libpotrace
$ curl https://potrace.sourceforge.net/download/1.16/potrace-1.16.tar.gz -o potrace-1.16.tar.gz
$ tar -xzvf potrace-1.16.tar.gz
$ conan install . --build=missing
$ conan build .
$ conan create . user/testing
```
## Building

### CMake

This project uses CMake to build.
For macOS
```bash
$ mkdir build
$ cd build
$ cmake ..
$ make -j12
```

For Windows
```
$ mkdir build
$ cd build
$ conan install .. --build=missing
$ cd ..
$ cmake -B build -S . -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCMAKE_TOOLCHAIN_FILE="C:\vcpkg\scripts\buildsystems\vcpkg.cmake" -DCMAKE_PREFIX_PATH=C:/Qt/5.15.2/msvc2019_64/lib/cmake
$ cmake --build build --config RelWithDebInfo --parallel
```

### Qt Creator (QMake)

Open the .pro project file in the root directory, and click run.

## Deploy

### MacOS
We can use built-in tool `macdeployqt` to deploy the app bundle generated by QMake
```bash
$ cd <bin folder in Qt>
$ ./macdeployqt <absolute path>/build-swiftray-Desktop_Qt_5_15_2_clang_64bit-Release/Swiftray.app -qmldir=<absolute path>/swiftray//src/windows/qml -dmg
```
### Windows
```
$ <Qt_Installed_Dir>/5.15.2/msvc2019_64/bin/windeployqt.exe --qmldir src/windows/qml --compiler-runtime build/bin/Swiftray.exe
```

## Coding Guides

1. Use Modern C++ as possible as you can.
2. Reduce logic implementation in widgets and QML codes, to maintain low coupling with Qt Framework.

## Document

Run `$ doxygen Doxygen` and view docs/index.html

## Affiliation

Swiftray is community developed, commercially supported for long-term development.

Swiftray is brought to you by the development team at FLUX Inc.


## License

Swiftray is GNU General Public License v3.0 licensed.
