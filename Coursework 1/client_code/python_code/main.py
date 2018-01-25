import client

deviceAddr = 0x68 

client = Client(deviceAddr, playerNum)
valueBuf = bytearray(14);

def fetchAccelValues():
	accelHandle.writeto(deviceAddr, 0x3B)
	valueBuf = accelHandle.readfrom(deviceAddr, 14)
	micropython.schedule(client.updateAccelValues(valueBuf)
	
p0 = Pin(0, Pin.IN)

p0.irq(trigger=Pin.IRQ_RISING, handler=fetchAccelValues())

