from machine import Pin,I2C

dev_addr = 0x39
power_up = 0x03
read_0 = 0xAC
read_1 = 0xAE

i2cport = I2C(scl=Pin(5), sda=Pin(4), freq = 100000)

def readValues(i2cport):
	i2cport.writeto(dev_addr, bytearray([read_0]))
	data = i2cport.readfrom(dev_addr,2)
	chan_0 = int.from_bytes(data, 'big')
	i2cport.writeto(dev_addr, bytearray([read_1]))
	data = i2cport.readfrom(dev_addr,2)
	chan_1 = int.from_bytes(data, 'big')
	return chan_0, chan_1

i2cport.writeto(dev_addr, bytearray([power_up]))

chan_0, chan_1 = readValues(i2cport)
print(chan_0)
