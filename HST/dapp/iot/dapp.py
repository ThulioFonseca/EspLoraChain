import os
import time
import pytz
import json
import serial
from datetime import datetime
from iot.client import IoTClient

DISTRIBUTION_NAME = 'iot'
DEFAULT_URL = 'http://rest-api-0:8008'
DEFAULT_SENSOR_PAYLOAD = '{\"deviceId\":\"2b3212ba-90fb-11ed-a1eb-0242ac120002\",\"sensorData\":{\"ph\":\"6.2\",\"temperature\":\"3.0\"},\"timestamp\":\"1673364651\"}'

def _get_keyfile(customerName):
    return '/project/dapp/iot/{}.priv'.format(customerName)

def _get_pubkeyfile(customerName):
    return '/project/dapp/iot/{}.pub'.format(customerName)

def store_sensor_data(data):
    key_file = _get_keyfile(DISTRIBUTION_NAME)
    iot_cli = IoTClient(baseUrl=DEFAULT_URL, keyFile=key_file)
    response = iot_cli.store_sensor_data(str(data))
    print("Response: {}".format(response))

def sensor_simulation(nloops):
    for i in range(int(nloops)):
        store_sensor_data(DEFAULT_SENSOR_PAYLOAD)

def sensor_read_serial():
    ser = serial.Serial('/dev/ttyUSB0')
        # Reads one byte of information
    while True:
        myBytes = ser.read(122)
        # bufferBytes = ser.inWaiting()
        # if bufferBytes:
        #     myBytes = myBytes + ser.read(bufferBytes)
        print(myBytes.decode("utf-8"))
        print()

def get_sensor_data():
    key_file = _get_keyfile(DISTRIBUTION_NAME)
    iot_cli = IoTClient(baseUrl=DEFAULT_URL, keyFile=key_file)
    data = iot_cli.get_sensor_data()
    if data is not None:
        print("\n{} have a state = {}\n".format(DISTRIBUTION_NAME, data.decode()))
    else:
        raise Exception("state not found: {}".format(DISTRIBUTION_NAME))

# def get_sensor_history():
#     key_file = _get_keyfile(DISTRIBUTION_NAME)
#     iot_cli = IoTClient(baseUrl=DEFAULT_URL, keyFile=key_file)
#     data = iot_cli.get_sensor_history()

#     if data is not None:
#         for i in data:
#             try:
#                 print(i.decode())
#             except:
#                 pass
#     else:
#         raise Exception("history not found: {}".format(client))

def get_sensor_history():
    key_file = _get_keyfile(DISTRIBUTION_NAME)
    iot_cli = IoTClient(baseUrl=DEFAULT_URL, keyFile=key_file)
    data = iot_cli.get_sensor_history()

    if data is not None:
        for i in data:
            try:
                messageData = i.decode().split("store_sensor_data,")
                sensorDataObj = json.loads(messageData[1])
                tz = pytz.timezone('America/Sao_Paulo')
                sensorDateTime = datetime.fromtimestamp(int(sensorDataObj["timestamp"]), tz)

                print('')
                print("Id do Dispositivo: " + sensorDataObj["deviceId"])
                print("Dados dos Sensores: ph = " + sensorDataObj["sensorData"]["ph"] + " / Temperatura = " + sensorDataObj["sensorData"]["temperature"])
                print("Data: ", sensorDateTime)
                print('')

            except:
                pass
    else:
        raise Exception("history not found: {}".format(client))


def main():
    op = "-1"
    while op != "6":
        print("\n1 - store sensor data\n2 - get sensor data\n3 - get sensor history\n4 - Simulate sensor loops\n5 - Sensor Read(Serial)\n6 - Exit")
        op = input("Operation: ")
        if op == "1":
            data = input("Data to insert: ")
            store_sensor_data(data)
        elif op == "2":
            get_sensor_data()
        elif op == "3":
            get_sensor_history()
        elif op == "4":
            loops = input("Number of sensor loops: ")
            sensor_simulation(loops)        
        elif op == "5":
            sensor_read_serial()