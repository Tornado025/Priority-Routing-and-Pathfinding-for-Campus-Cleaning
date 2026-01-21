#!/bin/bash
cd "$(dirname "$0")/.."
make -f Makefile.shared all
cd gui
python3 main.py
