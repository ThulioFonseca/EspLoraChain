FROM ubuntu:bionic

RUN apt-get update

COPY keys/validator* /shared_data/

RUN ls -al /shared_data

RUN chmod -R 755 /shared_data/
