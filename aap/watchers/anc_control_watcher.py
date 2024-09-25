from aap.enums.enums import AncMode, Cmd, Cmd_Settings
from aap.watchers.watcher import Watcher

class AncControlWatcher(Watcher):
    """
    AncControlWatcher uses the same packet as AncControl
    """
    def __init__(self):
        self.tag = "AncControlWatcher"

    # TODO: add event

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
        print(mode)