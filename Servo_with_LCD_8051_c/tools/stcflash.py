#!/usr/bin/env python
#coding=utf-8
# stcflash  Copyright (C) 2013  laborer (laborer@126.com)

# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.


import time
import logging
import sys
import serial
import os.path
import binascii
import struct
import argparse


PROTOCOL_89 = "89"
PROTOCOL_12C5A = "12c5a"
PROTOCOL_12C52 = "12c52"
PROTOCOL_12Cx052 = "12cx052"
PROTOCOL_8 = "8" 
PROTOCOL_15 = '15' 

PROTOSET_89 = [PROTOCOL_89]
PROTOSET_12 = [PROTOCOL_12C5A, PROTOCOL_12C52, PROTOCOL_12Cx052]
PROTOSET_12B = [PROTOCOL_12C52, PROTOCOL_12Cx052]
PROTOSET_8 = [PROTOCOL_8]
PROTOSET_15 = [PROTOCOL_15]
PROTOSET_PARITY = [PROTOCOL_12C5A, PROTOCOL_12C52]

class Programmer:
    def __init__(self, conn, protocol=None):
        self.conn = conn
        self.protocol = protocol

        self.conn.timeout = 0.05
        if self.protocol in PROTOSET_PARITY:
            self.conn.parity = serial.PARITY_EVEN
        else:
            self.conn.parity = serial.PARITY_NONE

        self.chkmode = 0

    def __conn_read(self, size): 
        buf = bytearray() 
        while len(buf) < size:
            s = bytearray(self.conn.read(size - len(buf))) 
            buf += s

            logging.debug("recv: " + " ".join(["%02X" % i for i in s]))

            if len(s) == 0:
                raise IOError()

        return list(buf) 

    def __conn_write(self, s):
        logging.debug("send: " + " ".join(["%02X" % i for i in s]))

        self.conn.write(bytearray(s))

    def __conn_baudrate(self, baud, flush=True):
        logging.debug("baud: %d" % baud)

        if flush:
            if self.protocol not in PROTOSET_8 and self.protocol not in PROTOSET_15:
                self.conn.flush()
                time.sleep(0.2)

        self.conn.baudrate = baud

    def __model_database(self, model):
        modelmap = {0xE0: ("12", 1, {(0x00, 0x1F): ("C54", ""),
                                     (0x60, 0x7F): ("C54", "AD"),
                                     (0x80, 0x9F): ("LE54", ""),
                                     (0xE0, 0xFF): ("LE54", "AD"),
                                     }),
                    0xE1: ("12", 1, {(0x00, 0x1F): ("C52", ""),
                                     (0x20, 0x3F): ("C52", "PWM"),
                                     (0x60, 0x7F): ("C52", "AD"),
                                     (0x80, 0x9F): ("LE52", ""),
                                     (0xA0, 0xBF): ("LE52", "PWM"),
                                     (0xE0, 0xFF): ("LE52", "AD"),
                                     }),
                    0xE2: ("11", 1, {(0x00, 0x1F): ("F", ""),
                                     (0x20, 0x3F): ("F", "E"),
                                     (0x70, 0x7F): ("F", ""),
                                     (0x80, 0x9F): ("L", ""),
                                     (0xA0, 0xBF): ("L", "E"),
                                     (0xF0, 0xFF): ("L", ""),
                                     }),
                    0xE6: ("12", 1, {(0x00, 0x1F): ("C56", ""),
                                     (0x60, 0x7F): ("C56", "AD"),
                                     (0x80, 0x9F): ("LE56", ""),
                                     (0xE0, 0xFF): ("LE56", "AD"),
                                     }),
                    0xD1: ("12", 2, {(0x20, 0x3F): ("C5A", "CCP"),
                                     (0x40, 0x5F): ("C5A", "AD"),
                                     (0x60, 0x7F): ("C5A", "S2"),
                                     (0xA0, 0xBF): ("LE5A", "CCP"),
                                     (0xC0, 0xDF): ("LE5A", "AD"),
                                     (0xE0, 0xFF): ("LE5A", "S2"),
                                     }),
                    0xD2: ("10", 1, {(0x00, 0x0F): ("F", ""),
                                     (0x60, 0x6F): ("F", "XE"),
                                     (0x70, 0x7F): ("F", "X"),
                                     (0xA0, 0xAF): ("L", ""),
                                     (0xE0, 0xEF): ("L", "XE"),
                                     (0xF0, 0xFF): ("L", "X"),
                                     }),
                    0xD3: ("11", 2, {(0x00, 0x1F): ("F", ""),
                                     (0x40, 0x5F): ("F", "X"),
                                     (0x60, 0x7F): ("F", "XE"),
                                     (0xA0, 0xBF): ("L", ""),
                                     (0xC0, 0xDF): ("L", "X"),
                                     (0xE0, 0xFF): ("L", "XE"),
                                     }),
                    0xF0: ("89", 4, {(0x00, 0x10): ("C5", "RC"),
                                     (0x20, 0x30): ("C5", "RC"),  #STC90C5xRC
                                     }),
                    0xF1: ("89", 4, {(0x00, 0x10): ("C5", "RD+"),
                                     (0x20, 0x30): ("C5", "RD+"),  #STC90C5xRD+
                                     }),
                    0xF2: ("12", 1, {(0x00, 0x0F): ("C", "052"),
                                     (0x10, 0x1F): ("C", "052AD"),
                                     (0x20, 0x2F): ("LE", "052"),
                                     (0x30, 0x3F): ("LE", "052AD"),
                                     }),
                    0xF2A0: ("15W", 1, {(0xA0, 0xA5): ("1", ""),   #STC15W1系列
                                     }),
                    0xF400: ("15F", 8, {(0x00, 0x07): ("2K", "S2"),   #STC15F2K系列
                                     }),
                    0xF407: ("15F", 60, {(0x07, 0x08): ("2K", "S2"),   #STC15F2K系列
                                     }),
                    0xF408: ("15F", 61, {(0x08, 0x09): ("2K", "S2"),   #STC15F2K系列
                                     }),
                    0xF400: ("15F", 4, {(0x09, 0x0C): ("4", "AD"),   #STC15FAD系列
                                     }),
                    0xF410: ("15F", 8, {(0x10, 0x17): ("1K", "AS"),   #STC15F1KAS系列
                                     }),
                    0xF417: ("15F", 60, {(0x17, 0x18): ("1K", "AS"),   #STC15F1KAS系列
                                     }),
                    0xF418: ("15F", 61, {(0x18, 0x19): ("1K", "AS"),   #STC15F1KAS系列
                                     }),
                    0xF420: ("15F", 8, {(0x20, 0x27): ("1K", "S"),   #STC15F1KS系列
                                     }),
                    0xF427: ("15F", 60, {(0x27, 0x28): ("1K", "S"),   #STC15F1KS系列
                                     }),
                    0xF440: ("15F", 8, {(0x40, 0x47): ("1K", "S2"),   #STC15F1KS2系列
                                     }),
                    0xF447: ("15F", 60, {(0x47, 0x48): ("1K", "S2"),   #STC15F1KS2系列
                                     }),
                    0xF448: ("15F", 61, {(0x48, 0x49): ("1K", "S2"),   #STC15F1KS2系列
                                     }),
                    0xF44C: ("15F", 13, {(0x4C, 0x4D): ("4", "AD"),  
                                     }),
                    0xF450: ("15F", 8, {(0x50, 0x57): ("1K", "AS"),   #STC15F1KAS系列
                                     }),
                    0xF457: ("15F", 60, {(0x57, 0x58): ("1K", "AS"),   #STC15F1KAS系列
                                     }),
                    0xF458: ("15F", 61, {(0x58, 0x59): ("1K", "AS"),   #STC15F1KAS系列
                                     }),
                    0xF460: ("15F", 8, {(0x60, 0x67): ("1K", "S"),   #STC15F1KS系列
                                     }),
                    0xF467: ("15F", 60, {(0x67, 0x68): ("1K", "S"),   #STC15F1KS系列
                                     }),
                    0xF468: ("15F", 61, {(0x68, 0x69): ("1K", "S"),   #STC15F1KS系列
                                     }),
                    0xF480: ("15L", 8, {(0x80, 0x87): ("2K", "S2"),   #STC15L2KS2系列
                                     }),
                    0xF487: ("15L", 60, {(0x87, 0x88): ("2K", "S2"),   #STC15L2KS2系列
                                     }),
                    0xF488: ("15L", 61, {(0x88, 0x89): ("2K", "S2"),   #STC15L2KS2系列
                                     }),
                    0xF489: ("15L", 5, {(0x89, 0x8C): ("4", "AD"),   #STC15L4AD系列
                                     }),
                    0xF490: ("15L", 8, {(0x90, 0x97): ("2K", "AS"),   #STC15L2KAS系列
                                     }),
                    0xF497: ("15L", 60, {(0x97, 0x98): ("2K", "AS"),   #STC15L2KAS系列
                                     }),
                    0xF498: ("15L", 61, {(0x98, 0x99): ("2K", "AS"),   #STC15L2KAS系列
                                     }),
                    0xF4A0: ("15L", 8, {(0xA0, 0xA7): ("2K", "S"),   #STC15L2KS系列
                                     }),
                    0xF4A7: ("15L", 60, {(0xA7, 0xA8): ("2K", "S"),   #STC15L2KS系列
                                     }),
                    0xF4A8: ("15L", 61, {(0xA8, 0xA9): ("2K", "S"),   #STC15L2KS系列
                                     }),
                    0xF4C0: ("15L", 8, {(0xC0, 0xC7): ("1K", "S2"),   #STC15L1KS2系列
                                     }),
                    0xF4C7: ("15L", 60, {(0xC7, 0xC8): ("1K", "S2"),   #STC15L1KS2系列
                                     }),
                    0xF4C8: ("15L", 61, {(0xC8, 0xC9): ("1K", "S2"),   #STC15L1KS2系列
                                     }),
                    0xF4CC: ("15L", 13, {(0xCC, 0xCD): ("4", "AD"),   
                                     }),
                    0xF4D0: ("15L", 8, {(0xD0, 0xD7): ("1K", "AS"),   #STC15L1KS2系列
                                     }),
                    0xF4D7: ("15L", 60, {(0xD7, 0xD8): ("1K", "AS"),   #STC15L1KS2系列
                                     }),
                    0xF4D8: ("15L", 61, {(0xD8, 0xD9): ("1K", "AS"),   #STC15L1KS2系列
                                     }),
                    0xF4E0: ("15L", 8, {(0xE0, 0xE7): ("1K", "S"),   #STC15L1KS系列
                                     }),
                    0xF4E7: ("15L", 60, {(0xE7, 0xE8): ("1K", "S"),   #STC15L1KS系列
                                     }),
                    0xF4E8: ("15L", 61, {(0xE8, 0xE9): ("1K", "S"),   #STC15L1KS系列
                                     }),
                    0xF500: ("15W", 1, {(0x00, 0x04): ("1", "SW"),   #STC15W1SW系列
                                     }),
                    0xF507: ("15W", 1, {(0x07, 0x0B): ("1", "S"),   #STC15W1S系列
                                     }),
                    0xF510: ("15W", 1, {(0x10, 0x14): ("2", "S"),   #STC15W2S系列
                                     }),
                    0xF514: ("15W", 8, {(0x14, 0x17): ("1K", "S"),   #STC15W1KS系列
                                     }),
                    0xF518: ("15W", 4, {(0x18, 0x1A): ("4", "S"),   #STC15W4S系列
                                     }),
                    0xF51A: ("15W", 4, {(0x1A, 0x1C): ("4", "S"),   #STC15W4S系列
                                     }),
                    0xF51C: ("15W", 4, {(0x1C, 0x1F): ("4", "AS"),   #STC15W4AS系列
                                     }),
                    0xF51F: ("15W", 10, {(0x19, 0x20): ("4", "AS"),   #STC15W4AS系列
                                     }),
                    0xF520: ("15W", 12, {(0x20, 0x21): ("4", "AS"),   #STC15W4AS系列
                                     }),
                    0xF522: ("15W", 16, {(0x22, 0x23): ("4K", "S4"),   #STC15W4KS4系列
                                     }),
                    0xF523: ("15W",24, {(0x23, 0x24): ("4K", "S4"),   #STC15W4KS4系列
                                     }),
                    0xF524: ("15W", 32, {(0x24, 0x25): ("4K", "S4"),   #STC15W4KS4系列
                                     }),
                    0xF525: ("15W", 40, {(0x25, 0x26): ("4K", "S4"),   #STC15W4KS4系列
                                     }),
                    0xF526: ("15W", 48, {(0x26, 0x27): ("4K", "S4"),   #STC15W4KS4系列
                                     }),
                    0xF527: ("15W", 56, {(0x27, 0x28): ("4K", "S4"),   #STC15W4KS4系列
                                     }),
                    0xF529: ("15W", 1, {(0x29, 0x2B): ("4", "A4"),   #STC15W4AS系列
                                     }),
                    0xF52C: ("15W", 8, {(0x2C, 0x2E): ("1K", "PWM"),   #STC15W1KPWM系列
                                     }),
                    0xF52E: ("15W", 20, {(0x2E, 0x2F): ("1K", "S"),   #STC15W1KS系列
                                     }),
                    0xF52F: ("15W", 32, {(0x2F, 0x30): ("2K", "S2"),   #STC15W2KS2系列
                                     }),
                    0xF530: ("15W", 48, {(0x30, 0x31): ("2K", "S2"),   #STC15W2KS2系列
                                     }),
                    0xF531: ("15W", 32, {(0x31, 0x32): ("2K", "S2"),   #STC15W2KS2系列
                                     }),
                    0xF533: ("15W", 20, {(0x33, 0x34): ("1K", "S2"),   #STC15W1KS2系列
                                     }),
                    0xF534: ("15W", 32, {(0x34, 0x35): ("1K", "S2"),   #STC15W1KS2系列
                                     }),
                    0xF535: ("15W", 48, {(0x35, 0x36): ("1K", "S2"),   #STC15W1KS2系列
                                     }),
                    0xF544: ("15W", 5, {(0x44, 0x45): ("", "SW"),   #STC15SW系列
                                     }),
                    0xF554: ("15W", 5, {(0x54, 0x55): ("2", "S"),   #STC15W2S系列
                                     }),
                    0xF557: ("15W", 29, {(0x57, 0x58): ("1K", "S"),   #STC15W1KS系列
                                     }),
                    0xF55C: ("15W", 13, {(0x5C, 0x5D): ("4", "S"),   #STC15W4S系列
                                     }),
                    0xF568: ("15W", 58, {(0x68, 0x69): ("4K", "S4"),   #STC15W4KS4系列
                                     }),
                    0xF569: ("15W", 61, {(0x69, 0x6A): ("4K", "S4"),   #STC15W4KS4系列
                                     }),
                    0xF56C: ("15W", 58, {(0x6C, 0x6D): ("4K", "S4-Student"),   #STC15W4KS4系列
                                     }),
                    0xF57E: ("15U", 8, {(0x7E, 0x85): ("4K", "S4"),   #STC15U4KS4系列
                                     }),
                    0xF600: ("15H", 8, {(0x00, 0x08): ("4K", "S4"),   #STC154K系列
                                     }),
                    0xF620: ("8A", 8, {(0x20, 0x28): ("8K", "S4A12"),   #STC8A8K系列
                                     }),
                    0xF628: ("8A", 60, {(0x28, 0x29): ("8K", "S4A12"),   #STC8A8K系列
                                     }),
                    0xF630: ("8F", 8, {(0x30, 0x38): ("2K", "S4"),   #STC8F2K系列
                                     }),
                    0xF638: ("8F", 60, {(0x38, 0x39): ("2K", "S4"),   #STC8F2K系列
                                     }),
                    0xF640: ("8F", 8, {(0x40, 0x48): ("2K", "S2"),   #STC8F2K系列
                                     }),
                    0xF648: ("8F", 60, {(0x48, 0x49): ("2K", "S2"),   #STC8F2K系列
                                     }),
                    0xF650: ("8A", 8, {(0x50, 0x58): ("4K", "S2A12"),   #STC8A4K系列
                                     }),
                    0xF658: ("8A", 60, {(0x58, 0x59): ("4K", "S2A12"),   #STC8A4K系列
                                     }),
                    0xF660: ("8F", 2, {(0x60, 0x66): ("1K", "S2"),   #STC8F1K系列
                                     }),
                    0xF666: ("8F", 17, {(0x66, 0x67): ("1K", "S2"),   #STC8F1K系列
                                     }),
                    0xF670: ("8F", 2, {(0x70, 0x76): ("1K", ""),   #STC8F1K系列
                                     }),
                    0xF676: ("8F", 17, {(0x76, 0x77): ("1K", ""),   #STC8F1K系列
                                     }),
                    0xF700: ("8C", 2, {(0x00, 0x06): ("1K", ""),   #STC8C系列
                                     }),   
                    0xF730: ("8H", 2, {(0x30, 0x36): ("1K", ""),   #STC8H1K系列
                                     }),
                    0xF736: ("8H", 17, {(0x36, 0x37): ("1K", ""),   #STC8H1K系列
                                     }),
                    0xF740: ("8H", 8, {(0x40, 0x42): ("3K", "S4"),   #STC8H3K系列
                                     }),
                    0xF742: ("8H", 60, {(0x42, 0x43): ("3K", "S4"),   #STC8H3K系列
                                     }),
                    0xF743: ("8H", 64, {(0x43, 0x44): ("3K", "S4"),   #STC8H3K系列
                                     }),
                    0xF748: ("8H", 16, {(0x48, 0x4A): ("3K", "S2"),   #STC8H3K系列
                                     }),
                    0xF74A: ("8H", 60, {(0x4A, 0x4B): ("3K", "S2"),   #STC8H3K系列
                                     }),
                    0xF74B: ("8H", 64, {(0x4B, 0x4C): ("3K", "S2"),   #STC8H3K系列
                                     }),
                    0xF750: ("8G", 2, {(0x50, 0x56): ("1K", "-20/16pin"),   #STC8G1K系列
                                     }),
                    0xF756: ("8G", 17, {(0x56, 0x57): ("1K", "-20/16pin"),   #STC8G1K系列
                                     }),
                    0xF760: ("8G", 16, {(0x60, 0x62): ("2K", "S4"),   #STC8G2K系列
                                     }),
                    0xF762: ("8G", 60, {(0x62, 0x63): ("2K", "S4"),   #STC8G2K系列
                                     }),
                    0xF763: ("8G", 64, {(0x63, 0x64): ("2K", "S4"),   #STC8G2K系列
                                     }),
                    0xF768: ("8G", 16, {(0x68, 0x6A): ("2K", "S2"),   #STC8G2K系列
                                     }),
                    0xF76A: ("8G", 60, {(0x6A, 0x6B): ("2K", "S2"),   #STC8G2K系列
                                     }),
                    0xF76B: ("8G", 64, {(0x6B, 0x6C): ("2K", "S2"),   #STC8G2K系列
                                     }),
                    0xF770: ("8G", 2, {(0x70, 0x76): ("1K", "T"),   #STC8G2K系列
                                     }),
                    0xF776: ("8G", 17, {(0x76, 0x77): ("1K", "T"),   #STC8G2K系列
                                     }),
                    0xF780: ("8H", 16, {(0x80, 0x82): ("8K", "U"),   #STC8H8K系列
                                     }),
                    0xF782: ("8H", 60, {(0x82, 0x83): ("8K", "U"),   #STC8H8K系列
                                     }),
                    0xF783: ("8H", 64, {(0x83, 0x84): ("8K", "U"),   #STC8H8K系列
                                     }),
                    0xF790: ("8G", 2, {(0x90, 0x96): ("1K", "A-8PIN"),   #STC8G1K系列
                                     }),
                    0xF796: ("8G", 17, {(0x96, 0x97): ("1K", "A-8PIN"),   #STC8G1K系列
                                     }),
                    0xF7A0: ("8G", 2, {(0xA0, 0xA6): ("1K", "-8PIN"),   #STC8G1K系列
                                     }),
                    0xF7A6: ("8G", 17, {(0xA6, 0xA7): ("1K", "-8PIN"),   #STC8G1K系列
                                     }),
                    }

        iapmcu = ((0xD1, 0x3F), (0xD1, 0x5F), (0xD1, 0x7F), (0xF4, 0x4D), (0xF4, 0x99), (0xF4, 0xD9), (0xF5, 0x58), 
                  (0xD2, 0x7E), (0xD2, 0xFE), (0xF4, 0x09), (0xF4, 0x59), (0xF4, 0xA9), (0xF4, 0xE9), (0xF5, 0x5D), 
                  (0xD3, 0x5F), (0xD3, 0xDF), (0xF4, 0x19), (0xF4, 0x69), (0xF4, 0xC9), (0xF5, 0x45), (0xF5, 0x62),
                  (0xE2, 0x76), (0xE2, 0xF6), (0xF4, 0x49), (0xF4, 0x89), (0xF4, 0xCD), (0xF5, 0x55), (0xF5, 0x69),
                  (0xF5, 0x6A), (0xF5, 0x6D),
                  )

        try:
            model = tuple(model) 
            if self.model[0] in [0xF4, 0xF5, 0xF6, 0xF7]:
                prefix, romratio, fixmap = modelmap[stc_type_map(model[0],model[1])]
            elif self.model[0] == 0xF2 and self.model[1] in range(0xA0, 0xA6):
                prefix, romratio, fixmap = modelmap[stc_type_map(model[0],model[1])]
                self.protocol = PROTOCOL_15
            else:
                prefix, romratio, fixmap = modelmap[model[0]]

            if model[0] in (0xF0, 0xF1) and 0x20 <= model[1] <= 0x30:
                prefix = "90" 

            for key, value in fixmap.items():
                if key[0] <= model[1] <= key[1]:
                    break
            else:
                raise KeyError()

            infix, postfix = value

            romsize = romratio * (model[1] - key[0])

            try:
                romsize = {(0xF0, 0x03): 13}[model]
            except KeyError:
                pass

            if model[0] in (0xF0, 0xF1):
                romfix = str(model[1] - key[0])
            elif model[0] in (0xF2,):
                romfix = str(romsize)
            else:
                romfix = "%02d" % romsize

            name = "IAP" if model in iapmcu else "STC"
            name += prefix + infix + romfix + postfix
            return (name, romsize)

        except KeyError:
            return ("Unknown %02X %02X" % model, None)

    def recv(self, timeout = 1, start = [0x46, 0xB9, 0x68]): 
        timeout += time.time()

        while time.time() < timeout:
            try:
                if self.__conn_read(len(start)) == start:                   
                    break
            except IOError:
                continue
        else:
            logging.debug("recv(..): Timeout")
            raise IOError()

        chksum = start[-1] 

        s = self.__conn_read(2) 
        n = s[0] * 256 + s[1] 
        if n > 64: 
            logging.debug("recv(..): Incorrect packet size")
            raise IOError()
        chksum += sum(s) 

        s = self.__conn_read(n - 3) 
        if s[n - 4] != 0x16: 
            logging.debug("recv(..): Missing terminal symbol")
            raise IOError()

        chksum += sum(s[:-(1+self.chkmode)]) 
        if self.chkmode > 0 and chksum & 0xFF != s[-2]:
            logging.debug("recv(..): Incorrect checksum[0]")
            raise IOError()
        elif self.chkmode > 1 and (chksum >> 8) & 0xFF != s[-3]:
            logging.debug("recv(..): Incorrect checksum[1]")
            raise IOError()

        return (s[0], s[1:-(1+self.chkmode)]) 
    def first_recv(self, timeout = 1, start = [0x46, 0xB9, 0x68]): 
        timeout += time.time()

        while time.time() < timeout:
            try:
                if self.__conn_read(len(start)) == start:
                    time.sleep(0.02) #加上20ms延时，增大接收成功率
                    break
            except IOError:
                continue
        else:
            logging.debug("recv(..): Timeout")
            raise IOError()

        chksum = start[-1]

        s = self.__conn_read(2) 
        n = s[0] * 256 + s[1] 
        if n > 64:
            logging.debug("recv(..): Incorrect packet size")
            raise IOError()
        chksum += sum(s) 

        s = self.__conn_read(n - 3) 
        if s[n - 4] != 0x16: 
            logging.debug("recv(..): Missing terminal symbol")
            raise IOError()

        chksum += sum(s[:-(1+self.chkmode)]) 
        if self.chkmode > 0 and chksum & 0xFF != s[-2]:
            logging.debug("recv(..): Incorrect checksum[0]")
            raise IOError()
        elif self.chkmode > 1 and (chksum >> 8) & 0xFF != s[-3]:
            logging.debug("recv(..): Incorrect checksum[1]")
            raise IOError()

        return (s[0], s[1:-(1+self.chkmode)]) 

    def send(self, cmd, dat): 
        buf = [0x46, 0xB9, 0x6A] 

        n = 1 + 2 + 1 + len(dat) + self.chkmode + 1 
        buf += [n >> 8, n & 0xFF, cmd] 

        buf += dat  

        chksum = sum(buf[2:]) 
        if self.chkmode > 1:
            buf += [(chksum >> 8) & 0xFF] 
        buf += [chksum & 0xFF, 0x16] 

        self.__conn_write(buf)

    def detect(self):  
       
        for i in range(500): 
            try:
                if self.protocol in [PROTOCOL_89,PROTOCOL_12C52,PROTOCOL_12Cx052,PROTOCOL_12C5A]:
                    self.__conn_write([0x7F,0x7F])
                    cmd, dat = self.first_recv(0.03, [0x68]) 
                else:
                    self.__conn_write([0x7F])  
                    cmd, dat = self.first_recv(0.03, [0x68]) 
                break
            except IOError:
                pass
        else:
            raise IOError()       
        
        
        self.info = dat[16:]

        self.version = "%d.%d%c" % (self.info[0] >> 4,
                                        self.info[0] & 0x0F,
                                        self.info[1])

        self.model = self.info[3:5]  

        self.name, self.romsize = self.__model_database(self.model)

        logging.info("Model ID: %02X %02X" % tuple(self.model))
        logging.info("Model name: %s" % self.name)
        logging.info("ROM size: %s" % self.romsize)

        
        if self.protocol is None:
            try:
                self.protocol = {0xF0: PROTOCOL_89,       #STC89/90C5xRC
                                 0xF1: PROTOCOL_89,       #STC89/90C5xRD+
                                 0xF2: PROTOCOL_12Cx052,  #STC12Cx052
                                 0xD1: PROTOCOL_12C5A,    #STC12C5Ax
                                 0xD2: PROTOCOL_12C5A,    #STC10Fx
                                 0xE1: PROTOCOL_12C52,    #STC12C52x
                                 0xE2: PROTOCOL_12C5A,    #STC11Fx
                                 0xE6: PROTOCOL_12C52,    #STC12C56x
                                 0xF4: PROTOCOL_15,    #STC15系列
                                 0xF5: PROTOCOL_15,    #STC15系列
                                 0xF6: PROTOCOL_8,  #STC8系列
                                 0xF7: PROTOCOL_8,  #STC8系列
                                 }[self.model[0]]
            except KeyError:
                pass

        if self.protocol in PROTOSET_8: 
            self.fosc = (dat[0]*0x1000000 +dat[1]*0x10000+dat[2]*0x100) /1000000
            self.internal_vol = (dat[34]*256+dat[35]) 
            self.wakeup_fosc = (dat[22]*256+dat[23]) /1000            
            self.test_year = str(hex(dat[36])).replace("0x",'')
            self.test_month = str(hex(dat[37])).replace("0x",'')
            self.test_day = str(hex(dat[38])).replace("0x",'')
            self.version = "%d.%d.%d%c" % (self.info[0] >> 4,
                                        self.info[0] & 0x0F,
                                        self.info[5],
                                        self.info[1]) 
            if dat[10] == 191:
                self.det_low_vol = 2.2
            else:
                self.det_low_vol = (191 - dat[10])*0.3 + 2.1    

        elif self.protocol in PROTOSET_15:
            self.fosc = (dat[7]*0x1000000 +dat[8]*0x10000+dat[9]*0x100) /1000000 
            self.wakeup_fosc = (dat[0]*256+dat[1]) /1000
            self.internal_vol = (dat[34]*256+dat[35])  
            self.test_year = str(hex(dat[41])).replace("0x",'')
            self.test_month = str(hex(dat[42])).replace("0x",'')
            self.test_day = str(hex(dat[43])).replace("0x",'')
            self.version = "%d.%d.%d%c" % (self.info[0] >> 4,
                                        self.info[0] & 0x0F,
                                        self.info[5],
                                        self.info[1])   
            
        else:
            self.fosc = (float(sum(dat[0:16:2]) * 256 + sum(dat[1:16:2])) / 8
                     * self.conn.baudrate / 580974)

        if self.protocol in PROTOSET_PARITY or self.protocol in PROTOSET_8 or self.protocol in PROTOSET_15: 
            self.chkmode = 2
            self.conn.parity = serial.PARITY_EVEN
        else:
            self.chkmode = 1
            self.conn.parity = serial.PARITY_NONE

        if self.protocol is not None:
            del self.info[-self.chkmode:]

            logging.info("Protocol ID: %s" % self.protocol)
            logging.info("Checksum mode: %d" % self.chkmode)
            logging.info("UART Parity: %s"
                         % {serial.PARITY_NONE: "NONE",
                            serial.PARITY_EVEN: "EVEN",
                            }[self.conn.parity])

        for i in range(0, len(self.info), 16):
            logging.info("Info string [%d]: %s"
                         % (i // 16,
                            " ".join(["%02X" % j for j in self.info[i:i+16]])))
    def print_info(self):
        print("系统时钟频率: %.3fMHz" % self.fosc)
        if self.protocol in PROTOSET_8:
            print("掉电唤醒定时器频率: %.3fKHz" % self.wakeup_fosc)
            print("内部参考电压: %d mV" %self.internal_vol)
            print("低压检测电压: %.1f V" %self.det_low_vol) 
            print("内部安排测试时间: 20%s年%s月%s日" %(self.test_year,self.test_month,self.test_day))           

        if self.protocol in PROTOSET_15:
            print("掉电唤醒定时器频率: %.3fKHz" % self.wakeup_fosc)
            print("内部参考电压: %d mV" %self.internal_vol) 
            print("内部安排测试时间: 20%s年%s月%s日" %(self.test_year,self.test_month,self.test_day))   

        print("单片机型号: %s" % self.name)
        print("固件版本号: %s" % self.version)
        if self.romsize is not None:
            print("程序空间: %dKB" % self.romsize)

        if self.protocol == PROTOCOL_89:
            switches = [( 2, 0x80, "Reset stops                                                                                                  "),
                        ( 2, 0x40, "Internal XRAM"),
                        ( 2, 0x20, "Normal ALE pin"),
                        ( 2, 0x10, "Full gain oscillator"),
                        ( 2, 0x08, "Not erase data EEPROM"),
                        ( 2, 0x04, "Download regardless of P1"),
                        ( 2, 0x01, "12T mode")]

        elif self.protocol == PROTOCOL_12C5A:
            switches = [( 6, 0x40, "Disable reset2 low level detect"),
                        ( 6, 0x01, "Reset pin not use as I/O port"),
                        ( 7, 0x80, "Disable long power-on-reset latency"),
                        ( 7, 0x40, "Oscillator high gain"),
                        ( 7, 0x02, "External system clock source"),
                        ( 8, 0x20, "WDT disable after power-on-reset"),
                        ( 8, 0x04, "WDT count in idle mode"),
                        (10, 0x02, "Not erase data EEPROM"),
                        (10, 0x01, "Download regardless of P1")]
            print(" WDT prescal: %d" % 2**((self.info[8] & 0x07) + 1))

        elif self.protocol in PROTOSET_12B:
            switches = [(8, 0x02, "Not erase data EEPROM")]

        else:
            switches = []

        for pos, bit, desc in switches:
            print(" [%c] %s" % ("X" if self.info[pos] & bit else " ", desc))

    def handshake(self):
        baud0 = self.conn.baudrate
        
        if self.protocol in PROTOSET_8:
            baud = 115200 #若没指定波特率，默认为115200
            if highbaud_pre != 115200:
                baud = highbaud_pre
            #支持460800以内的任意波特率
            #典型波特率：460800、230400、115200、57600、38400、28800、19200、14400、9600、4800
            if baud in range(460801): 
                #定时器1重载值计算微调，可能由于目标芯片的差异性需要微调
                if baud in [300000,350000]:
                    Timer1_value = int(65536.2 - float(24.0 * 1000000 / 4 / baud))  
                else: 
                    Timer1_value = int(65536.5 - float(24.0 * 1000000 / 4 / baud))  

                if self.fosc < 24.5 and self.fosc > 23.5:    #24M
                    foc_value = 0x7B
                elif self.fosc < 27.5 and self.fosc > 26.5:  #27M
                    foc_value = 0xB0
                elif self.fosc < 22.7 and self.fosc > 21.7:  #22.1184M
                    foc_value = 0x5A
                elif self.fosc < 20.5 and self.fosc > 19.5:  #20M
                    foc_value = 0x35
                elif self.fosc < 12.3 and self.fosc > 11.7:  #12M
                    foc_value = 0x7B
                elif self.fosc < 11.4 and self.fosc > 10.8:  #11.0592M
                    foc_value = 0x5A
                elif self.fosc < 18.8 and self.fosc > 18.0:  #18.432M
                    foc_value = 0x1A
                elif self.fosc < 6.3 and self.fosc > 5.7:#6M
                    foc_value = 0x12
                elif self.fosc < 5.9 and self.fosc > 5.0:  #5.5296M
                    foc_value = 0x5A
                else:
                    foc_value = 0x6B
                              
                baudstr = [0x00, 0x00, Timer1_value >> 8, Timer1_value & 0xff, 0x01, foc_value, 0x81]
                
                self.send(0x01, baudstr )
                try:
                    cmd, dat = self.recv()                   
                except Exception:
                    logging.info("Cannot use baudrate %d" % baud)
                    time.sleep(0.2)
                    self.conn.flushInput()
                finally:
                    self.__conn_baudrate(baud0, False)
                
            logging.info("Change baudrate to %d" % baud)
            self.__conn_baudrate(baud)
            self.baudrate = baud
        elif self.protocol in PROTOSET_15:
            baud = 115200 #若没指定波特率，默认为115200
            if highbaud_pre != 115200:
                baud = highbaud_pre
            #支持460800以内的任意波特率
            #典型波特率：460800、230400、115200、57600、38400、28800、19200、14400、9600、4800
            if baud in range(460801): 
                #定时器1重载值计算微调，可能由于目标芯片的差异性需要微调
                if baud in [300000,350000]:
                    Timer1_value = int(65536.2 - float(22.1184 * 1000000 / 4 / baud))  
                else: 
                    Timer1_value = int(65536.5 - float(22.1184 * 1000000 / 4 / baud))  

                if self.fosc < 24.5 and self.fosc > 23.5:    #24M
                    foc_value_1 = 0x40
                    foc_value_2 = 0x9F
                elif self.fosc < 27.5 and self.fosc > 26.5:  #27M
                    foc_value_1 = 0x40
                    foc_value_2 = 0xDC
                elif self.fosc < 22.7 and self.fosc > 21.7:  #22.1184M
                    foc_value_1 = 0x40
                    foc_value_2 = 0x79
                elif self.fosc < 20.5 and self.fosc > 19.5:  #20M
                    foc_value_1 = 0x40
                    foc_value_2 = 0x4F
                elif self.fosc < 12.3 and self.fosc > 11.7:  #12M
                    foc_value_1 = 0x80
                    foc_value_2 = 0xA2
                elif self.fosc < 11.4 and self.fosc > 10.8:  #11.0592M
                    foc_value_1 = 0x80
                    foc_value_2 = 0x7D
                elif self.fosc < 18.8 and self.fosc > 18.0:  #18.432M
                    foc_value_1 = 0x40
                    foc_value_2 = 0x31
                elif self.fosc < 6.3 and self.fosc > 5.7:#6M
                    foc_value_1 = 0xC0
                    foc_value_2 = 0x9f
                elif self.fosc < 5.9 and self.fosc > 5.0:  #5.5296M
                    foc_value_1 = 0xC0
                    foc_value_2 = 0x7B
                              
                baudstr = [0x6d, 0x40, Timer1_value >> 8, Timer1_value & 0xff, foc_value_1,foc_value_2, 0x81]
                #baudstr = [0x6b, 0x40, 0xff,0xf4,   0x40,0x92, 0x81]
                
                self.send(0x01, baudstr )
                try:
                    cmd, dat = self.recv()                   
                except Exception:
                    logging.info("Cannot use baudrate %d" % baud)
                    time.sleep(0.2)
                    self.conn.flushInput()
                finally:
                    self.__conn_baudrate(baud0, False)
                
            logging.info("Change baudrate to %d" % baud)
            self.__conn_baudrate(baud)
            self.baudrate = baud
        else:
            for baud in [115200, 57600, 38400, 28800, 19200,
                     14400, 9600, 4800, 2400, 1200]:

                t = self.fosc * 1000000 / baud / 32
                if self.protocol not in PROTOSET_89:
                    t *= 2

                if abs(round(t) - t) / t > 0.03:
                    continue

                if self.protocol in PROTOSET_89:
                    tcfg = 0x10000 - int(t + 0.5)
                else:
                    if t > 0xFF:
                        continue
                    tcfg = 0xC000 + 0x100 - int(t + 0.5)

                baudstr = [tcfg >> 8,
                       tcfg & 0xFF,
                       0xFF - (tcfg >> 8),
                       min((256 - (tcfg & 0xFF)) * 2, 0xFE),
                       int(baud0 / 60)]

                logging.info("Test baudrate %d (accuracy %0.4f) using config %s"
                         % (baud,
                            abs(round(t) - t) / t,
                            " ".join(["%02X" % i for i in baudstr])))
               
                if self.protocol in PROTOSET_89:
                    freqlist = (40, 20, 10, 5)
                else:
                    freqlist = (30, 24, 20, 12, 6, 3, 2, 1)

                for twait in range(0, len(freqlist)):
                    if self.fosc > freqlist[twait]:
                        break

                logging.info("Waiting time config %02X" % (0x80 + twait))

                self.send(0x8F, baudstr + [0x80 + twait])

                try:
                    self.__conn_baudrate(baud)
                    cmd, dat = self.recv()
                    break
                except Exception:
                    logging.info("Cannot use baudrate %d" % baud)

                    time.sleep(0.2)
                    self.conn.flushInput()
                finally:
                    self.__conn_baudrate(baud0, False)

            else:
                raise IOError()
            logging.info("Change baudrate to %d" % baud)

            self.send(0x8E, baudstr)
            self.__conn_baudrate(baud)
            self.baudrate = baud

            cmd, dat = self.recv()


    def erase(self):
        if self.protocol in PROTOSET_89:
            self.send(0x84, [0x01, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33])
            cmd, dat = self.recv(10)
            assert cmd == 0x80
        
        elif self.protocol in PROTOSET_8 or self.protocol in PROTOSET_15: 
            self.send(0x05, [0x00, 0x00, 0x5A, 0xA5])
            cmd, dat = self.recv(10)
            self.send(0x03, [0x00, 0x00, 0x5A, 0xA5])
            cmd, dat = self.recv(10)
            for i in range(7):
                dat[i] = hex(dat[i])
                dat[i] = str(dat[i])
                dat[i] = dat[i].replace("0x",'')
                if len(dat[i]) == 1:
                    dat_value = list(dat[i])
                    dat_value.insert(0, '0')
                    dat[i] = ''.join(dat_value) 
            serial_number = ""
            for i in dat:
                serial_number = serial_number +str(i)
            self.serial_number = str(serial_number)
            print("\r")
            sys.stdout.write("芯片出厂序列号: ")
            sys.stdout.write(self.serial_number.upper())
            sys.stdout.flush()
            print("\r")

        else:
            self.send(0x84, ([0x00, 0x00, self.romsize * 4,
                              0x00, 0x00, self.romsize * 4]
                             + [0x00] * 12
                             + [i for i in range(0x80, 0x0D, -1)]))
            cmd, dat = self.recv(10)
            if dat:
                logging.info("Serial number: "
                             + " ".join(["%02X" % j for j in dat]))

    def flash(self, code):
        code = list(code) + [0xff] * (511 - (len(code) - 1) % 512)
        
        for i in range(0, len(code), 128):
            logging.info("Flash code region (%04X, %04X)" % (i, i + 127))
           
            if self.protocol in PROTOSET_8 or self.protocol in PROTOSET_15:
                flag_test = 1
                addr = [i >> 8, i & 0xFF, 0x5A, 0xA5]
                if flag_test == 1:
                    self.send(0x22, addr + code[i:i+128])
                    flag_test = 10
                else:
                    self.send(0x02, addr + code[i:i+128])
            else:
                addr = [0, 0, i >> 8, i & 0xFF, 0, 128]
                self.send(0x00, addr + code[i:i+128])
            cmd, dat = self.recv()

            #assert dat[0] == sum(code[i:i+128]) % 256

            yield (i + 128.0) / len(code)

    def options(self, **kwargs):
        erase_eeprom = kwargs.get("erase_eeprom", None)

        dat = []
        fosc = list(bytearray(struct.pack(">I", int(self.fosc * 1000000))))

        if self.protocol == PROTOCOL_89:
            if erase_eeprom is not None:
                self.info[2] &= 0xF7
                self.info[2] |= 0x00 if erase_eeprom else 0x08
            dat = self.info[2:3] + [0xFF]*3

        elif self.protocol == PROTOCOL_12C5A:
            if erase_eeprom is not None:
                self.info[10] &= 0xFD
                self.info[10] |= 0x00 if erase_eeprom else 0x02
            dat = (self.info[6:9] + [0xFF]*5 + self.info[10:11]
                   + [0xFF]*6 + fosc)

        elif self.protocol in PROTOSET_12B:
            if erase_eeprom is not None:
                self.info[8] &= 0xFD
                self.info[8] |= 0x00 if erase_eeprom else 0x02
            dat = (self.info[6:11] + fosc + self.info[12:16] + [0xFF]*4
                   + self.info[8:9] + [0xFF]*7 + fosc + [0xFF]*3)

        elif erase_eeprom is not None:
            logging.info("Modifying options is not supported for this target")
            return False

        if dat:
            self.send(0x8D, dat)
            cmd, dat = self.recv()

        return True

    def terminate(self):
        logging.info("Send termination command")

        if self.protocol in PROTOSET_8 or self.protocol in PROTOSET_15:
            self.send(0xFF, [])
        else:
            self.send(0x82, [])
        self.conn.flush()
        time.sleep(0.2)

    def unknown_packet_1(self):
        if self.protocol in PROTOSET_PARITY:
            logging.info("Send unknown packet (50 00 00 36 01 ...)")
            self.send(0x50, [0x00, 0x00, 0x36, 0x01] + self.model)
            cmd, dat = self.recv()
            assert cmd == 0x8F and not dat

    def unknown_packet_2(self):
        if self.protocol not in PROTOSET_PARITY and self.protocol not in PROTOSET_8 and self.protocol not in PROTOSET_15:
            for i in range(5):
                logging.info("Send unknown packet (80 00 00 36 01 ...)")
                self.send(0x80, [0x00, 0x00, 0x36, 0x01] + self.model)
                cmd, dat = self.recv()
                assert cmd == 0x80 and not dat

    def unknown_packet_3(self):
        if self.protocol in PROTOSET_PARITY:
            logging.info("Send unknown packet (69 00 00 36 01 ...)")
            self.send(0x69, [0x00, 0x00, 0x36, 0x01] + self.model)
            cmd, dat = self.recv()
            assert cmd == 0x8D and not dat


def autoisp(conn, baud, magic):
    if not magic:
        return

    bak = conn.baudrate
    conn.baudrate = baud
    conn.write(bytearray(ord(i) for i in magic))
    conn.flush()
    time.sleep(0.5)
    conn.baudrate = bak


def program(prog, code, erase_eeprom=None):
    sys.stdout.write("检测目标...")
    sys.stdout.flush()

    prog.detect()

    print("完成")

    prog.print_info() 

    if prog.protocol is None:
        raise IOError("未知目标")

    if code is None:
        return

    prog.unknown_packet_1() 

    sys.stdout.write("切换至最高波特率: ")
    sys.stdout.flush()

    prog.handshake() 

    print("%d bps"% prog.baudrate) 

    prog.unknown_packet_2()

    sys.stdout.write("开始擦除芯片...")
    sys.stdout.flush()

    time_start = time.time()

    prog.erase() 

    print("擦除完成")

    print("代码长度: %d bytes" % len(code)) 


    # print("Programming: ", end="", flush=True)
    sys.stdout.write("正在下载用户代码...")  
    sys.stdout.flush()  

    oldbar = 0
    for progress in prog.flash(code): 
        bar = int(progress * 25)  
        sys.stdout.write("#" * (bar - oldbar)) 
        sys.stdout.flush()   
        oldbar = bar

    print(" 完成")

    prog.unknown_packet_3() 

    sys.stdout.write("设置选项...") 
    sys.stdout.flush()

    if prog.options(erase_eeprom=erase_eeprom):
        print("设置完成")
    else:
        print("设置失败")

    prog.terminate() 
    time_end = time.time()
    print("耗时: %.3fs"% (time_end-time_start))


# Convert Intel HEX code to binary format
def hex2bin(code):
    buf = bytearray()
    base = 0
    line = 0

    for rec in code.splitlines():
        # Calculate the line number of the current record
        line += 1

        try:
            # bytes(...) is to support python<=2.6
            # bytearray(...) is to support python<=2.7
            n = bytearray(binascii.a2b_hex(bytes(rec[1:3])))[0]
            dat = bytearray(binascii.a2b_hex(bytes(rec[1:n*2+11])))
        except:
            raise Exception("Line %d: Invalid format" % line)

        if rec[0] != ord(":"):
            raise Exception("Line %d: Missing start code \":\"" % line)
        if sum(dat) & 0xFF != 0:
            raise Exception("Line %d: Incorrect checksum" % line)

        if dat[3] == 0:      # Data record
            addr = base + (dat[1] << 8) + dat[2]
            # Allocate memory space and fill it with 0xFF
            buf[len(buf):] = [0xFF] * (addr + n - len(buf))
            # Copy data to the buffer
            buf[addr:addr+n] = dat[4:-1]

        elif dat[3] == 1:    # EOF record
            if n != 0:
                raise Exception("Line %d: Incorrect data length" % line)

        elif dat[3] == 2:    # Extended segment address record
            if n != 2:
                raise Exception("Line %d: Incorrect data length" % line)
            base = ((dat[4] << 8) + dat[5]) << 4

        elif dat[3] == 4:    # Extended linear address record
            if n != 2:
                raise Exception("Line %d: Incorrect data length" % line)
            base = ((dat[4] << 8) + dat[5]) << 16

        else:
            raise Exception("Line %d: Unsupported record type" % line)

    return buf

def stc_type_map(type, value):  
    if type == 0xF6:      
        if value in range(0x01,0x09):
            return 0xF600
        elif value in range(0x21,0x29):
            return  0xF620
        elif value == 0x29:
            return  0xF628
        elif value in range(0x31,0x39):
            return  0xF630
        elif value == 0x39:
            return  0xF638
        elif value in range(0x41,0x49):
            return  0xF640
        elif value == 0x49:
            return  0xF648
        elif value in range(0x51,0x59):
            return  0xF650
        elif value == 0x59:
            return  0xF658
        elif value in range(0x61,0x67):
            return  0xF660
        elif value == 0x67:
            return  0xF666
        elif value in range(0x71,0x77):
            return  0xF670
        elif value == 0x77:
            return  0xF676
    if type == 0xF7: 
        if value in range(0x01,0x07):
            return  0xF700
        elif value in range(0x31,0x37):
            return  0xF730
        elif value == 0x37:
            return  0xF736
        elif value in range(0x41,0x43):
            return  0xF740
        elif value == 0x43:
            return  0xF742
        elif value == 0x44:
            return  0xF743
        elif value in range(0x49,0x4B):
            return  0xF748
        elif value == 0x4B:
            return  0xF74A
        elif value == 0x4C:
            return  0xF74B
        elif value in range(0x51,0x57):
            return  0xF750
        elif value == 0x57:
            return  0xF756
        elif value in range(0x61,0x63):
            return  0xF760
        elif value == 0x63:
            return  0xF762
        elif value == 0x64:
            return  0xF763
        elif value in range(0x69,0x6B):
            return  0xF768
        elif value == 0x6B:
            return  0xF76A
        elif value == 0x6C:
            return  0xF76B
        elif value in range(0x71,0x77):
            return  0xF770
        elif value == 0x77:
            return  0xF776
        elif value in range(0x81,0x83):
            return  0xF780
        elif value == 0x83:
            return  0xF782
        elif value == 0x84:
            return  0xF783
        elif value in range(0x91,0x97):
            return  0xF790
        elif value == 0x97:
            return  0xF796
        elif value in range(0xA1,0xA7):
            return  0xF7A0
        elif value == 0xA7:
            return  0xF7A6
    if type == 0xF4:
        if value in range(0x01,0x08):
            return  0xF400
        elif value == 0x08:
            return 0xF407
        elif value == 0x09:
            return 0xF408
        elif value in range(0x0A,0x0D):
            return 0xF409
        elif value in range(0x11,0x18):
            return 0xF410
        elif value == 0x18:
            return 0xF417
        elif value == 0x19:
            return 0xF418
        elif value in range(0x21,0x28):
            return 0xF420
        elif value == 0x28:
            return 0xF427
        elif value == 0x29:
            return 0xF428
        elif value in range(0x41,0x48):
            return 0xF440
        elif value == 0x48:
            return 0xF447
        elif value == 0x49:
            return 0xF448
        elif value == 0x4D:
            return 0xF44C
        elif value in range(0x51,0x57):
            return 0xF450
        elif value == 0x58:
            return 0xF457
        elif value == 0x59:
            return 0xF458
        elif value in range(0x61,0x68):
            return 0xF460
        elif value == 0x68:
            return 0xF467
        elif value == 0x69:
            return 0xF468
        elif value in range(0x81,0x88):
            return 0xF480
        elif value == 0x88:
            return 0xF487
        elif value == 0x89:
            return 0xF488
        elif value in range(0x8A,0x8D):
            return 0xF489
        elif value in range(0x91,0x98):
            return 0xF490
        elif value == 0x98:
            return 0xF497
        elif value == 0x99:
            return 0xF498
        elif value in range(0xA1,0xA8):
            return 0xF4A0
        elif value == 0xA8:
            return 0xF4A7
        elif value == 0xA9:
            return 0xF4A8
        elif value in range(0xC1,0xC8):
            return 0xF4C0
        elif value == 0xC8:
            return 0xF4C7
        elif value == 0xC9:
            return 0xF4C8
        elif value == 0xCD:
            return 0xF4CC
        elif value in range(0xD1,0xD8):
            return 0xF4D0
        elif value == 0xD8:
            return 0xF4D7
        elif value == 0xD9:
            return 0xF4D8
        elif value in range(0xE1,0xE8):
            return 0xF4E0
        elif value == 0xE8:
            return 0xF4E7
        elif value == 0xE9:
            return 0xF4E8
    if type == 0xF5:
        if value in range(0x01,0x05):
            return 0xF500
        elif value in range(0x08,0x0C):
            return 0xF507
        elif value in range(0x11,15):
            return 0xF510
        elif value in range(0x15,0x18):
            return 0xF514
        elif value in range(0x19,0x1B):
            return 0xF518
        elif value in range(0x1B,0x1D):
            return 0xF51A
        elif value in range(0x1D,0x20):
            return 0xF51C
        elif value == 0x20:
            return 0xF51F
        elif value == 0x21:
            return 0xF520
        elif value == 0x23:
            return 0xF522
        elif value == 0x24:
            return 0xF523
        elif value == 0x25:
            return 0xF524
        elif value == 0x26:
            return 0xF525
        elif value == 0x27:
            return 0xF526
        elif value == 0x28:
            return 0xF527
        elif value in range(0x2A,0x2C):
            return 0xF529
        elif value in range(0x2D,0x2F):
            return 0xF52C
        elif value == 0x2F:
            return 0xF52E
        elif value == 0x30:
            return 0xF52F
        elif value == 0x31:
            return 0xF530
        elif value == 0x32:
            return 0xF531
        elif value == 0x34:
            return 0xF533
        elif value == 0x35:
            return 0xF534
        elif value == 0x36:
            return 0xF535
        elif value == 0x45:
            return 0xF544
        elif value == 0x55:
            return 0xF554
        elif value == 0x58:
            return 0xF557
        elif value == 0x5D:
            return 0xF55C
        elif value == 0x69:
            return 0xF568
        elif value == 0x6A:
            return 0xF569
        elif value == 0x6D:
            return 0xF56C
        elif value in range(0x7F,0x86):
            return 0xF57E
    if type == 0xF2:  
        if value in range(0xA0,0xA6): 
            return  0xF2A0
    

def main(): 
 
    if sys.platform == "win32":
        port = "COM3"
    elif sys.platform == "darwin":
        port = "/dev/tty.usbserial"
    else:
        port = "/dev/ttyUSB0"

    parser = argparse.ArgumentParser(
        description=("Stcflash, a command line programmer for "
                     + "STC 8051 microcontroller.\n"
                     + "https://github.com/laborer/stcflash"))
    parser.add_argument("image",
                        help="code image (bin/hex)",
                        type=argparse.FileType("rb"), nargs='?')
    parser.add_argument("-p", "--port",
                        help="serial port device (default: %s)" % port,
                        default=port)
    parser.add_argument("-l", "--lowbaud",
                        help="initial baud rate (default: 2400)",
                        type=int,
                        default=2400)
    parser.add_argument("-hb", "--highbaud",
                        help="initial baud rate (default: 115200)",
                        type=int,
                        default=115200)
    parser.add_argument("-r", "--protocol",
                        help="protocol to use for programming",
                        choices=["89", "12c5a", "12c52", "12cx052", "8", "15", "auto"],
                        default="auto")
    parser.add_argument("-a", "--aispbaud",
                        help="baud rate for AutoISP (default: 4800)",
                        type=int,
                        default=4800)
    parser.add_argument("-m", "--aispmagic",
                        help="magic word for AutoISP")
    parser.add_argument("-v", "--verbose",
                        help="be verbose",
                        default=0,
                        action="count")
    parser.add_argument("-e", "--erase_eeprom",
                        help=("erase data eeprom during next download"
                              +"(experimental)"),
                        action="store_true")
    parser.add_argument("-ne", "--not_erase_eeprom",
                        help=("do not erase data eeprom next download"
                              +"(experimental)"),
                        action="store_true")

    opts = parser.parse_args()

    opts.loglevel = (logging.CRITICAL,
                     logging.INFO,
                     logging.DEBUG)[min(2, opts.verbose)]

    opts.protocol = {'89': PROTOCOL_89,
                     '12c5a': PROTOCOL_12C5A,
                     '12c52': PROTOCOL_12C52,
                     '12cx052': PROTOCOL_12Cx052,
                     '8': PROTOCOL_8,  
                     '15': PROTOCOL_15, 
                     'auto': None}[opts.protocol]

    if not opts.erase_eeprom and not opts.not_erase_eeprom:
        opts.erase_eeprom = None

    logging.basicConfig(format=("%(levelname)s: "
                                + "[%(relativeCreated)d] "
                                + "%(message)s"),
                        level=opts.loglevel)

    if opts.image:
        code = bytearray(opts.image.read())
        opts.image.close()
        if os.path.splitext(opts.image.name)[1] in (".hex", ".ihx"):
            code = hex2bin(code)
    else:
        code = None

    print("通信端口：%s  最低波特率：%d bps" % (opts.port, opts.lowbaud))
    
    global highbaud_pre
    highbaud_pre = opts.highbaud
    with serial.Serial(port=opts.port,
                       baudrate=opts.lowbaud,
                       parity=serial.PARITY_NONE) as conn:
        if opts.aispmagic:
            autoisp(conn, opts.aispbaud, opts.aispmagic)
        program(Programmer(conn, opts.protocol), code, opts.erase_eeprom)


if __name__ == "__main__":
    main()
