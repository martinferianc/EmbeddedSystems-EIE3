import math
average_factor = 0.962      # Factor used to decide how much of old angle is used
time_step =                 # Time step between program running
accel_gain =                # Multiplied by accel_raw to give reading in degrees
tau = (time_step*average_factor)/(1-average_factor) # Time constant
gyro_cal_x = 0
gyro_cal_y = 0
gyro_cal_z = 0

# Need to make sure both are in degrees before calculation commences!
# Depending on resoltuon, accel needs to divide by 16384 and gyro by 131.
# Accelerometer may need calibration

# Calibrate the gyros by taking 2000 samples and then creating an average offset
def calibrate():
    global gyro_cal_x
    global gyro_cal_y
    global gyro_cal_z

    for i in range(0:2000):
        gyro_cal_x = gyro_cal_x + gyro_x
        gyro_cal_y = gyro_cal_y + gyro_y
        gyro_cal_z = gyo_cal_z + gyro_z
    gyro_cal_x = gyro_cal_x/2000
    gyro_cal_y = gyro_cal_y/2000
    gyro_cal_z = gyro_cal_z/2000


def complement_filt(old_angle, accel, gyro):
    global average_factor
    global time_step
    global accel_gain
    #filtered_gyro = average_factor*filtered_gyro + (1-average_factor)*gyro
    #filtered_acc = average_factor*filtered_acc + (1-average_factor)*(accel*accel_gain)
    #filtered_angle = tau*filtered_gyro+filtered_acc
    filtered_angle = average_factor*(old_angle + gyro*time_step) + (1-average_factor)*accel
    return filtered_angle

rollangle = atan2(accel_y,accel_z)*180/pi
pitchangle = atan2(accelX, (accel_y*accel_y+accel_z*accel_z))*180/pi

roll = complement_filt(roll, rollangle, gyro_x)
pitch = complement_filt(pitch, pitchangle, gyro_y)
yaw = gyro_z
