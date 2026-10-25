# MagicPodsCore for Linux

A powerful Bluetooth backend with a simple WebSocket API for AirPods, Beats, Galaxy Buds, and Pixel Buds.

Build your own apps without dealing with complex Bluetooth protocols, or get started instantly with our ready-to-use apps below.

## 💻 Ready-to-use apps

- [MagicPods for Linux](https://github.com/steam3d/MagicPodsLinux) - A desktop app for Linux with battery monitoring, case open animations, noise control, and advanced headphone settings.
- [MagicPods for Decky Loader](https://github.com/steam3d/MagicPodsDecky) - A Steam plugin for controlling your headphones directly from Gaming Mode.

## 🎨 Core features

🎧 One API for all brands  
🔋 Battery level  
⚙️ Noise control  
🔗 Connect and disconnect headphones  
📶 Turn Bluetooth on or off  
🎵 Switch Bluetooth audio profiles

## 🔥 Advanced features for AirPods & Beats

Feature availability depends on the headphone model and firmware.

- Case open animation events
- Automatic ear detection actions
- High-resolution virtual microphone
- Custom equalizer
- Press and hold action for each AirPod (voice assistant, noise control, volume control)
- Listening modes assigned to press and hold controls
- Digital Crown direction
- Conversation awareness
- Personalized volume
- Adaptive audio noise level
- Noise cancellation with one AirPod
- Off listening mode
- Stem press events (single, double, triple, or long press)
- Press speed and press and hold duration
- Volume swipe and swipe length
- Tone volume
- Call controls
- Sleep detection
- Optimized charging
- Hearing protection
- Firmware information

## 🎧 Supported headphones

| Apple            | Beats                  | Samsung           | Google              |
| ---------------- | ---------------------- | ----------------- | ------------------- |
| AirPods 1        | PowerBeats Pro         | Galaxy Buds       | Pixel Buds Pro      |
| AirPods 2        | PowerBeats Pro 2       | Galaxy Buds Plus  | Pixel Buds Pro 2    |
| AirPods 3        | PowerBeats 3           | Galaxy Buds Live  | Pixel Buds 2a       |
| AirPods 4        | PowerBeats 4           | Galaxy Buds Pro   | Pixel Buds A-Series |
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

Note: Some headphone models do not support noise control or other advanced features.

## 🚀 Quick start

### Ubuntu / SteamOS:

Build
```
git clone https://github.com/steam3d/MagicPodsCore.git && \
cd MagicPodsCore && \
docker build -o . . && \
chmod +x magicpodscore
```

Run
```
./magicpodscore
```

Connect to 172.0.1.0:2020 WebSocket and use the API reference below to communicate with MagicPodsCore.

## 📘 API reference

Explore the WebSocket API, available commands, events, and JSON examples.

📄 [`api-reference.md`](./api-reference.md)

## 🧪 Feedback and bug reports

In the [Discord](https://discord.com/invite/8XZmDQwen6) community you can suggest an idea or report a problem.

## 🩼 Known issues

MagicPodsCore may become unresponsive when running inside VirtualBox.

## 💰 Donate

[Support the project here](https://magicpods.app/donate/) — every bit helps ❤️

## 💖 Developers

Developed by [Aleksandr Maslov](https://github.com/steam3d/) and [Andrey Litvintsev](https://github.com/andreylitvintsev)
