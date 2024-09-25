import re
import bluetooth
import select
import threading
import time
from aap.enums.enums import NotificationsMode
from aap.packets.Init import Init
from aap.packets.notifications import Notifications
from aap.watchers.anc_control_watcher import AncControlWatcher
from aap.watchers.battery_watcher import BatteryWatcher

anc_watcher = AncControlWatcher()
battery_watcher = BatteryWatcher()

def socket_reader(sock):
    try:
        while True:
            readable, _, _ = select.select([sock], [], [], 0.5)
            if sock in readable:
                data = sock.recv(1024)
                if not data:
                    break                
                #print("r:", data.hex())
                anc_watcher.process_response(data)
                battery_watcher.process_response(data)
            else:
                time.sleep(0.1)
    except Exception as e:
        print(f"Reader error: {e}")
    finally:
        sock.close()
        print("Socket closed (Reader)")

def socket_writer(sock):
    try:
        while True:
            message = input()
            if message.lower() == "exit":
                break            
            sock.send(bytes(bytearray.fromhex(message)))            
            print("s:",bytes(int(b,16) for b in re.findall('..',message)).hex())
    except Exception as e:
        print(f"Writer error: {e}")
    finally:
        sock.close()
        print("Socket closed (Writer)")

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
    print("\nType 'exit' to close. Type any hex string to send to the AirPods. For example, (0400040009000d01000000).\n")
    sock = bluetooth.BluetoothSocket(bluetooth.L2CAP)
    sock.connect((mac, port))

    # Handshake with AirPods
    sock.send(Init().to_bytes())
    
    # Subscribe to AirPods changes, such as battery status, etc.
    sock.send(Notifications(NotificationsMode.Unknown1).to_bytes())  
    
    #Sending and receiving is written in a hurry, so just press enter to read data.
    reader_thread = threading.Thread(target=socket_reader, args=(sock,))
    writer_thread = threading.Thread(target=socket_writer, args=(sock,))

    reader_thread.start()
    writer_thread.start()

    reader_thread.join()
    writer_thread.join()

    print("Program finished.")