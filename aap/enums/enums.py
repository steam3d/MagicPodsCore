from enum import IntEnum

class Cmd(IntEnum):
    Battery = 0x04
    """Battery change notification"""

    Settings = 0x09
    """Request to change or respond that the setting in the AirPods has been changed."""
    
    Notifications = 0x0f
    """To set up AirPods for broadcasting battery status, ear detection, and ANC changes"""

class Cmd_Settings(IntEnum):
    """
    To use you must set byte 4 Cmd.Settings and byte 6 one of the following (see packet.py):
    """
    Anc = 0x0d

class NotificationsMode(IntEnum):
    Unknown1 = 0xff
    """Always use this"""

    Unknown2 = 0xfe

class AncMode(IntEnum):
    Off = 0x01
    On = 0x02
    Tra = 0x03

class BatteryType(IntEnum):
    Single = 0x01
    Right = 0x02    
    Left = 0x04    
    Case = 0x08

class BatteryChargingStatus(IntEnum):
    Undefined = 0x00
    """Sometimes AirPods sends it when both AirPods out of the case"""
    Charging = 0x01
    NotCharging = 0x02
    Disconnected = 0x04    