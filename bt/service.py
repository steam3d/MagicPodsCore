import asyncio
import socket
import time

class Service:
    def __init__(self, address: str, psm: int):        
        self.address = address
        self.psm = psm
        self.reader = None
        self.writer = None
        self.subscribers = []
        self.running = True

    def subscribe(self, callback):        
        self.subscribers.append(callback)

    def unsubscribe(self, callback):        
        self.subscribers.remove(callback)

    def notify_subscribers(self, message):        
        for callback in self.subscribers:
            callback(message)

    async def Write(self, bytes:bytes):
        await self.command_queue.put(bytes)

    def WriteDirectly(self, bytes:bytes):
        self.writer.write(bytes)

    async def _receive_handler(self):
        while self.running:            
            response = await self.reader.read(4096)
            if response:
                #print(f"Incoming AACP Message: {response.hex()}")
                self.notify_subscribers(response)

    # This method executes only after reading. If the socket does not have anything to read, this method will get stuck forever.
    async def _send_handler(self):
        while self.running:
            command = await self.command_queue.get()
            #print(f"Sending AACP Message in queue: {command.hex()}")
            self.writer.write(command)
            await self.writer.drain()
            #print("Sent")


    async def connect(self):
        sock = socket.socket(socket.AF_BLUETOOTH, socket.SOCK_STREAM, socket.BTPROTO_L2CAP)
        sock.setblocking(False)
        await asyncio.get_event_loop().sock_connect(sock, (self.address, self.psm))
        self.reader, self.writer = await asyncio.open_connection(sock=sock)        

    async def start(self):
        print(f"Connecting to {self.address}")
        self.command_queue = asyncio.Queue()        
        await self.connect()
        
        receiver_task = asyncio.create_task(self._receive_handler())
        sender_task = asyncio.create_task(self._send_handler())
        print("Sending init and notification packets")
        #TODO: remove. Testing only
        self.WriteDirectly(b"\x00\x00\x04\x00\x01\x00\x02\x00\x00\x00\x00\x00\x00\x00\x00\x00")
        time.sleep(0.25) # Without a pause, the packet will not be sent        
        self.WriteDirectly(b"\x04\x00\x04\x00\x0f\x00\xff\xff\xff\xff")
        
        try:
            await asyncio.gather(receiver_task, sender_task)
        except ConnectionResetError:
            print("Connection lost")
        except Exception as e:
            self.writer.close()
            print(f"An error occurred in receive_handler: {e}")
        finally:
            self.running = False
            self.writer.close()
            await self.writer.wait_closed()

if __name__ == "__main__":
    address = "00:00:00:00:00:00"
    psm = 0x1001
    service = Service(address, psm)     
    asyncio.run(service.start())
