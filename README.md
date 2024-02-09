# MagicPodsCore

## API reference

### Get devices

Request

```
{
    "method":"GetDevices"
}
```

Responce

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
        "address": "FC:1D:43:DE:80:49"
    }
}
```
Responce

> Get Devices

### Disconnect device

Request

```
{
    "method":"DisconnectDevice",
    "arguments": {
        "address": "FC:1D:43:DE:80:49"
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

### Get decky Info

Request

```
{
    "method":"GetDeckyInfo"
}
```

Response

```
{
  "info":{
    "name": "AirPods (steam3d)",
    "address": "12:45:ds:23:fd:12",
    "connected": true,
    "battery": {
      "s": "int",
      "sc": "bool",
      "l": "int",
      "lc": "bool",
      "r": "int",
      "rc": "bool",
      "c": "int",
      "cc": "bool"      
    },
    "capabilities": {
      "anc"?: "int"      
    } 
  }
}
```

if headphones do not have capabilities return empty:

```
{
  "capabilities": {}
}
```

### Get all

Collects requests `GetDevices` `GetDefaultBluetoothAdapter` `GetDeckyInfo` together

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
    "name": "AirPods (steam3d)",
    "address": "12:45:ds:23:fd:12",
    "connected": true,
    "battery": {
      "s": "int",
      "sc": "bool",
      "l": "int",
      "lc": "bool",
      "r": "int",
      "rc": "bool",
      "c": "int",
      "cc": "bool"      
    },
    "capabilities": {
      "anc?": "int"      
    } 
  }
}
```
