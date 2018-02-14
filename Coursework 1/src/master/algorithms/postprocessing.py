import math, json, ast, re, time

class PostProcessing():
##################### Member Variables ##########################

    def __init__(self):
        # Gyro calibration values
        self.gyro_cal_x = 0
        self.gyro_cal_y = 0
        self.gyro_cal_z = 0

        self.yaw = 0
        self.roll = 0
        self.pitch = 0

        self.average_number = 1000

        # Factor used to decide how much of old angle is used
        self._average_factor = 0.962
        # Time step between program running
        self._time_step = 0.125
        # Time constant
        self.tau = (self._time_step*self._average_factor)/(1-self._average_factor)

        # Convert radians to degrees
        self._rad2deg = 180/math.pi


##################### Member Functions ##########################

    # Normalise the acceleration values
    def _acc_normal(self, value, offset = 0):
        # Make negative if req.
        if(value > 2**15):
            value = value - 2**16
        # Return normal
        return (value-offset)/2048

    def _gyro_normal(self, value, offset = 0):
        # Make neg if req.
        if(value > 2**15):
            value = value - 2**16
        # Return normal
        return (value-offset)/16.4

    # Controls how much the gyro and accelerometer contribute to the change in value
    def _complement_filt(self, old_angle, accel, gyro):
        filtered_angle = self._average_factor*(old_angle + gyro*self._time_step) + (1-self._average_factor)*accel
        return filtered_angle

    # Calculate the yaw pitch and roll values from the accelerometer readings
    def _yaw_pitch_roll(self, feature_vector):
        rollangle = math.atan2(feature_vector['ACY'],feature_vector['ACZ'])*self._rad2deg
        pitchangle = math.atan2(feature_vector['ACX'], (feature_vector['ACY']**2+feature_vector['ACZ']**2))*self._rad2deg

        self.roll = self._complement_filt(self.roll, rollangle, feature_vector['GYX'])
        self.pitch = self._complement_filt(self.pitch, pitchangle, feature_vector['GYY'])
        self.yaw = feature_vector['GYZ']
        return {'YAW': yaw, 'PITCH': pitch, 'ROLL': roll}

##################### Public Functions ##########################

    # Saves the calibration variables into a json file
    def save_gyro_calibration(self, file_path = "calibration/calibration_values.txt"):
        offset_values = {"GYOFX": self.gyro_cal_x, "GYOFY": self.gyro_cal_y, "GYOFZ": self.gyro_cal_z}

        f = open(file_path, 'w')
        f.write(str(offset_values))
        f.close()

    # Loads the json file with the calibrated values
    def load_gyro_calibration(self, file_path = "calibration/calibration_values.txt"):
        f = open(file_path, 'r')
        offset_values = ast.literal_eval(f.read())
        self.gyro_cal_x = offset_values['GYOFX']
        self.gyro_cal_y = offset_values['GYOFY']
        self.gyro_cal_y = offset_values['GYOFZ']
        f.close()

    # Calibrate the gyros by taking 2000 samples and then creating an average offset
    def refresh_calibration_values(self, data):
        tmp_gyro_cal_x = 0
        tmp_gyro_cal_y = 0
        tmp_gyro_cal_z = 0
        for i in range(0, self.average_number):
            # Average first 1000 values to find gyro offset
            tmp_gyro_cal_x = tmp_gyro_cal_x + data[i]['GYX']
            tmp_gyro_cal_y = tmp_gyro_cal_y + data[i]['GYY']
            tmp_gyro_cal_z = tmp_gyro_cal_z + data[i]['GYZ']
        self.gyro_cal_x = tmp_gyro_cal_x/self.average_number
        self.gyro_cal_y = tmp_gyro_cal_y/self.average_number
        self.gyro_cal_z = tmp_gyro_cal_z/self.average_number

    def magnitude(self,x,y,z):
        return math.sqrt(x**2+y**2+z**2)

    # Converts the values into actual meaningful units
    def postprocess_data(self, feature_vector, yaw_pitch_roll_values = False):
        # Get only the relevant data from the feature vector
        if(self.gyro_cal_x == 0 and self.gyro_cal_y == 0 and self.gyro_cal_z == 0):
            raise Exception("Gyro Calibration Values are all 0.")
        feature_vector = feature_vector["DATA"]
        processed_vector = []

        accelMag = self.magnitude(feature_vector['ACX'],feature_vector['ACY'],feature_vector['ACZ'])
        gyroMag  = self.magnitude(feature_vector['GYX'],feature_vector['GYY'],feature_vector['GYZ'])

        '''
        processed_vector.append(self._acc_normal(feature_vector['ACX']))
        processed_vector.append(self._acc_normal(feature_vector['ACY']))
        processed_vector.append(self._acc_normal(feature_vector['ACZ']))
        processed_vector.append(self._gyro_normal(feature_vector['GYX'], self.gyro_cal_x))
        processed_vector.append(self._gyro_normal(feature_vector['GYY'], self.gyro_cal_y))
        processed_vector.append(self._gyro_normal(feature_vector['GYZ'], self.gyro_cal_z)) 
        '''  

        processed_vector.append(accelMag)
        processed_vector.append(gyroMag)

        return processed_vector

    # Process the whole JSON fie
    def postprocess_file(self, filename, new_gyro_cal = False, old_gyro_cal = False, save_gyro_cal = False, save_to_file = False):
        raw_data = []
        processed_data = []
        processed_data_str = ""
        times = []
        # Load in all the JSON from the file
        with open(filename, 'r') as f:
            for line in f:
                tmp_data = json.loads(line)
                sensor_data = tmp_data['DATA']
                raw_data.append(sensor_data)
                times.append(tmp_data['TIMESTAMP'])
        if(old_gyro_cal):
            # Iterate through the array and normalise all of the values
            for i in range(0, len(raw_data)):
                
                raw_data[i]['ACX'] = self._acc_normal(raw_data[i]['ACX'])
                raw_data[i]['ACY'] = self._acc_normal(raw_data[i]['ACY'])
                raw_data[i]['ACZ'] = self._acc_normal(raw_data[i]['ACZ'])
                raw_data[i]['GYX'] = self._gyro_normal(raw_data[i]['GYX'], self.gyro_cal_x)
                raw_data[i]['GYY'] = self._gyro_normal(raw_data[i]['GYY'], self.gyro_cal_y)
                raw_data[i]['GYZ'] = self._gyro_normal(raw_data[i]['GYZ'], self.gyro_cal_z)
                
                accelMag = self.magnitude(raw_data[i]['ACX'],raw_data[i]['ACY'],raw_data[i]['ACZ'])
                gyroMag  = self.magnitude(raw_data[i]['GYX'],raw_data[i]['GYY'],raw_data[i]['GYZ']) 

                raw_data[i]['ACMAG'] = accelMag
                raw_data[i]['GYMAG'] = gyroMag

        else:
            # Iterate through the array and normalise all of the values
            for i in range(0, len(raw_data)):
                raw_data[i]['ACX'] = self._acc_normal(raw_data[i]['ACX'])
                raw_data[i]['ACY'] = self._acc_normal(raw_data[i]['ACY'])
                raw_data[i]['ACZ'] = self._acc_normal(raw_data[i]['ACZ'])
                raw_data[i]['GYX'] = self._gyro_normal(raw_data[i]['GYX'])
                raw_data[i]['GYY'] = self._gyro_normal(raw_data[i]['GYY'])
                raw_data[i]['GYZ'] = self._gyro_normal(raw_data[i]['GYZ'])

                accelMag = self.magnitude(raw_data[i]['ACX'],raw_data[i]['ACY'],raw_data[i]['ACZ'])
                gyroMag  = self.magnitude(raw_data[i]['GYX'],raw_data[i]['GYY'],raw_data[i]['GYZ']) 

                raw_data[i]['ACMAG'] = accelMag
                raw_data[i]['GYMAG'] = gyroMag

        if(new_gyro_cal):
            # Take an average of a set number of samples to get the gyro offset
            self.refresh_calibration_values(raw_data)
            # Pass back over the array and remove the offset from all the samples
            for i in range(0, len(raw_data)):
                raw_data[i]['GYX'] = self._gyro_normal(raw_data[i]['GYX'], self.gyro_cal_x)
                raw_data[i]['GYY'] = self._gyro_normal(raw_data[i]['GYY'], self.gyro_cal_y)
                raw_data[i]['GYZ'] = self._gyro_normal(raw_data[i]['GYZ'], self.gyro_cal_z)
        # Put the data in the correct form to be outputted
        for i in range(0, len(raw_data)):
            feature_vector = {"ACX": raw_data[i]['ACX'], "ACY":raw_data[i]['ACY'], "ACZ":raw_data[i]['ACZ'],"GYX": raw_data[i]['GYX'],"GYY":raw_data[i]['GYY'], "GYZ": raw_data[i]['GYZ'], "ACMAG":raw_data[i]['ACMAG'], "GYMAG":raw_data[i]['GYMAG']}
            processed_data_str+=str(feature_vector)+"\n"
            processed_data_str = processed_data_str.replace("\'", "\"")
            processed_data.append(feature_vector)
        # Save calibration values and return
        if(save_gyro_cal):
            self.save_gyro_calibration()

        if(save_to_file):
            path = filename.strip().split("/")
            path[-1] = "data_processed.txt"
            new_path = ""
            for element in path:
                new_path+=element+"/"
            f = open(new_path[:-1], "w")
            f.write(processed_data_str)
            f.close()
        return processed_data, times

def encapsulate_data(data):
    new_data = {}
    new_data["PLAYER"] = 1
    # Package the data
    new_data['TIMESTAMP'] = time.ctime()
    tmp = data['DATA']
    tmp = {'ACX':tmp[0], 'ACY':tmp[1], 'ACZ':tmp[2], 'GYX':tmp[4], 'GYY':tmp[5], 'GYZ':tmp[6]}
    new_data['DATA'] = tmp
    return new_data

if __name__ == '__main__':
    postprocessing = PostProcessing()
    postprocessing.postprocess_file('../../../data/data_raw.txt', new_gyro_cal = True, save_gyro_cal = True, save_to_file = True)
    postprocessing.load_gyro_calibration()
