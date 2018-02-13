import math, json
# Factor used to decide how much of old angle is used
average_factor = 0.962
# Time step between program running
time_step = 0.125
# Time constant
tau = (time_step*average_factor)/(1-average_factor)
average_number = 1000
# Gyro calibration values
gyro_cal_x = 0
gyro_cal_y = 0
gyro_cal_z = 0

# Convert radians to degrees
rad2deg = 180/math.pi

# Normalise the acceleration values
def acc_normal(value, offset = 0):
    # Make negative if req.
    if(value > 2**15){
        value = value - 2**16
    }
    # Return normal
    return (value-offset)/2048

def gyro_normal(value, offset = 0):
    # Make neg if req.
    if(value > 2**15){
        value = value - 2**16
    }
    # Return normal
    return (value-offset)/16.4

# Saves the calibration variables into a json file
def save_calibration(file_path):
    global gyro_cal_x
    global gyro_cal_y
    global gyro_cal_z
    offset_values = {"GYOFX": gyro_cal_x, "GYOFY": gyro_cal_y, "GYOFZ": gyro_cal_z}
    file = open(file_path, 'w')
    file.write(offset_values)
    file.close()

# Loads the json file with the calibrated values
def load_calibration(file_path):
    global gyro_cal_x
    global gyro_cal_y
    global gyro_cal_z
    file = open(file_path, 'r')
    offset_values = file.readline()
    gyro_cal_x = offset_values['GYX']
    gyro_cal_y = offset_values['GYY']
    gyro_cal_y = offset_values['GYZ']

# Calibrate the gyros by taking 2000 samples and then creating an average offset
def calibrate(data):
    global gyro_cal_x
    global gyro_cal_y
    global gyro_cal_z
    global average_number

    # Average first 1000 values to find gyro offset
    for i in range(0,average_number):
        gyro_cal_x = gyro_cal_x + data[i]['GYX']
        gyro_cal_y = gyro_cal_y + data[i]['GYY']
        gyro_cal_z = gyro_cal_z + data[i]['GYZ']
    gyro_cal_x = gyro_cal_x/average_number
    gyro_cal_y = gyro_cal_y/average_number
    gyro_cal_z = gyro_cal_z/average_number

# Converts the values into actual meaningful units
def postprocess_data(feature_vector):
    # Get only the relevant data from the feature vector
    feature_vector = feature_vector["DATA"]
    feature_vector['ACX'] = acc_normal(feature_vector['ACX'])
    feature_vector['ACY'] = acc_normal(feature_vector['ACY'])
    feature_vector['ACZ'] = acc_normal(feature_vector['ACZ'])
    feature_vector['GYX'] = gyro_normal(feature_vector['GYX'])
    feature_vector['GYY'] = gyro_normal(feature_vector['GYY'])
    feature_vector['GYZ'] = gyro_normal(feature_vector['GYZ'])

    return feature_vector

# Process the whole JSON fie
def postprocess_file(file):
    raw_data = []
    processed_data = []
    times = []
    # Load in all the JSON from the file
    with open(file, 'r') as f:
        for line in f:
            tmp_data = json.loads(line)
            sensor_data = tmp_data['DATA']
            data.append(sensor_data)
            times.append(tmp_data['TIMESTAMP'])
    # Iterate through the array and normalise all of the values
    for i in range(0, len(raw_data)):
        raw_data[i]['ACX'] = acc_normal(raw_data[i]['ACX'])
        raw_data[i]['ACY'] = acc_normal(raw_data[i]['ACY'])
        raw_data[i]['ACZ'] = acc_normal(raw_data[i]['ACZ'])
        raw_data[i]['GYX'] = gyro_normal(raw_data[i]['GYX'])
        raw_data[i]['GYY'] = gyro_normal(raw_data[i]['GYY'])
        raw_data[i]['GYZ'] = gyro_normal(raw_data[i]['GYZ'])
    # Take an average of a set number of samples to get the gyro offset
    calibrate(raw_data)
    # Pass back over the array and remove the offset from all the samples
    for i in range(0, len(raw_data)):
        raw_data[i]['GYX'] = gyro_normal(raw_data[i]['GYX'], gyro_cal_x)
        raw_data[i]['GYY'] = gyro_normal(raw_data[i]['GYY'], gyro_cal_y)
        raw_data[i]['GYZ'] = gyro_normal(raw_data[i]['GYZ'], gyro_cal_z)
    # Put the data in the correct form to be outputted
    for i in range(0, len(raw_data)):
        processed_data.append([raw_data[i]['ACX'],raw_data[i]['ACY'],raw_data[i]['ACZ'],raw_data[i]['GYX'],raw_data[i]['GYY'],raw_data[i]['GYZ']])
    # Save calibration values and return
    save_calibration()
    return processed_data, times
