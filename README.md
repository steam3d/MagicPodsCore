# MagicPodsCore for Linux

Backend service exposing a WebSocket API for managing AirPods, Beats, Galaxy Buds, and Pixel Buds.

## 🎨 Features

🔋Battery level  
⚙️ Noise control

### 🔥 Exclusive to AirPods and Beats

- Conversation awareness
- Personalized volume
- Noise level adjustment in adaptive mode
- Noise cancellation with one AirPod
- Press duration adjustment
- Press and hold duration adjustment
- Customization of single and double tap for call control
- High-resolution AirPods microphone exposed as `MagicPods Virtual Mic`

## 🎧 Headphones supported

| Apple            | Beats                  | Samsung           | Google           |
| ---------------- | ---------------------- | ----------------- | ---------------- |
| AirPods 1        | PowerBeats Pro         | Galaxy Buds       | Pixel Buds Pro   |
| AirPods 2        | PowerBeats Pro 2       | Galaxy Buds Plus  | Pixel Buds Pro 2 |
| AirPods 3        | PowerBeats 3           | Galaxy Buds Live  |                  |
| AirPods 4        | PowerBeats 4           | Galaxy Buds Pro   |                  |
| AirPods 4 (ANC)  | Beats Fit Pro          | Galaxy Buds 2     |                  |
| AirPods 5        | Beats Studio Buds      | Galaxy Buds 2 Pro |                  |
| AirPods 5 (WCC)  | Beats Studio Buds Plus | Galaxy Buds FE    |                  |
| AirPods Pro      | Beats Studio Pro       | Galaxy Buds 3     |                  |
| AirPods Pro 2    | Beats Solo 3           | Galaxy Buds 3 Pro |                  |
| AirPods Pro 3    | Beats Solo Pro         |                   |                  |
| AirPods Max      | Beats Studio 3         |                   |                  |
| AirPods Max 2024 | Beats X                |                   |                  |
| AirPods Max 2    | Beats Flex             |                   |                  |
|                  | Beats Solo Buds        |                   |                  |
|                  | Powerbeats Fit         |                   |                  |

Some of the headphones in the table do not have or do not support the noise control feature.

## 🚀 Getting started

### For Ubuntu or Steam OS:

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

Complete reference for the MagicPodsCore WebSocket JSON API.

📄 [`api-reference.md`](./api-reference.md)

Example frontend projects using the MagicPodsCore:
- [MagicPodsDecky](https://github.com/steam3d/MagicPodsDecky)
- [MagicPods for Linux](https://github.com/steam3d/MagicPodsLinux)

## 🧪 Ideas and bugs

In the [Discord](https://discord.com/invite/8XZmDQwen6) community you can suggest an idea or report a problem.

## 🩼 Known issue

Stuck when running through VirtualBox.

## 💰 Donate

[Support the project here](https://magicpods.app/donate/) — every bit helps ❤️

## 💖 Developers

Developed by [Aleksandr Maslov](https://github.com/steam3d/) and [Andrey Litvintsev](https://github.com/andreylitvintsev)
