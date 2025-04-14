# MagicPodsCore for Linux

A command-line utility for managing AirPods, Beats, and Galaxy Buds.

## 🎨 Features

🔋Battery level  
⚙️ Noise control

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
| AirPods Max      | Beats Studio Pro       | Galaxy Buds 3     |
| AirPods Max 2024 | Beats Solo 3           | Galaxy Buds 3 Pro |
|                  | Beats Solo Pro         |                   |
|                  | Beats Studio 3         |                   |
|                  | Beats X                |                   |
|                  | Beats Flex             |                   |

Some of the headphones in the table do not have or do not support the noise control feature.

## 🚀 Getting started

### For Ubuntu or Steam OS:

Build
```
git clone https://github.com/steam3d/MagicPodsCore.git && \
cd MagicPodsCore && \
docker build -o . . && \
chmod +x MagicPodsCore
```

Run
```
./MagicPodsCore
```

Connect to 172.0.1.0:2020 WebSocket and use the API below to communicate with MagicPodsCore.

## 📘 API reference

List of available commands and events.
⚠️ The API is in alpha stage and may change between updates.

<details><summary>Get default Bluetooth adapter</summary>

Returns the status of the active Bluetooth adapter. If the connection status of active Bluetooth adapter changes, MagicPodsCore will notify (see Response section).

**Request:**

```
{
    "method":"GetDefaultBluetoothAdapter"
}
```

**Response:**

```
{
  "defaultbluetooth": {
    "enabled":"bool"
  }
}
```

Returns an empty object if no Bluetooth adapter is found:

```
{
  "defaultbluetooth": {}
}
```
</details>

<details><summary>Enable default Bluetooth adapter</summary>

Enables the active Bluetooth adapter.

**Request:**

```
{
    "method":"EnableDefaultBluetoothAdapter"
}
```

**Response:**

Same as `GetDefaultBluetoothAdapter` response.

</details>

<details><summary>Disable default Bluetooth adapter</summary>

Disables the active Bluetooth adapter.

**Request:**

```
{
    "method":"DisableDefaultBluetoothAdapter"
}
```

**Response:**

Same as `GetDefaultBluetoothAdapter` response.

</details>

<details><summary>Get devices</summary>

Returns a list of headphones supported by MagicPodsCore that are found in the system. If the connection status of any device changes, MagicPodsCore will notify (see Response section).

**Request:**

```
{
    "method":"GetDevices"
}
```

**Response:**

```
{
  "headphones": [{
    "name": "string",
    "address": "string",
    "connected":"bool"
  },
  {
    "name": "string",
    "address": "string",
    "connected":"bool"
  }]
}
```

Returns an empty list if no headphones are found:

```
{
  "headphones": []
}
```
</details>

<details><summary>Connect device</summary>

Connects to a pair of headphones by the specified `address`. You can get the address using the `GetDevices` method.

**Request:**

```
{
    "method":"ConnectDevice",
    "arguments": {
        "address": "string"
    }
}
```
**Response:**

Returns the list of headphones regardless of connection success — see the `GetDevices` response.

</details>


<details><summary>Disconnect device</summary>

Disconnects the headphones by the specified `address`. The address can be retrieved using the `GetDevices` method.

**Request:**

```
{
    "method":"DisconnectDevice",
    "arguments": {
        "address": "string"
    }
}
```

**Response:**

Returns the list of headphones regardless of disconnection success — see the `GetDevices` response.

</details>

<details><summary>Get active device info</summary>

Returns details about the currently connected device. If any property changes, MagicPodsCore will notify (see Response section).

**Request:**

```
{
    "method":"GetActiveDeviceInfo"
}
```

**Response:**

```
{
  "info":{
    "name": "string",
    "address": "string",
    "connected": "bool",
    "capabilities": {
      "battery": {
        "single": {
          "battery": "int",
          "charging": "bool",
          "status": "int"
        },
        "left": {
          "battery": "int",
          "charging": "bool",
          "status": "int"
        },
        "right": {
          "battery": "int",
          "charging": "bool",
          "status": "int"
        },
        "case": {
          "battery": "int",
          "charging": "bool",
          "status": "int"
        },
        "readonly": "bool"
      },
      "anc": {
        "options": "int",
        "selected": "int",
        "readonly": "bool"
      }
    }
  }
}
```




If no device is connected:

```
{
    "info": {}
}
```

If device has no capabilities:

```
{
  "capabilities": {}
}
```

</details>

<details><summary>Set capabilities</summary>

If a capability is not marked as read-only (`readonly: false`), you can change it. You can retrieve the list of available capabilities using `GetActiveDeviceInfo`. The `options` field shows available values, and `selected` shows the current one.

**Request:**

```
{
  "method": "SetCapabilities",
  "arguments": {
    "address": "string",
    "capabilities": {
      "capabilityname1": {
        "selected": "int"
      },
      "capabilityname2": {
        "selected": "int"
      }
    }
  }
}
```

</details>

<details><summary>Get all</summary>

Combines the following requests: `GetDevices`, `GetDefaultBluetoothAdapter`, and `GetActiveDeviceInfo`.

**Request:**

```
{
    "method":"GetAll"
}
```

**Response:**

```
{
  "headphones": [{
    "name": "string",
    "address": "string",
    "connected":"bool"
  },
  {
    "name": "string",
    "address": "string",
    "connected":"bool"
  }],

  "defaultbluetooth": {
    "enabled":"bool"
 },

  "info":{
      "name": "string",
      "address": "string",
      "connected": "bool",
      "capabilities": {
        "battery": {
          "single": {
            "battery": "int",
            "charging": "bool",
            "status": "int"
          },
          "left": {
            "battery": "int",
            "charging": "bool",
            "status": "int"
          },
          "right": {
            "battery": "int",
            "charging": "bool",
            "status": "int"
          },
          "case": {
            "battery": "int",
            "charging": "bool",
            "status": "int"
          },
          "readonly": "bool"
        },
        "anc": {
          "options": "int",
          "selected": "int",
          "readonly": "bool"
        }
      }
  }

}
```
</details>

<details><summary>Set log level</summary>

Sets the logging level.
⚠️ This method is not intended for public use.

**Request:**

```
{
  "method": "SetLogLevel",
  "arguments": {
      "selected": int,
  }
}
```

</details>

## 🧪 Ideas and bugs

In the [Discord](https://discord.com/invite/8XZmDQwen6) community you can suggest an idea or report a problem.

## 🩼 Known issue

Stuck when running through VirtualBox.

## 💰 Donate

[Support the project here](https://magicpods.app/donate/) — every bit helps ❤️

## 💖 Developers

Developed by [Aleksandr Maslov](https://github.com/steam3d/) and [Andrey Litvintsev](https://github.com/andreylitvintsev)