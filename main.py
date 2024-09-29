import sys
from bt.service import Service
from ui.console import TreeApp

if __name__ == "__main__":
    from aap.packets.Init import Init
    from aap.packets.notifications import Notifications
    from aap.enums.enums import NotificationsMode, AncMode
    from aap.packets.anc_control import AncControl
    packets = {        
        "Init": Init(),
        "Notifications": [
            ("Unknown1", Notifications(NotificationsMode.Unknown1)),
            ("Unknown2", Notifications(NotificationsMode.Unknown2))
        ], 
        "Noise control": [
            ("Noise cancelling", AncControl(AncMode.On)),
            ("Off", AncControl(AncMode.Off)),
            ("Transparency", AncControl(AncMode.Tra))],
    }
    address = sys.argv[1]
    psm = 0x1001
    service = Service(address, psm)    
    app = TreeApp(service,packets)
    app.run()