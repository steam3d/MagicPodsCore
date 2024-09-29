#
# MagicPodsCore
# https://github.com/steam3d/MagicPodsCore
#
# Copyright: 2020-2024 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
# License: GNU General Public License v3.0
#

from aap.enums.enums import Cmd, BatteryType, BatteryChargingStatus
from aap.watchers.watcher import Watcher

class BatteryWatcher(Watcher):
    """    
    04000400040003020164020104016301010801110201 AirPods 2
    0400040004000101015d0101                     AirPods Max

    0  1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16 17 18 19 20 21
                         |Right    |    |Left     |    |Case     |
    04 00 04 00 04 00 03 02 01 64 02 01 04 01 63 01 01 08 01 11 02 01 
                         |Left     |    |Right    |    |Case     |                 
    04 00 04 00 04 00 03 04 01 63 02 01 02 01 00 04 01 08 01 00 04 01
                         |Single   |
    04 00 04 00 04 00 01 01 01 5d 01 01

    6 byte:
    01-03 - count of batteries

    Each battery is separated by the number 0x01. 
    
    Consider a piece of the packet containing the right battery:
    0  1  2  3
    |Right    |
    02 01 64 02
    
    0 byte:
    01 - single
    02 - right battery
    04 - left battery
    08 - case

    2 byte:
    00    - possible shows that device is disconnected
    01-64 - charge level
    ff    - unknown

    4 bytes:
    01 - charging
    02 - not charging
    04 - disconnected, the battery level usually is 00

    This pattern is repeated for each individual battery in the AirPods.    
    """
    battery = []
    def __init__(self):
        self.tag = "BatteryWatcher"

    # TODO: add event

    def process_response(self, b_arr):
        # Minimum packet length
        if len(b_arr) < 11:
            return

        # Package type must be Battery
        if b_arr[4] != Cmd.Battery:
            return

        battery_count = b_arr[6]
        start_byte = 7
        battery = []
        batt_string = "BatteryWatcher: "
        for i in range(battery_count):
            if not any(value == b_arr[start_byte] for value in BatteryType):
                print("Unknown battery type value", b_arr[start_byte])
                start_byte += 5
                continue

            if not any(value == b_arr[start_byte + 3] for value in BatteryChargingStatus):
                print("Unknown battery type value", b_arr[start_byte + 3])
                start_byte += 5
                continue

            battery_type = BatteryType(b_arr[start_byte])
            battery_level = max(0, min(b_arr[start_byte + 2], 100))         
            status = self.get_enum_name(b_arr[start_byte + 3], BatteryChargingStatus)                                 
            batt_string += f"{battery_type.name} {battery_level} {status}, "
            battery.append((battery_type, battery_level,status))
            start_byte += 5
        
        self.battery = battery
        #print(self.battery)
        #print(batt_string)