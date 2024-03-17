import numpy as np
import csv
import time
import serial

scale_factor = 1000

def save_csv_file(file_path):
    arduino_port = "/dev/ttyUSB0"
    arduino_baudrate = 115200
    with serial.Serial(arduino_port, arduino_baudrate) as arduino_serial:
        with open(file_path,mode="w",newline="") as file:
            csv_writer = csv.writer(file)
            start_time = time.time()
            while time.time() - start_time < 60:
                data = arduino_serial.readline().decode("utf-8").strip()
                try:
                    print(data)
                    csv_writer.writerow(map(int,data.split(",")))
                except ValueError as e:
                    print(f"Invalid data format: {data}; {e}")
                    
def read_csv_file(file_path):
    rps_data = [[], [], [], []]
    rpp_data = [[], [], [], []]
    avg_revolution = [[], [], [], []]
    time_interval = []
    try:
        with open(file_path, "r") as csvfile:
            reader = csv.reader(csvfile)
            for k, row in enumerate(reader):
                if k < 1000:
                    continue
                microsecond = float(row[0])
                time_interval.append(microsecond)
                for i in range(4):
                    x = abs(float(row[i + 1]))
                    rps_data[i].append(np.true_divide(x, microsecond))
                    rpp_data[i].append(np.true_divide(x, 255))
                    avg_revolution[i].append(x)
    finally:
        return rps_data, rpp_data, avg_revolution, time_interval


csv_file_path = "data.csv"
# save_csv_file(csv_file_path)
rps_data,rpp_data,avg_revolution, time_interval = read_csv_file(csv_file_path)

print(f"interval: {np.average(time_interval)/1000:.6f}\n")
for i in range(len(rps_data)):
    print(f"rps{i}: {np.average(rps_data[i])*1000:.6f}")
print()
for i in range(len(rpp_data)):
    print(f"rpp{i}: {np.average(rpp_data[i]):.6f}")
print()
for i in range(len(rpp_data)):
    print(f"avr{i}: {np.average(avg_revolution[i]):.6f}")