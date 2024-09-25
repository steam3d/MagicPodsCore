#
# MagicPodsCore
# https://github.com/steam3d/MagicPodsCore
#
# Copyright: 2020-2024 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
# License: GNU General Public License v3.0
#

from aap.packets.packet import Packet

class Init(Packet):
    """    
    Initialization handshake with aap service. This packet is different on iPhone. The reason is unknown."
    0  1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16 17
    00 00 04 00 01 00 02 00 00 00 00 00 00 00 00 00       
    00 00 04 00 00 00 00 00 00 00 00 00 00 00 00 00       Also work (not recommend to use)
    01 00 04 00 00 00 01 00 02 00 05 00 49 4e 05 00 a5 4f Response from AirPods (Unknown usage)     
    """
    def __init__(self):
        super().__init__()
        self.tag = "Init"

    def to_bytes(self):
        return bytes([0x00, 0x00, 0x04, 0x00, 0x01, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00])
    
    def is_supported(self, model):
        return True