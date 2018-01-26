import client
# Setup the Device Address
deviceAddr = 0x68 

# Initialise the client and the buffer for the registers
client = Client(deviceAddr, playerNum)
valueBuf = bytearray(14);

# 
def fetchAccelValues():
	accelHandle.writeto(deviceAddr, 0x3B)
	valueBuf = accelHandle.readfrom(deviceAddr, 14)
	micropython.schedule(client.updateAccelValues(valueBuf))
	
# Declare and setup the interupt pin from the MPU when a new set of values are ready.
p0 = Pin(0, Pin.IN)
p0.irq(trigger=Pin.IRQ_RISING, handler=fetchAccelValues())

