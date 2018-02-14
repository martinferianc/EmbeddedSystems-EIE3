import client, micropython, time
from machine import Pin, I2C

#buffer to store error messages
micropython.alloc_emergency_exception_buf(100)

# Setup the Device Address
deviceAddress = 0x68
playerNum = 1

# Initialise the client and the buffer for the registers
client_board = client.Client(playerNum, deviceAddress)
'''
# Declare and setup the interupt pin from the MPU when a new set of values are ready.
p13 = Pin(13, Pin.IN)
p13.irq(trigger=Pin.IRQ_RISING, handler=client_board.read_sensor_reg)

while True:
    print (client_board.getJsonValues())
    time.sleep(3)
'''
