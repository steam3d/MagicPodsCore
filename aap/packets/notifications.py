#
# MagicPodsCore
# https://github.com/steam3d/MagicPodsCore
#
# Copyright: 2020-2024 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
# License: GNU General Public License v3.0
#

from typing import Type
from aap.enums.enums import Cmd, NotificationsMode
from aap.packets.packet import Packet


class Notifications(Packet):
    """
    0  1  2  3  4  5  6  7  8  9 
                            *
    04 00 04 00 0f 00 ff ff ff ff
    04 00 04 00 0f 00 ff ff fe ff 

    8 byte:
    ff - Unknown1
    fe - Unknown2

    Use Unknown1 to set up AirPods for broadcasting battery status, ear detection, and ANC changes

    Unknown behavior:
        iPhone sends:
        040004000f00fffffeff

        Then the iPhone receives some packets and sends:
        040004000f00ffffffff

        My guess is that the iPhone checks the capabilities of the AirPods and sends an additional packet based on them.   
    """
    def __init__(self, option: Type[NotificationsMode]):
        super().__init__()
        self.tag = "AncControl"
        self.option = option

    def to_bytes(self):
        return bytes([0x04, 0x00, 0x04, 0x00, Cmd.Notifications, 0x00, 0xff, 0xff, self.option, 0xff])
    
    def is_supported(model):
        return True