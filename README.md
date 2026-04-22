# OBS AirPlay Receiver

[![Build](https://github.com/aomkoyo/obs-airplay-receiver/actions/workflows/build.yml/badge.svg)](https://github.com/aomkoyo/obs-airplay-receiver/actions/workflows/build.yml)
[![License: LGPL v2.1](https://img.shields.io/badge/License-LGPL_v2.1-blue.svg)](LICENSE)
[![Release](https://img.shields.io/github/v/release/aomkoyo/obs-airplay-receiver)](https://github.com/aomkoyo/obs-airplay-receiver/releases)

This repository contains two tools built on the same UxPlay AirPlay 2 library:

1. **OBS Plugin** (`obs-airplay-receiver.dll`) — receives AirPlay screen mirroring and displays it as an OBS Studio source.
2. **Standalone Streamer** (`airplay-stream.exe`) — receives AirPlay screen mirroring and re-streams it as MPEG-TS over TCP so you can open it directly in **VLC** or any media player, without OBS.

> **Built entirely with [Claude Code](https://claude.ai/code)** (Anthropic's AI coding agent) — from protocol implementation to CI/CD pipeline. This project is a Windows port of [mika314/obs-airplay](https://github.com/mika314/obs-airplay), using [UxPlay](https://github.com/FDH2/UxPlay)'s battle-tested AirPlay 2 protocol library.

---

## OBS Plugin

### Features

- **Screen Mirroring** - Receive AirPlay screen mirroring as a native OBS source
- **Audio Support** - AAC audio decoded and synced with video
- **Configurable Resolution** - Set width and height to match your scene layout
- **Configurable FPS** - Control the maximum frame rate of the mirrored stream
- **Auto Discovery** - mDNS/Bonjour advertisement so Apple devices find the receiver automatically
- **Hardware Decoding** - Tries NVIDIA NVDEC, Intel QSV, and D3D11VA before falling back to software H.264 decoding
- **Low Latency** - Direct H.264 frame pipeline with minimal buffering

### Requirements

- **OBS Studio 30+** (tested with 32.1.1)
- **Windows 10/11 64-bit**
- **Apple Bonjour** - Required for mDNS discovery so Apple devices can find the receiver. Install [iTunes](https://www.apple.com/itunes/) or the [Bonjour Print Services](https://support.apple.com/kb/DL999) package.

### Installation

1. Download the latest release `.zip` from the [Releases](../../releases) page
2. Extract the zip contents
3. Run `install.bat` as Administrator, or manually copy the files:
   - `obs-airplay-receiver.dll` to `%PROGRAMDATA%\obs-studio\plugins\obs-airplay-receiver\bin\64bit\`
   - `libcrypto-3-x64.dll` to the same directory
4. Restart OBS Studio

To uninstall, run `uninstall.bat` as Administrator or delete the `%PROGRAMDATA%\obs-studio\plugins\obs-airplay-receiver` folder.

### Usage

1. Start OBS Studio
2. Click **+** under Sources and select **AirPlay Receiver**
3. Configure the source settings:
   - **Server Name** - How the receiver appears on Apple devices (default: "OBS AirPlay Receiver")
   - **Width / Height** - Resolution of the received stream
   - **Max FPS** - Maximum frame rate
4. On your Apple device, open **Control Center** > **Screen Mirroring** and select the server name
5. The mirrored screen appears in OBS with audio

---

## Standalone Streamer (`airplay-stream.exe`)

**No OBS required.** `airplay-stream.exe` is a command-line tool that:

- Advertises itself on the local network via mDNS/Bonjour (same as the plugin)
- Accepts an AirPlay screen-mirroring connection from any Apple device
- Re-streams the content as **MPEG-TS over TCP** so you can open it in VLC or any compatible player

### How it works

```
iPhone/iPad/Mac  ──(AirPlay)──►  airplay-stream.exe  ──(TCP MPEG-TS)──►  VLC / mpv / ffplay
```

- **Video**: H.264 frames are passed directly into the MPEG-TS container (no decode/re-encode — low CPU, no quality loss).
- **Audio**: AAC-ELD audio is decoded and re-encoded as AAC-LC for maximum player compatibility.

### Usage

```bat
airplay-stream.exe [options]
```

| Option | Default | Description |
|---|---|---|
| `--name <name>` | `AirPlay Stream` | Server name shown on Apple device |
| `--port <port>` | `8888` | TCP port for the MPEG-TS stream |
| `--width <px>` | device native | Requested video width |
| `--height <px>` | device native | Requested video height |
| `--fps <fps>` | `60` | Requested frame rate |
| `--help` | | Show help and exit |

**Example:**

```bat
airplay-stream.exe --name "My Stream" --port 8888
```

Then open in VLC:
- Command line: `vlc tcp://localhost:8888`
- GUI: **Media → Open Network Stream** → `tcp://localhost:8888`

Or with FFplay: `ffplay tcp://localhost:8888`

### Standalone Streamer — Firewall

Allow the following through Windows Firewall:
- **TCP port 7000** (AirPlay)
- **TCP port `<stream-port>`** (MPEG-TS output, default 8888)
- **UDP port 5353** (mDNS/Bonjour)

---

### OBS Plugin — Firewall

If your Apple device cannot find the receiver, make sure Windows Firewall allows:
- **TCP port 7000** (AirPlay)
- **UDP port 5353** (mDNS/Bonjour)

## Building from Source

### Prerequisites

- Visual Studio 2019 or 2022 with C/C++ workload (MSVC compiler)
- CMake 3.16+
- OpenSSL (install via `choco install openssl` or Scoop)
- Git (for submodules)

### Steps

All commands below should be run from a **VS Developer Command Prompt**.

1. **Clone with submodules**
   ```bash
   git clone --recursive https://github.com/aomkoyo/obs-airplay-receiver.git
   cd obs-airplay-receiver
   ```

2. **Download dependencies** into the `deps/` directory:
   - **OBS Studio SDK** (32.1.1) - extract to `deps/obs-sdk/obs-studio-32.1.1/`
     ```bash
     curl -L -o deps/obs-sdk.zip https://github.com/obsproject/obs-studio/archive/refs/tags/32.1.1.zip
     cd deps && mkdir obs-sdk && cd obs-sdk && tar -xf ../obs-sdk.zip && cd ..\..
     ```
   - **FFmpeg 7.1 headers** - extract to `deps/ffmpeg7-include/`
   - **libplist 2.7.0** - extract to `deps/libplist-2.7.0/`

3. **Generate import libraries** from OBS DLLs:
   ```bat
   lib /def:deps\obs.def /out:deps\obs.lib /machine:x64
   lib /def:deps\w32-pthreads.def /out:deps\w32-pthreads.lib /machine:x64
   lib /def:deps\dnssd.def /out:deps\dnssd.lib /machine:x64
   ```

4. **Build libplist**
   ```bat
   cd deps\libplist-2.7.0
   mkdir build && cd build
   cmake .. -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=cl
   nmake
   cd ..\..\..
   ```

5. **Build UxPlay libraries** (playfair, llhttp, airplay)
   ```bat
   cd deps\uxplay-build
   mkdir build && cd build
   cmake .. -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=cl
   nmake
   cd ..\..\..
   ```

6. **Build the plugin** (and optionally the standalone streamer)
   ```bat
   mkdir build && cd build
   cmake .. -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=cl
   nmake
   ```

   To also build `airplay-stream.exe`, add `-DBUILD_STANDALONE=ON`:
   ```bat
   cmake .. -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=cl -DBUILD_STANDALONE=ON
   nmake
   ```

7. Output files in the `build/` directory:
   - `obs-airplay-receiver.dll` — OBS plugin
   - `standalone/airplay-stream.exe` — standalone streamer (if `BUILD_STANDALONE=ON`)

## Troubleshooting

- **Device doesn't see the receiver** - Check that Bonjour is installed and Windows Firewall allows TCP 7000 and UDP 5353
- **No audio (OBS plugin)** - Ensure the source's audio monitoring is enabled in the OBS audio mixer
- **No audio (standalone)** - VLC sometimes needs a moment to buffer; try pausing and unpausing
- **VLC can't connect** - Check that Windows Firewall allows the stream port (default TCP 8888); try `telnet localhost 8888`
- **One client at a time** - AirPlay screen mirroring supports a single connected device

## Built with Claude Code

This entire project — from initial protocol implementation through debugging, Windows porting, CI/CD pipeline, and installer — was built using [Claude Code](https://claude.ai/code), Anthropic's AI coding agent. The development process included:

- Porting UxPlay's POSIX-only AirPlay library to Windows (patching pthreads, sockets, endianness, timing)
- Debugging real-time AirPlay connections with an actual iPhone
- Building a complete CI pipeline that downloads and compiles 5 dependencies from source
- Creating an Inno Setup installer for one-click installation

## Credits

- [UxPlay](https://github.com/FDH2/UxPlay) - Open-source AirPlay 2 server (core protocol: FairPlay, pairing, encryption)
- [mika314/obs-airplay](https://github.com/mika314/obs-airplay) - Original OBS AirPlay plugin for Linux (inspiration and reference)
- [FFmpeg](https://ffmpeg.org/) - H.264 and AAC-ELD decoding
- [OBS Studio](https://obsproject.com/) - Plugin API
- [libplist](https://github.com/libimobiledevice/libplist) - Apple binary plist format
- [OpenSSL](https://www.openssl.org/) - Cryptography (AES, SHA, Ed25519)

## Contributing

Contributions welcome! Please open an issue or pull request.

## License

LGPL-2.1 — same as UxPlay, which this project depends on. See [LICENSE](LICENSE).
