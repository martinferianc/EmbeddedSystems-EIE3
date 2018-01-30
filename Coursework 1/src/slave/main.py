import mpu
import ring_buf
import micropython

#buffer to store error messages
micropython.alloc_emergency_exception_buf(100)

'''
TODO:
 - interrupt handler for MPU I2C pin
 - interrupt handler for WIFI requests
 - shared buffer
 - make sure to disable interrupts when accessing buffer (pyb.disable_irq())
'''
#constant defines
BUF_SIZE = micropython.const(100)

#Initialisations
mpu = mpu()
#add wifi

fifo_accel_x  = ring_buf(BUF_SIZE)
fifo_accel_y  = ring_buf(BUF_SIZE)
fifo_accel_z  = ring_buf(BUF_SIZE)

fifo_gyro_x   = ring_buf(BUF_SIZE)
fifo_gyro_y   = ring_buf(BUF_SIZE)
fifo_gyro_z   = ring_buf(BUF_SIZE)

#global function defines
def update_buffer():
  mpu.get_accel()
  fifo_accel_x.put(mpu.accel_buf[0]<<8|mpu.accel_buf[1])  
  fifo_accel_y.put(mpu.accel_buf[2]<<8|mpu.accel_buf[3])  
  fifo_accel_z.put(mpu.accel_buf[4]<<8|mpu.accel_buf[5])  

  mpu.get_gyro()
  fifo_gyro_x.put(mpu.gyro_buf[0]<<8|mpu.gyro_buf[1])  
  fifo_gyro_y.put(mpu.gyro_buf[2]<<8|mpu.gyro_buf[3])  
  fifo_gyro_z.put(mpu.gyro_buf[4]<<8|mpu.gyro_buf[5])  
  
  return

#Main loop
while True:
  pass

