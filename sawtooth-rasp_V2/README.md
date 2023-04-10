# sawtooth-iot
A simple sawtooth "iot" transaction family (processor + client)

# Introduction

This is a minimal example of a sawtooth 1.0 application. This example demonstrates, a common use-case, where a customer can:
1. store sensor data
2. get sensor data
3. get sensor data history

The customer is identified by a customer name and a corresponding public key. The sensor state is stored at an address, derived from SHA 512 hash of customer's public key and the IoT transaction family namespace.

# Components
The client application is written in Python, written in two parts: _client.py file representing the backend stuff and the _app.py representing the frontend stuff. The example is built by using the setup.py file located in one directory level up. The Transaction Processor (smart contract | chaincode) is written in Python using python-sawtooth-sdk in _tp.py file.

# Usage

Start the pre-built Docker containers in docker-compose.yaml file, located in sawtooth-iot directory:
```bash
cd sawtooth-iot
docker-compose up
```
At this point all the containers should be running.

To launch the client, you could do this:
```bash
docker exec -it iot-client bash
```

You can locate the right Docker client container name using `docker ps`.

Sample command usage:

```bash
#Create a iot
sawtooth keygen iot #This creates the public/private keys for iot, a pre-requisite for the following commands

iot_cli # To execute app

# Client App Menu
1 - store sensor data
2 - get sensor data
3 - get sensor history
4 - exit
```