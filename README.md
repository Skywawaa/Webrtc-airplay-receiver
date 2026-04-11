# OBS AirPlay Receiver

An OBS Studio plugin that receives **AirPlay screen mirroring** from Apple devices (iPhone, iPad, Mac) and displays it as a video source with audio.

## Features

- **Screen Mirroring** - Receive AirPlay screen mirroring as an OBS source
- **Audio Support** - AAC audio decoded and synced with video
- **Auto Discovery** - mDNS/Bonjour advertisement so Apple devices find the receiver automatically
- **Hardware Decoding** - Tries NVIDIA NVDEC, Intel QSV, D3D11VA before falling back to software H.264 decoding
- **Low Latency** - Direct H.264 frame pipeline with minimal buffering
- **Cross-Platform** - Windows (primary), with Linux/macOS build support

## How It Works

```
iPhone/iPad/Mac                    OBS Studio
     |                                |
     |--- mDNS Discovery ----------->|  (finds "OBS AirPlay Receiver")
     |--- HTTP /info ---------------->|  (device capabilities exchange)
     |--- HTTP /pair-setup --------->|  (authentication)
     |--- HTTP /stream -------------->|  (start mirroring)
     |                                |
     |=== H.264 Video Stream =======>|  --> FFmpeg Decode --> OBS Source
     |=== AAC Audio Stream =========>|  --> FFmpeg Decode --> OBS Audio
```

## Requirements

### Runtime
- OBS Studio 30+ (tested with 32.1.1)
- Windows 10/11 64-bit

### Build Dependencies
- CMake 3.16+
- Visual Studio 2019+ (or GCC/Clang on Linux/macOS)
- FFmpeg development libraries (libavcodec, libavutil, libswscale, libswresample)
- OBS Studio source headers

## Building

### Windows (Quick Start)

1. **Clone the repo**
   ```bash
   git clone https://github.com/aomkoyo/obs-airplay-receiver.git
   cd obs-airplay-receiver
   ```

2. **Get dependencies** (if not already in `deps/`)
   ```bash
   # OBS SDK headers
   curl -L -o deps/obs-sdk.zip https://github.com/obsproject/obs-studio/archive/refs/tags/32.1.1.zip
   cd deps && unzip obs-sdk.zip -d obs-sdk && cd ..

   # FFmpeg dev (BtbN builds)
   curl -L -o deps/ffmpeg.zip https://github.com/BtbN/FFmpeg-Builds/releases/download/latest/ffmpeg-master-latest-win64-gpl-shared.zip
   cd deps && unzip ffmpeg.zip && cd ..

   # Generate OBS import library (requires VS Developer Command Prompt)
   # Use pefile: pip install pefile
   python -c "import pefile; pe=pefile.PE('C:/Program Files/obs-studio/bin/64bit/obs.dll'); f=open('deps/obs.def','w'); f.write('LIBRARY obs\nEXPORTS\n'); [f.write(f'    {e.name.decode()}\n') for e in pe.DIRECTORY_ENTRY_EXPORT.symbols if e.name]"
   # Then in VS Developer Command Prompt:
   lib /def:deps/obs.def /out:deps/obs.lib /machine:x64
   ```

3. **Build**
   ```bat
   :: Open VS Developer Command Prompt, then:
   mkdir build && cd build
   cmake .. -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=cl
   nmake
   ```

4. **Install**
   ```bat
   :: Copy to OBS plugins directory
   mkdir "%APPDATA%\obs-studio\plugins\obs-airplay-receiver\bin\64bit"
   copy build\obs-airplay-receiver.dll "%APPDATA%\obs-studio\plugins\obs-airplay-receiver\bin\64bit\"
   ```

### Linux

```bash
sudo apt install cmake build-essential pkg-config libobs-dev \
  libavcodec-dev libavutil-dev libswscale-dev libswresample-dev

mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# Install
mkdir -p ~/.config/obs-studio/plugins/obs-airplay-receiver/bin/64bit
cp obs-airplay-receiver.so ~/.config/obs-studio/plugins/obs-airplay-receiver/bin/64bit/
```

## Usage

1. Start OBS Studio
2. Add a new source: **Sources** > **+** > **AirPlay Receiver**
3. Configure:
   - **Server Name** - How the receiver appears on Apple devices (default: "OBS AirPlay Receiver")
   - **Port** - AirPlay port (default: 7000)
4. On your Apple device: open Control Center > Screen Mirroring > select "OBS AirPlay Receiver"
5. The mirrored screen appears in OBS with audio

## Architecture

```
obs-airplay-receiver/
├── src/
│   ├── plugin-main.c              # OBS module entry point
│   ├── airplay-source.c/h         # OBS source (video + audio output)
│   ├── audio-decoder.c/h          # AAC → PCM (FFmpeg)
│   ├── video-decoder.c/h          # H.264 → NV12 (FFmpeg, HW accel)
│   ├── airplay/
│   │   ├── airplay-server.c/h     # AirPlay protocol coordinator
│   │   ├── airplay-mirror.c/h     # Mirror stream receiver (H.264 + AAC)
│   │   └── airplay-plist.c/h      # Apple plist builder/parser
│   └── network/
│       ├── http-server.c/h        # HTTP server for AirPlay control
│       ├── mdns-publish.c/h       # mDNS/Bonjour service advertisement
│       └── net-utils.c/h          # Cross-platform socket utilities
└── CMakeLists.txt
```

## Known Limitations

- **FairPlay DRM**: Modern iOS (15+) requires FairPlay-SAP authentication for AirPlay screen mirroring. The current implementation has stub FairPlay handlers. For full compatibility, the crypto layer needs proper key exchange (similar to UxPlay/RPiPlay approach).
- **AirPlay 2**: Partial support. Full AirPlay 2 requires additional protocol features.
- **One client at a time**: Screen mirroring supports a single connected device.

## Troubleshooting

- **Device doesn't see the receiver**: Check Windows Firewall - allow TCP port 7000 and UDP port 5353 (mDNS)
- **Black screen after connecting**: May be a FairPlay authentication issue with newer iOS versions
- **No audio**: Ensure the source's audio monitoring is enabled in OBS mixer

## License

MIT

## Credits

Built with:
- [OBS Studio](https://obsproject.com/) plugin API
- [FFmpeg](https://ffmpeg.org/) for H.264/AAC decoding
- AirPlay protocol documentation from the open-source community
