# MQTT to InfluxDB Bridge

## Build

```sh
$ docker build -t amirmohseninia/mqttbridge .
```


## Run

```sh
$ docker run -d --name mqttbridge amirmohseninia/mqttbridge
```


## Dev

```sh
$ docker run -it --rm -v `pwd`:/app --name python python:3.7-alpine sh
```
