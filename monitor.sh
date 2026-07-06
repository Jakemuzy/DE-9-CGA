#!/bin/bash

# Used for easily setting up the qemu emulator for graphics output
idf.py qemu | python3 image_test.py
