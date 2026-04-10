# MagicPodsCore for Linux

Backend service exposing a WebSocket API for managing AirPods, Beats, and Galaxy Buds.

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

## 🎧 Headphones supported

| Apple            | Beats                  | Samsung           |
| ---------------- | ---------------------- | ----------------- |
| AirPods 1        | PowerBeats Pro         | Galaxy Buds       |
| AirPods 2        | PowerBeats Pro 2       | Galaxy Buds Plus  |
| AirPods 3        | PowerBeats 3           | Galaxy Buds Live  |
| AirPods 4        | PowerBeats 4           | Galaxy Buds Pro   |
| AirPods 4 (ANC)  | Beats Fit Pro          | Galaxy Buds 2     |
| AirPods Pro      | Beats Studio Buds      | Galaxy Buds 2 Pro |
| AirPods Pro 2    | Beats Studio Buds Plus | Galaxy Buds Fe    |
| AirPods Pro 3    | Beats Studio Pro       | Galaxy Buds 3     |
| AirPods Max      | Beats Solo 3           | Galaxy Buds 3 Pro |
| AirPods Max 2024 | Beats Solo Pro         |                   |
| AirPods Max 2    | Beats Studio 3         |                   |
|                  | Beats X                |                   |
|                  | Beats Flex             |                   |
|                  | Beats Solo Buds        |                   |
|                  | Powerbeats Fit         |                   |

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
- [MagicPodsPlasmoid](https://github.com/steam3d/MagicPodsPlasmoid)

## 🧪 Ideas and bugs

In the [Discord](https://discord.com/invite/8XZmDQwen6) community you can suggest an idea or report a problem.

## 🩼 Known issue

Stuck when running through VirtualBox.

## 💰 Donate

[Support the project here](https://magicpods.app/donate/) — every bit helps ❤️

## 💖 Developers

Developed by [Aleksandr Maslov](https://github.com/steam3d/) and [Andrey Litvintsev](https://github.com/andreylitvintsev)