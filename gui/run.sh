#!/bin/bash
cd ..
make -f Makefile all
cd gui
python3 main.py
