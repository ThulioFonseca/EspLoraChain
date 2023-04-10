import os
import time
from iot.iot_client import IoTClient

DISTRIBUTION_NAME = 'iot'
#DEFAULT_URL = 'http://rest-api-rasp-2:8008'
DEFAULT_URL = 'http://rest-api-0:8008'

def _get_keyfile(customerName):
    return '/project/iot/pyclient/iot/{}.priv'.format(customerName)

def _get_pubkeyfile(customerName):
    return '/project/iot/pyclient/iot/{}.pub'.format(customerName)

def store_sensor_data(quantidade):
    key_file = _get_keyfile("iot")
    iot_cli = IoTClient(baseUrl=DEFAULT_URL, keyFile=key_file)
    iot_cli.store_sensor_data(str(quantidade))

def main():
    rodadas = 5
    while rodadas > 0:
        quantidade = 1000
        print("Rodada iniciada: ", rodadas)
        while quantidade > 0:
            store_sensor_data(quantidade)
            quantidade = quantidade - 1
            time.sleep(1)
        print("Rodada finalizada: ", rodadas)
        rodadas = rodadas - 1
        time.sleep(120)
