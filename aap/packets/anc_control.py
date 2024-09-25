#
# MagicPodsCore
# https://github.com/steam3d/MagicPodsCore
#
# Copyright: 2020-2024 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
# License: GNU General Public License v3.0
#

from typing import Type
from aap.enums.enums import Cmd, Cmd_Settings, AncMode
from aap.enums.pids import ProductIds
from aap.packets.packet import Packet

class AncControl(Packet):
    """    
    0400040009000d01000000
    
    0  1  2  3  4  5  6  7  8  9  10
                         *
    04 00 04 00 09 00 0d 01 00 00 00
    04 00 04 00 09 00 0d 02 00 00 00
    04 00 04 00 09 00 0d 03 00 00 00
    
    7 byte:
    01 - off
    02 - on
    03 - transparency    
    
    AirPods send the same format of packet when the ANC state changes
    ---------------------------------------------------------------------------
    """
    def __init__(self, option: Type[AncMode]):
        super().__init__()
        self.tag = "AncControl"
        self.option = option

    def to_bytes(self):
        return bytes([0x04, 0x00, 0x04, 0x00, Cmd.Settings, 0x00, Cmd_Settings.Anc, self.option, 0x00, 0x00, 0x00])
    
    def is_supported(model):
        supported_models = [
            ProductIds.airpodspro,
            ProductIds.airpodspro2,
            ProductIds.airpodsprousbc,
            ProductIds.airpodsmax,
            ProductIds.beatsfitpro,
            ProductIds.airpodsmax2024,
            ProductIds.airpods4,
        ]
        return model in supported_models