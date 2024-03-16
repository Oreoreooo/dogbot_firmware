import numpy as np
import csv
import time
import serial

def save_csv_file(file_path):
    arduino_port = "COM4"
    arduino_baudrate = 115200
    with serial.Serial(arduino_port, arduino_baudrate) as arduino_serial:
        with open(file_path,mode="w",newline="") as file:
            csv_writer = csv.writer(file)
            start_time = time.time()
            while time.time() - start_time < 35:
                data = arduino_serial.readline().decode("utf-8").strip()
                try:
                    print(data)
                    csv_writer.writerow(map(int,data.split(",")))
                except ValueError as e:
                    print(f"Invalid data format: {data}; {e}")
                    
def read_csv_file(file_path):
    data = [[], [], [], []]
    with open(file_path, "r") as csvfile:
        reader = csv.reader(csvfile)
        for k, row in enumerate(reader):
            if k == 0:
                continue
            microsecond = float(row[0])
            for i in range(4):
                x = float(row[i + 1])
                data[i].append(np.true_divide(x, microsecond))
    return data


csv_file_path = "data.csv"
# save_csv_file(csv_file_path)
data = read_csv_file(csv_file_path)

for i in range(len(data)):
    print(f"RPC_{i}: {np.average(data[i])}")
