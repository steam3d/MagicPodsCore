import re
import bluetooth

from aap.enums.enums import NotificationsMode
from aap.packets.Init import Init
from aap.packets.notifications import Notifications

def print_available_services(mac):
    services = bluetooth.find_service(address=mac)

    if len(services) > 0:
        print("Found {} services on {}.".format(len(services), mac))
    else:
        print("No services found.")

    for svc in services:
        print("\nService Name:", svc["name"])
        print("    Host:       ", svc["host"])
        print("    Description:", svc["description"])
        print("    Provided By:", svc["provider"])
        print("    Protocol:   ", svc["protocol"])
        print("    channel/PSM:", svc["port"])
        print("    svc classes:", svc["service-classes"])
        print("    profiles:   ", svc["profiles"])
        print("    service id: ", svc["service-id"])

if __name__ == "__main__":
    mac = "00:00:00:00:00:00" # Replace for AirPods mac 
    port = 4097 # Port aap

    print_available_services(mac)

    sock = bluetooth.BluetoothSocket(bluetooth.L2CAP)
    sock.connect((mac, port))

    # Handshake with AirPods
    sock.send(Init().to_bytes())
    
    # Subscribe to AirPods changes, such as battery status, etc.
    sock.send(Notifications(NotificationsMode.Unknown1).to_bytes())  

    #Sending and receiving is written in a hurry, so just press enter to read data.

    # TODO: Make async read and write to socket
    try:
        while True:        
            try:                
                data = input()
                if data:
                    sock.send(bytes(bytearray.fromhex(data)))
                    byts = bytes(int(b,16) for b in re.findall('..',data))
                    print("s:",byts.hex())
                    sock.send(byts)
                    
                data = sock.recv(1024)
                if not data:
                    break
                print("r:", data.hex())

            except Exception as e:
                print(e)
    finally:
        sock.close()
        print("Socked is closed")