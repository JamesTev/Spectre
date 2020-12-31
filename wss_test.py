import asyncio
import datetime 
import random
import websockets
import os

async def time(websocket, path):
    while True:
        now = datetime.datetime.utcnow().isoformat() + "Z"
        x = bytearray(os.urandom(128))
        await websocket.send(x)
        await asyncio.sleep(2)

start_server = websockets.serve(time, "127.0.0.1", 8765)

asyncio.get_event_loop().run_until_complete(start_server)
asyncio.get_event_loop().run_forever()
