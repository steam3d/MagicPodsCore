#
# MagicPodsCore
# https://github.com/steam3d/MagicPodsCore
#
# Copyright: 2020-2024 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
# License: GNU General Public License v3.0
#

from aap.enums.enums import AncMode, Cmd, Cmd_Settings
from aap.watchers.watcher import Watcher

class AncControlWatcher(Watcher):
    """
    AncControlWatcher uses the same packet as AncControl
    """
    mode = -1 
    def __init__(self):
        self.tag = "AncControlWatcher"
        self.subscribers = []

    def subscribe(self, callback):        
        self.subscribers.append(callback)

    def unsubscribe(self, callback):        
        self.subscribers.remove(callback)

    def notify_subscribers(self, mode):        
        for callback in self.subscribers:
            callback(mode)

    def process_response(self, b_arr):
        # Minimum packet length
        if len(b_arr) < 10:
            return

        # Package type must be Settings
        if b_arr[4] != Cmd.Settings:
            return

        # The settings type must be Anc
        if b_arr[6] != Cmd_Settings.Anc:
            return

        # Проверка на существование значения
        if not any(value == b_arr[7] for value in AncMode):
            print("Unknown anc value", b_arr[7])
            return
        
        mode = self.get_enum_name(b_arr[7], AncMode)
        #print("AncControlWatcher:", mode)
        self.mode = b_arr[7]
        self.notify_subscribers(mode)