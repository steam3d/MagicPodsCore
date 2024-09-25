#
# MagicPodsCore
# https://github.com/steam3d/MagicPodsCore
#
# Copyright: 2020-2024 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
# License: GNU General Public License v3.0
#

from abc import ABC, abstractmethod

class Packet(ABC):
    """
    Most of the packets have this format
    0  1  2  3  4  5  6  7  8  9  10
    04 00 04 00 09 00 0d 01 00 00 00

    Byte 4 - Cmd type
    Byte 6 - Cmd subtype

    All known values are specified in the enums.py file
    """
    def __init__(self):
        self.tag = "Packet"

    @abstractmethod
    def to_bytes(self):
        pass

    @abstractmethod
    def is_supported(self, model):
        pass