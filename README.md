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

Responce

> Get Devices

### Get default Bluetooth adapter

Request

```
{
    "method":"GetDefaultBluetoothAdapter"
}
```

responce

```
{
  "defaultbluetooth": {
    "connected":"bool"
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

Responece

> Get default Bluetooth adapter

### Disable default Bluetooth adapter

Request

```
{
    "method":"DisableDefaultBluetoothAdapter"
}
```

Responece

> Get default Bluetooth adapter



