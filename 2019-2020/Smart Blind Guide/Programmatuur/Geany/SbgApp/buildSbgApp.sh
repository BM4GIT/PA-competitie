#!/bin/bash
cd /home/leerling/GP/SbgApp
g++ -Wall -D RPI -I/usr/include/bm4 -c SbgApp.cpp -o SbgApp.o
g++ -Wall SbgApp.o -o /home/leerling/SbgApp -lbcm2835 -lbm4 -lmysqlclient -lpthread
