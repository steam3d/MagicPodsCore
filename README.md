# MagicPodsCore💖 for Linux

A powerful Bluetooth backend with a simple WebSocket API for AirPods, Beats, Galaxy Buds, and Pixel Buds.

Build your own apps without dealing with complex Bluetooth protocols, or get started instantly with our ready-to-use apps below.

## Applications

- [MagicPods for Linux](https://github.com/steam3d/MagicPodsLinux) - Desktop application with battery monitoring, case-opening animations, noise control, and advanced headphone settings.
- [MagicPods for Decky Loader](https://github.com/steam3d/MagicPodsDecky) - Plugin for controlling headphones directly from Steam Gaming Mode.

## Core Features

- Battery levels and charging status
- Noise control
- Bluetooth adapter management
- Headphone connection and disconnection
- Bluetooth audio profiles and codecs
- Device information and capabilities
- WebSocket JSON API with commands and events

## Advanced AirPods & Beats Features

### Audio & Noise Control

- HD Microphone
- Conversation Awareness
- Personalized Volume
- Adaptive Audio
- Custom Equalizer
- Noise Cancellation with One AirPod
- Earbud Noise Control Modes
- Off Listening Mode

### Controls & Gestures

- Press-and-hold actions
- Press speed and press-and-hold duration
- Volume swipe gestures
- Call controls
- Digital Crown direction
- Stem press events

### Device Features

- Case-opening events
- Ear detection
- Audio output switching
- Headphone tone volume
- Sleep detection
- Optimized charging
- Hearing protection
- Firmware information

*Feature availability depends on the headphone model and firmware.*

## Supported Headphones

| Apple            | Beats                  | Samsung           | Google              |
| ---------------- | ---------------------- | ----------------- | ------------------- |
| AirPods 1        | PowerBeats Pro         | Galaxy Buds       | Pixel Buds A-Series |
| AirPods 2        | PowerBeats Pro 2       | Galaxy Buds Plus  | Pixel Buds Pro      |
| AirPods 3        | PowerBeats 3           | Galaxy Buds Live  | Pixel Buds Pro 2    |
| AirPods 4        | PowerBeats 4           | Galaxy Buds Pro   | Pixel Buds 2a       |
| AirPods 4 (ANC)  | Beats Fit Pro          | Galaxy Buds 2     |                     |
| AirPods 5        | Beats Studio Buds      | Galaxy Buds 2 Pro |                     |
| AirPods 5 (WCC)  | Beats Studio Buds Plus | Galaxy Buds FE    |                     |
| AirPods Pro      | Beats Studio Pro       | Galaxy Buds 3     |                     |
| AirPods Pro 2    | Beats Solo 3           | Galaxy Buds 3 Pro |                     |
| AirPods Pro 3    | Beats Solo Pro         | Galaxy Buds Core  |                     |
| AirPods Max      | Beats Studio 3         | Galaxy Buds 3 FE  |                     |
| AirPods Max 2024 | Beats X                | Galaxy Buds 4     |                     |
| AirPods Max 2    | Beats Flex             | Galaxy Buds 4 Pro |                     |
|                  | Beats Solo Buds        |                   |                     |
|                  | Beats Solo 4           |                   |                     |
|                  | Beats 360              |                   |                     |
|                  | Powerbeats Fit         |                   |                     |

*Some headphones support only basic features, such as battery monitoring and audio profile selection.*

## Quick Start

### Ubuntu / SteamOS:

Build
```bash
git clone https://github.com/steam3d/MagicPodsCore.git && \
cd MagicPodsCore && \
docker build -o . . && \
chmod +x magicpodscore
```

Run
```bash
./magicpodscore
```

Connect to the WebSocket server at `127.0.0.1:2020` to send commands and receive events.

## API Reference

Explore the WebSocket API, available commands, events, and JSON examples.

📄 [`api-reference.md`](./api-reference.md)

## Known Issues

MagicPodsCore may become unresponsive when running inside VirtualBox.

## Community

- [Join Discord to suggest features or report bugs](https://discord.com/invite/8XZmDQwen6).
- 💰 [Support the project](https://magicpods.app/donate/).

---

Developed by [Aleksandr Maslov](https://github.com/steam3d/) and [Andrey Litvintsev](https://github.com/andreylitvintsev).