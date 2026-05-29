#!/bin/bash
cd /workspaces/Smart-EV-Charging-Network-Operations
g++ -std=c++98 -Wall -pedantic src/*.cpp -o evmanager
./evmanager

