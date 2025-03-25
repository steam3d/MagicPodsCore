# MagicPodsCore

## API reference

### Get devices

Request

```
{
    "method":"GetDevices"
}
```

Response

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

Empty if there are no headphones paired in the system

```
{
  "headphones": []
}
```

### Connect device

Request

```
{
    "method":"ConnectDevice",
    "arguments": {
        "address": "string"
    }
}
```
Response

> Get Devices

### Disconnect device

Request

```
{
    "method":"DisconnectDevice",
    "arguments": {
        "address": "string"
    }
}
```

Response

> Get Devices

### Get default Bluetooth adapter

Request

```
{
    "method":"GetDefaultBluetoothAdapter"
}
```

Response

```
{
  "defaultbluetooth": {
    "enabled":"bool"
  }
}
```

Empty if there are no Bluetooth adapter in the system

```
{
  "defaultbluetooth": {}
}
```

### Enable default Bluetooth adapter

Request

```
{
    "method":"EnableDefaultBluetoothAdapter"
}
```

Resonance

> Get default Bluetooth adapter

### Disable default Bluetooth adapter

Request

```
{
    "method":"DisableDefaultBluetoothAdapter"
}
```

Resonance

> Get default Bluetooth adapter

### GetActiveDeviceInfo

Request:

```
{
    "method":"GetActiveDeviceInfo"
}
```

Response:

```
{
  "info":{
    "name": "string",
    "address": "string",
    "connected": "bool",
    "capabilities": {
      "battery"?: {
        "s": "int",
        "sc": "bool",
        "l": "int",
        "lc": "bool",
        "r": "int",
        "rc": "bool",
        "c": "int",
        "cc": "bool"
      },
      "anc"?: {
        "options": "int",
        "selected": "int",
        "readonly": "bool"
      }
    }
  }
}
```

Empty if there are no connected headphones:

```
{
    "info": {}
}
```

Empty if headphones do not have capabilities:

```
{
  "capabilities": {}
}
```


### Get all

Collects requests `GetDevices` `GetDefaultBluetoothAdapter` `GetActiveDevice` together

Request

```
{
    "method":"GetAll"
}
```

Response

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
      "battery"?: {
        "s": "int",
        "sc": "bool",
        "l": "int",
        "lc": "bool",
        "r": "int",
        "rc": "bool",
        "c": "int",
        "cc": "bool"
      },
      "anc"?: {
        "options": "int",
        "selected": "int",
        "readonly": "bool"
      }
    }
  }
}
```

### Set capabilities

If a capability is not read-only, you can change it or change multiple capabilities at once.

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

Response

if capability supports notification you will GetInfo

Response

> Get decky Info