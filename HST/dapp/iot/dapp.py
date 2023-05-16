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
    loop = 0
    ser = serial.Serial('/dev/ttyUSB0')
    ser.flushInput()
        # Reads one byte of information
    while loop < 5:
        myBytes = ''
        time.sleep(2)
        bufferBytes = ser.inWaiting()
        if bufferBytes > 85:
             myBytes = ser.read(bufferBytes)
             ser.flushInput()
             store_sensor_data(myBytes.decode("utf-8"))
             loop += 1

def get_sensor_data():
    key_file = _get_keyfile(DISTRIBUTION_NAME)
    iot_cli = IoTClient(baseUrl=DEFAULT_URL, keyFile=key_file)
    data = iot_cli.get_sensor_data()
    if data is not None:
        print("\n{} have a state = {}\n".format(DISTRIBUTION_NAME, data.decode()))
    else:
        raise Exception("state not found: {}".format(DISTRIBUTION_NAME))

def get_sensor_history():
    key_file = _get_keyfile(DISTRIBUTION_NAME)
    iot_cli = IoTClient(baseUrl=DEFAULT_URL, keyFile=key_file)
    data = iot_cli.get_sensor_history()

    if data is not None:
        for i in data:
            try:
                messageData = i.decode().split("store_sensor_data,")
                sensorDataObj = json.loads(messageData[1])
                print('')
                data = "Id do Dispositivo: {deviceId} \nDados dos Sensores: \nph = {ph}\nTemperatura =  {temperatura}\nData: {timestamp}".format(**sensorDataObj)
                print(data)
                print('')

            except Exception as e :
                print(e)
    else:
        raise Exception("history not found: {}".format(IoTClient))


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