import client, mpu
# Setup the Device Address
deviceAddress = 0x68

# Initialise the client and the buffer for the registers
client_board = Client(playerNum, deviceAddress)

# Declare and setup the interupt pin from the MPU when a new set of values are ready.
p0 = Pin(0, Pin.IN)
p0.irq(trigger=Pin.IRQ_RISING, handler=client_board.read_sensor_reg)
