#!/bin/bash

env -i git reset --hard HEAD
env -i git pull origin master

chmod +x deploy.sh
chmod +x explore_block_size.py
chmod +x explore_rectangle_size.py
chmod +x measure.py

make
make base