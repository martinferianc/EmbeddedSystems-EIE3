import micropython
import uctypes
from machine import I2C

#buffer to store error messages
micropython.alloc_emergency_exception_buf(100)

#register defines
ACCEL_X_H_REG = micropython.const(59)
ACCEL_X_L_REG = micropython.const(60)
ACCEL_Y_H_REG = micropython.const(61)
ACCEL_Y_L_REG = micropython.const(62)
ACCEL_Z_H_REG = micropython.const(63)
ACCEL_Z_L_REG = micropython.const(64)

GYRO_X_H_REG = micropython.const(67)
GYRO_X_L_REG = micropython.const(68)
GYRO_Y_H_REG = micropython.const(69)
GYRO_Y_L_REG = micropython.const(70)
GYRO_Z_H_REG = micropython.const(71)
GYRO_Z_L_REG = micropython.const(72)

class mpu:
    def __init__(self, freq=400000):
        self.i2c = I2C(freq=freq)
        self.slave = i2c.scan()

    def turn_off(self):
        self.i2c.deinit()

    def write_reg(self, reg_addr, data):
        if uctypes.size_of(data) > 1:
            raise ValueError('Only write 1 Byte of data to a register')
        else:
            self.i2c.writeto_mem(self.slave,reg_addr,data)
 
    def read_reg(self, reg_addr, size):
        self.i2c.readfrom_mem(self.slave,reg_addr,size)
