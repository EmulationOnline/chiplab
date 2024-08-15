#!/usr/bin/env python3

import sys
import os

# Configuration
MAX_CYCLES = 200
SERIAL_BAUD = 9600

oldprint = print
def print(*args, **kwargs):
    # print timing is used for debugging, make sure it flushes immediately
    oldprint(*args, **kwargs)
    sys.stdout.flush()

oldhex = hex
def hex(v):
    # arduino uses uppercase nibbles but lowercase 0x
    return f'0x{v:X}'

def tty_readline(t):
    '''
    read a newline terminated line from port
    '''
    print('> ', end='')
    data = ''
    while len(data) == 0 or data[-1] != "\n":
        data += t.read().decode('ascii')
        print(data, end='')
    data = data.strip()
    # print(data)
    return data

class StatusLine:
    def __init__(self, **kwargs):
        assert kwargs['address'] != None
        self.read = kwargs['read']
        self.address = kwargs['address']
        self.data = kwargs['data']

def read_val(val):
    '''
    decimal or
    0xHEX
    '''
    def read_hex(val):
        assert val[0:2] == '0x'
        return int(val, 0)
    if len(val) >2 and val[1] == 'x':
        return read_hex(val)
    else:
        return int(val)

def tty_expect_status(tty):
    '''
    read a line, and parse it as a pin status line.
    example status line:
    a=0x40 r=1 d=0
    '''
    line = tty_readline(tty).strip().split(' ')
    fields = {'read': None, 'address': None, 'data': None}
    for f in line:
        parts = f.split('=') 
        assert len(parts) == 2, "Bad status: {}".format(line)
        # print('parts:', parts)
        field = parts[0]
        val = read_val(parts[1])
        if field == 'r':
            fields['read'] = val
        elif field == 'a':
            fields['address'] = val
        elif field == 'd':
            fields['data'] = val
    return StatusLine(**fields)



    assert False

def tty_write(f, data):
    print("< {} ".format(data), end='')
    f.write((data + "\n").encode('ascii'))
    print('ok') # signal write completed

def port_setup(port):
    cmd = "stty -F {} {}".format(port, SERIAL_BAUD)
    print("$ {}".format(cmd), end='')
    os.system(cmd)
    print(" ok") # signal write completed

def tty_open(port):
    return open(port, 'w+b', buffering=0)

def debug(msg):
    for i, c in enumerate(msg):
        print("{} = {}".format(i, c))

def port_verify(port):
    port_setup(port)
    print('verify ', end='')
    with tty_open(port) as f:
        # clear
        # f.read() # read everything to clear

        tty_write(f, "info")
        msg = tty_readline(f)
    if msg != "6502_0.1":
        debug(msg)
        print("bad response for info: '{}' l={}".format(msg, len(msg)))
        exit(1)
    print('ok')


def port_main(port, memory):
    assert len(memory) == 0x10000
    port_verify(port)
    with tty_open(port) as tty:
        tty_write(tty, "reset")
        assert tty_readline(tty) == 'reset.'
        for i in range(0, MAX_CYCLES):
            tty_write(tty, 'step')
            status = tty_expect_status(tty)
            if status.read:
                data = memory[status.address]
                # print("prg[{}] = {}".format(hex(status.address), hex(data)))
                tty_write(tty, 'd{}'.format(data))
                assert tty_readline(tty) == ('d=' + hex(data))
            else:
                print('6502 wrote {} at {}'.format(hex(status.data),
                                                   hex(status.address)))

def usage():
    print("""
usage:
serial_rom.py serial_port_device rom.bin
""")

if __name__ == "__main__":
    if len(sys.argv) != 3:
          usage()
          exit(1)
    port = sys.argv[1]
    path = sys.argv[2]
    data = open(path, 'rb').read() 
    port_main(port, data)
        
    print('done')
