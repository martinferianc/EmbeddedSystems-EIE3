import math, json

class PostProcessing(self):
##################### Member Variables ##########################

    # Gyro calibration values
    self.gyro_cal_x = 0
    self.gyro_cal_y = 0
    self.gyro_cal_z = 0

    self.yaw = 0;
    self.roll = 0;
    self.pitch = 0;

    self.average_number = 1000

    # Factor used to decide how much of old angle is used
    self._average_factor = 0.962
    # Time step between program running
    self._time_step = 0.125
    # Time constant
    self.tau = (time_step*average_factor)/(1-average_factor)

    # Convert radians to degrees
    self._rad2deg = 180/math.pi


##################### Member Functions ##########################

    # Normalise the acceleration values
    def _acc_normal(self, value, offset = 0):
        # Make negative if req.
        if(value > 2**15){
            value = value - 2**16
        }
        # Return normal
        return (value-offset)/2048

    def _gyro_normal(self, value, offset = 0):
        # Make neg if req.
        if(value > 2**15){
            value = value - 2**16
        }
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
    def save_gyro_calibration(self, file_path = '\..\calibration\calibration_values.txt'):
        offset_values = {"GYOFX": self.gyro_cal_x, "GYOFY": self.gyro_cal_y, "GYOFZ": self.gyro_cal_z}
        file = open(file_path, 'w')
        file.write(offset_values)
        file.close()

    # Loads the json file with the calibrated values
    def load_gyro_calibration(self, file_path = '\..\calibration\calibration_values.txt' ):
        file = open(file_path, 'r')
        offset_values = file.readline()
        self.gyro_cal_x = offset_values['GYX']
        self.gyro_cal_y = offset_values['GYY']
        self.gyro_cal_y = offset_values['GYZ']
        file.close()

    # Calibrate the gyros by taking 2000 samples and then creating an average offset
    def refresh_calibration_values(self, data):
        for i in range(0, self.average_number):
            # Average first 1000 values to find gyro offset
            tmp_gyro_cal_x = tmp_gyro_cal_x + data[i]['GYX']
            tmp_gyro_cal_y = tmp_gyro_cal_y + data[i]['GYY']
            tmp_gyro_cal_z = tmp_gyro_cal_z + data[i]['GYZ']
        self.gyro_cal_x = tmp_gyro_cal_x/average_number
        self.gyro_cal_y = tmp_gyro_cal_y/average_number
        self.gyro_cal_z = tmp_gyro_cal_z/average_number

    # Converts the values into actual meaningful units
    def postprocess_data(self, feature_vector, yaw_pitch_roll_values = False):
        # Get only the relevant data from the feature vector
        feature_vector = feature_vector["DATA"]
        feature_vector['ACX'] = self._acc_normal(feature_vector['ACX'])
        feature_vector['ACY'] = self._acc_normal(feature_vector['ACY'])
        feature_vector['ACZ'] = self._acc_normal(feature_vector['ACZ'])
        feature_vector['GYX'] = self._gyro_normal(feature_vector['GYX'], self.gyro_cal_x)
        feature_vector['GYY'] = self._gyro_normal(feature_vector['GYY'], self.gyro_cal_y)
        feature_vector['GYZ'] = self._gyro_normal(feature_vector['GYZ'], self.gyro_cal_z)
        return feature_vector

    # Process the whole JSON fie
    def postprocess_file(self, filename, new_gyro_cal = False, old_gyro_cal = False, save_gyro_cal = False):
        raw_data = []
        processed_data = []
        times = []
        # Load in all the JSON from the file
        with open(filename, 'r') as f:
            for line in f:
                tmp_data = json.loads(line)
                sensor_data = tmp_data['DATA']
                data.append(sensor_data)
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
        else:
            # Iterate through the array and normalise all of the values
            for i in range(0, len(raw_data)):
                raw_data[i]['ACX'] = self._acc_normal(raw_data[i]['ACX'])
                raw_data[i]['ACY'] = self._acc_normal(raw_data[i]['ACY'])
                raw_data[i]['ACZ'] = self._acc_normal(raw_data[i]['ACZ'])
                raw_data[i]['GYX'] = self._gyro_normal(raw_data[i]['GYX'])
                raw_data[i]['GYY'] = self._gyro_normal(raw_data[i]['GYY'])
                raw_data[i]['GYZ'] = self._gyro_normal(raw_data[i]['GYZ'])
        if(new_gyro_cal):
            # Take an average of a set number of samples to get the gyro offset
            calibrate(raw_data)
            # Pass back over the array and remove the offset from all the samples
            for i in range(0, len(raw_data)):
                raw_data[i]['GYX'] = self._gyro_normal(raw_data[i]['GYX'], self.gyro_cal_x)
                raw_data[i]['GYY'] = self._gyro_normal(raw_data[i]['GYY'], self.gyro_cal_y)
                raw_data[i]['GYZ'] = self._gyro_normal(raw_data[i]['GYZ'], self.gyro_cal_z)
        # Put the data in the correct form to be outputted
        for i in range(0, len(raw_data)):
            processed_data.append([raw_data[i]['ACX'],raw_data[i]['ACY'],raw_data[i]['ACZ'],raw_data[i]['GYX'],raw_data[i]['GYY'],raw_data[i]['GYZ']])
        # Save calibration values and return
        if(save_gyro_cal):
            save_calibration()
        return processed_data, times
