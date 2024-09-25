#
# MagicPodsCore
# https://github.com/steam3d/MagicPodsCore
#
# Copyright: 2020-2024 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
# License: GNU General Public License v3.0
#

class Watcher():    
    def get_enum_name(self, value, enum):
        return next((enum_value.name for enum_value in enum if enum_value.value == value), "")