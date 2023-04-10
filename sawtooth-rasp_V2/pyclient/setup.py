import os
import subprocess

from setuptools import setup, find_packages

data_files = []

setup(
    name='iot-cli',
    version='1.0',
    description='Sawtooth IoT Example',
    author='Garrocho',
    url='https://github.com/garrocho/sawtooth-iot',
    packages=find_packages(),
    install_requires=[
        'aiohttp',
        'colorlog',
        'protobuf',
        'sawtooth-sdk',
        'sawtooth-signing',
        'PyYAML',
    ],
    data_files=data_files,
    entry_points={
        'console_scripts': [
            'iot = iot_app:main',
        ]
    })