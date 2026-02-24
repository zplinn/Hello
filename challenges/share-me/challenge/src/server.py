#!/bin/env python3

import os
import hashlib

flag = input('Flag: ')
if hashlib.sha256(flag.strip().encode()).hexdigest() == '5559a71f9b65cdc287dc240216bf8ea241a343e5adad5662962d2db2a2174c31':
    print('Correct! Congratulations!')
    os.system('/bin/sh')
else:
    print('Wrong!')
