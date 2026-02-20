# MagicPodsCore WebSocket JSON API

This document describes the public WebSocket API used by MagicPodsCore to communicate with the frontend.

All messages are sent as text WebSocket frames in JSON format.

## Connection

Connect to the WebSocket at the following address:

```
ws://172.0.1.0:2020/
```

After connecting to the WebSocket, MagicPodsCore sends an `init` message and then starts sending notifications about changes in `info`, `headphone`, and `defaultbluetooth` states.

```json
{
  "init": {
    "api": 0,
    "version": "2.0.7"
  }
}
```

Clients should request the initial state using the `GetAll` method after connection.

## Errors

MagicPodsCore returns an empty string response if invalid data is received, or ignores the `SetCapabilities` method when invalid data is provided.


# Methods

## Bluetooth adapter

### GetDefaultBluetoothAdapter

Returns the status of the active Bluetooth adapter.

Request:

```json
{ "method": "GetDefaultBluetoothAdapter" }
```

Response:
```json
{
  "defaultbluetooth": { "enabled": true }
}
```

### EnableDefaultBluetoothAdapter

Enables the active Bluetooth adapter.

Request:

```json
{ "method": "EnableDefaultBluetoothAdapter" }
```

Response:

```json
{
  "defaultbluetooth": { "enabled": true }
}
```

### DisableDefaultBluetoothAdapter

Disables the active Bluetooth adapter.

Request:

```json
{ "method": "DisableDefaultBluetoothAdapter" }
```

Response:

```json
{
  "defaultbluetooth": { "enabled": false }
}
```

### Broadcast (Notification)

If the connection status of the active Bluetooth adapter changes:

```json
{
  "defaultbluetooth": { "enabled": true }
}
```

## Headphones

### GetDevices

Returns a list of supported headphones on the system.

Request:

```json
{ "method": "GetDevices" }
```

Response:
```json
{
  "headphones": [
    {
      "name": "AirPods",
      "address": "AA:BB:CC:DD:EE:FF",
      "vendor": 76,
      "model": 8207,
      "color": 0,
      "connected": false
    },
    {
      "name": "Galaxy Buds 3",
      "address": "11:22:33:44:55:66",
      "vendor": 117,
      "model": 9,
      "color": 0,
      "connected": false
    }
  ]
}
```

Returns empty `headphones` if no supported headphones are found or if no headphones are paired with the system:

```json
{
  "headphones": []
}
```

### ConnectDevice

Connects to the headphones at the specified address.

Request:

```json
{
  "method": "ConnectDevice",
  "arguments": { "address": "AA:BB:CC:DD:EE:FF" }
}
```

Response:

```json
{
  "headphones": [
    {
      "name": "AirPods",
      "address": "AA:BB:CC:DD:EE:FF",
      "vendor": 76,
      "model": 8207,
      "color": 0,
      "connected": true
    },
    {
      "name": "Galaxy Buds 3",
      "address": "11:22:33:44:55:66",
      "vendor": 117,
      "model": 9,
      "color": 0,
      "connected": false
    }
  ]
}
```

### DisconnectDevice

Disconnects from the headphones at the specified address.

Request:

```json
{
  "method": "DisconnectDevice",
  "arguments": { "address": "AA:BB:CC:DD:EE:FF" }
}
```

Response:

```json
{
  "headphones": [
    {
      "name": "AirPods",
      "address": "AA:BB:CC:DD:EE:FF",
      "vendor": 76,
      "model": 8207,
      "color": 0,
      "connected": false
    },
    {
      "name": "Galaxy Buds 3",
      "address": "11:22:33:44:55:66",
      "vendor": 117,
      "model": 9,
      "color": 0,
      "connected": false
    }
  ]
}
```

### Broadcast (Notification)

When the connection status of any headphone changes:

```json
{
  "headphones": [
    {
      "name": "AirPods",
      "address": "AA:BB:CC:DD:EE:FF",
      "vendor": 76,
      "model": 8207,
      "color": 0,
      "connected": true
    },
    {
      "name": "Galaxy Buds 3",
      "address": "11:22:33:44:55:66",
      "vendor": 117,
      "model": 9,
      "color": 0,
      "connected": false
    }
  ]
}
```

## Info

### GetActiveDeviceInfo

Gets details about the currently connected headphones.

After the connection is established, the device `capabilities` section is updated dynamically, as headphones do not provide their capabilities immediately.


Request:
```json
{ "method": "GetActiveDeviceInfo" }
```

Response:
```json
{
  "info": {
    "name": "AirPods",
    "address": "AA:BB:CC:DD:EE:FF",
    "vendor": 117,
    "model": 9,
    "color": 0,
    "connected": true,
    "capabilities": {
      "battery": {...},
      "anc": {...},
      "bluetoothCodec": {...}
    }
  }
}
```

Returns empty `capabilities` if no features are available:

```json
{
  "info": {
    "name": "AirPods",
    "address": "AA:BB:CC:DD:EE:FF",
    "vendor": 117,
    "model": 9,
    "color": 0,
    "connected": true,
    "capabilities": { }
  }
}
```

Returns empty `info` if no headphones are connected:

```json
{
    "info": {}
}
```

### Broadcast (Notification)

On any property change:

```json
{
  "info": {
    "name": "AirPods Pro",
    "address": "AA:BB:CC:DD:EE:FF",
    "vendor": 117,
    "model": 9,
    "color": 0,
    "connected": true,
    "capabilities": {
      "battery": {...},
      "anc": {...},
      "bluetoothCodec": {...}
    }
  }
}
```


### GetAll

Combines GetDevices, GetDefaultBluetoothAdapter, and GetActiveDeviceInfo into a single request.

Request:

```json
{ "method": "GetAll" }
```

Response:

```json
{
  "headphones": [ /* same as GetDevices */ ],
  "defaultbluetooth": { /* same as GetDefaultBluetoothAdapter */ },
  "info": { /* same as GetActiveDeviceInfo */ }
}
```

## Capabilities

### SetCapabilities

If a `capabilities` property is not marked as `readonly: true`, it can be modified.

Send one of the available values listed in `options` (if present) or the value specified in `selected` for each `capabilities` property, as described in this documentation.

Request:

```json
{
  "method": "SetCapabilities",
  "arguments": {
    "address": "AA:BB:CC:DD:EE:FF",
    "capabilities": {
      "anc": { "selected": 16 }
    }
  }
}
```

Invalid or unsupported capability values are ignored.

### Capabilities structure

Each capability is returned as an object with the following structure, except for the battery level.

See `List of capabilities` for details on available features and values.


Example of a noise control capability:

```json
{
  "anc": {
    "selected": 1,
    "options": 23,
    "readonly": false
  }
}
```

| Field      | Description                                                     |
| ---------- | --------------------------------------------------------------- |
| `anc`      | Name of the capability used to make a `SetCapabilities` request |
| `selected` | Current selected value                                          |
| `options`  | Available values to select from (optional)                      |

### List of capabilities

#### Common capabilities

Common capabilities independent of the headphone manufacturer

##### Battery (readonly)

Unique capability structure that indicates the battery level of headphones. Always sends `single`, `left`, `right`, and `case` entries, even if the headphones support only some of them. Unavailable battery types will have the `NotAvailable` status.

```json
{
  "battery": {
    "single": { "battery": 0, "charging": false, "status": 0 },
    "left":   { "battery": 80, "charging": true,  "status": 2 },
    "right":  { "battery": 70, "charging": false, "status": 2 },
    "case":   { "battery": 40, "charging": false, "status": 3 },
    "readonly": true
  }
}
```

| Battery type |                                                                |
| ------------ | -------------------------------------------------------------- |
| `single`     | Battery info for non-TWS devices or averaged battery reporting |
| `left`       | Left earbud battery info                                       |
| `right`      | Right earbud battery info                                      |
| `case`       | Charging case battery info                                     |

| Bettery structure |             |
| ----------------- | ----------- |
| `battery`         | int (0-100) |
| `charging`        | bool        |
| `status`          | enum        |

| status |              |
| ------ | ------------ |
| `0`    | NotAvailable |
| `1`    | Disconnected |
| `2`    | Connected    |
| `3`    | Cached       |

##### Noise control

Controls switching between noise control modes.

```json
{
  "anc": {
    "selected": 16,
    "options": 19,
    "readonly": false
  }
}
```

| Field      |               |
| ---------- | ------------- |
| `selected` | int           |
| `options`  | bitmask (int) |
| `readonly` | bool          |

| options    |                   |
| ---------- | ----------------- |
| Bit 0 (1)  | Off               |
| Bit 1 (2)  | Transparency      |
| Bit 2 (4)  | Adaptive          |
| Bit 3 (8)  | WindCancellation  |
| Bit 4 (16) | NoiseCancellation |

##### Bluetooth codec

Supported Bluetooth profiles/codecs

```json
{
  "bluetoothCodec": {
    "selected": "a2dp-sink",
    "options": [
      ["a2dp-sink", "High Fidelity Playback (A2DP Sink)"],
      ["headset-head-unit", "Headset Head Unit (HSP/HFP)"]
    ],
    "readonly": false
  }
}
```

| Field      |                         |
| ---------- | ----------------------- |
| `selected` | string                  |
| `readonly` | bool                    |
| `options`  | array<[string, string]> |


| Tuple element |                                    |
| ------------- | ---------------------------------- |
| `[0]`         | Profile identifier                 |
| `[1]`         | Human-readable profile description |


### AirPods and Beats capabilities

##### Volume swipe

Adjust the volume by swiping up or down on the sensor located on the AirPods Pro stem.

```json
{
  "volumeSwipe": {
    "readonly": false,
    "selected": true
  }
}
```

| Field      |      |
| ---------- | ---- |
| `selected` | bool |
| `readonly` | bool |

| selected |         |
| -------- | ------- |
| `true`   | Enable  |
| `false`  | Disable |

##### Volume swipe length

Adjust the preferred waiting time between swipes.

```json
{
  "volumeSwipeLength": {
    "readonly": false,
    "selected": 0
  }
}
```

| Field      |      |
| ---------- | ---- |
| `selected` | enum |
| `readonly` | bool |

| selected |         |
| -------- | ------- |
| `0`      | Default |
| `1`      | Longer  |
| `2`      | Longest |

##### Press speed

Adjust the speed required to press the Left AirPod and the right AirPod twice or three times.

```json
{
  "pressSpeed": {
    "readonly": false,
    "selected": 0
  }
}
```

| Field      |      |
| ---------- | ---- |
| `selected` | enum |
| `readonly` | bool |


| selected |         |
| -------- | ------- |
| `0`      | Default |
| `1`      | Slower  |
| `2`      | Slowest |

##### Press and hold duration

Adjust the duration required to press and hold on your AirPods.

```json
{
  "pressAndHoldDuration": {
    "readonly": false,
    "selected": 0
  }
}
```

| Field      |      |
| ---------- | ---- |
| `selected` | enum |
| `readonly` | bool |


| selected |          |
| -------- | -------- |
| `0`      | Default  |
| `1`      | Shorter  |
| `2`      | Shortest |

##### Tone volume

Adjusts the volume of sound effects played by AirPods when no audio is playing.

```json
{
  "toneVolume": {
    "readonly": false,
    "selected": 29
  }
}
```

| selected |                     |
| -------- | ------------------- |
| `15-125` | Volume from 15-125% |

##### Adaptive audio noise

Adaptive audio dynamically responds to your environment and cancels or allows external noise.

```json
{
  "adaptiveAudioNoise": {
    "readonly": false,
    "selected": 50
  }
}
```

| selected |                                  |
| -------- | -------------------------------- |
| `0-100`  | 0 - more noise, 100 - less noise |

##### Personalized volume

Adjust the volume of media in response to your environment.

```json
{
  "personalizedVolume": {
    "readonly": false,
    "selected": false
  }
}
```

| Field      |      |
| ---------- | ---- |
| `selected` | enum |
| `readonly` | bool |

| selected |         |
| -------- | ------- |
| `true`   | Enable  |
| `false`  | Disable |

##### Conversation awareness

Lowers media volume and reduces background noise when you start speaking to other people.

```json
{
  "conversationAwareness": {
    "readonly": false,
    "selected": true
  }
}
```

| selected |         |
| -------- | ------- |
| `true`   | Enable  |
| `false`  | Disable |

##### Conversation awareness speaking (read-only)

Notifies when a conversation starts or ends.

```json
{
  "conversationAwarenessSpeaking": {
    "readonly": true,
    "selected": false
  }
}
```

| Field      | Type |
| ---------- | ---- |
| `selected` | bool |
| `readonly` | bool |

| selected |                    |
| -------- | ------------------ |
| `true`   | Start conversation |
| `false`  | Stop conversation  |

##### Noise cancellation with one AirPod

Allow AirPods to be put in noise cancellation mode when only one AirPod is in your ear.

```json
{
  "ancOneAirPod": {
    "readonly": false,
    "selected": false
  }
}
```

| selected |         |
| -------- | ------- |
| `true`   | Enable  |
| `false`  | Disable |

##### End call

Specifies how a call is ended. If ending a call is assigned to a single press, muting/unmuting the microphone is assigned to a double press, and vice versa.

```json
{
  "endCall": {
    "readonly": false,
    "selected": 2
  }
}
```

| Field      |      |
| ---------- | ---- |
| `selected` | enum |
| `readonly` | bool |

| selected |             |
| -------- | ----------- |
| `2`      | DoublePress |
| `3`      | SinglePress |


## Settings

A general settings storage. Settings are stored in containers in TOML format. Container names must use only Latin characters; the names must not include magicpods or MAC addresses. Allowed separators are _, -, or camelCase. Supported types are bool, string, int, and float.

Location:

```
~/.config/magicpods/config.toml
```

### GetSetting

Retrieves a setting value by container name and setting name.

Request:

```json
{
  "method": "GetSetting",
  "arguments": {
    "container": "myappname",
    "setting": "settingname"
  }
}
```

Response:

```json
{
  "settings": {
    "myappname": {
      "settingname": "value"
    }
  }
}

Returns an empty `settings` object if no setting is found:

```json
{
  "settings": {}
}
```

### SetSetting

Saves a setting value to the specified container.

Request:

```json
{
  "method": "SetSetting",
  "arguments": {
    "container": "myappname",
    "setting": "settingname",
    "value": "value"
  }
}
```

Response:

```json
{
  "settings": {
    "myappname": {
      "settingname": "value"
    }
  }
}

Returns an empty `settings` object if no setting is found:

```json
{
  "settings": {}
}

### Broadcast (Notification)

If any setting changes:

```json
{
  "settings":{
    "myappname":{
      "settingname": "value"
    }
  }
}
```




