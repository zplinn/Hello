#!/bin/env python3

import os
import hashlib

flag = input('Flag: ')
if hashlib.sha256(flag.strip().encode()).hexdigest() == '68d98b79e6289315b9546cea54addf9c5f6cbade7cd63ea6780ea4e8fdc4934e':
    print('Correct! Congratulations!')
    os.system('/bin/sh')
else:
    print('Wrong!')
