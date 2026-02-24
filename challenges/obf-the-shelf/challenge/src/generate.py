#!/usr/bin/env python3

import random
import os
import sys

flag = sys.argv[1]
seed = int(sys.argv[2])
outfile = sys.argv[3]

operations = ['ADD', 'SUB', 'MUL']

print(f'Generating code for flag "{flag}" with seed {seed}')

random.seed(seed)
with open(outfile, 'w') as fout:
    fout.write('#include "check.h"\n')
    fout.write('int check(uint8_t* flag, size_t len) {\n')
    statement_params = [(idx, x, 1+2*random.randrange(0, 0x80), random.choice(operations)) for idx, x in enumerate(flag.encode())]
    random.shuffle(statement_params)
    for idx, target, param1, operation in statement_params:
        fout.write(f'\tif((VM_{operation}(flag[{idx}], ')
        match operation:
            case 'ADD':
                #fout.write('+')
                param2 = (target+param1)&0xFF
            case 'SUB':
                #fout.write('-')
                param2 = (target-param1+0x100)&0xFF
            case 'MUL':
                #fout.write('*')
                param2 = (target*param1)&0xFF
            case 'XOR':
                #fout.write('^')
                param2 = (target^param1)&0xFF
        fout.write(f' {param1})&0xFF) != {param2}) {{ return 0; }}\n')
    fout.write('\treturn 1;\n')
    fout.write('}\n')
