#!/usr/bin/env python3

from random import randint
import sys
import argparse

time = 1
streams = 10
count = 1000

parser = argparse.ArgumentParser()
parser.add_argument("-s", "--streams", type=int, help="number of streams")
parser.add_argument("-c", "--count", type=int, help="count of ts/v pairs")
args = parser.parse_args()

if args.streams:
    streams = args.streams

if args.count:
    count = args.count

print("#define number_inputs (sizeof (stream_input) / sizeof (const char *))")
print("const char *stream_input[] = {\\")

for i in range (0,count):
    time = time + randint(0,streams-1)
    print("\"" + str(randint(0,streams-1)) + "\\t" + str(time) + "\\t" + str(randint(0,99))+ "\"" +"," +"\\")

#time = time + randint(0,9)
#print("\"" + str(randint(0,9)) + "\\t" + str(time) + "\\t" + str(randint(0,99))+ "\"" + "};")

for i in range (0,streams-1):
    print("\"" + str(i) + "\\t" + str(0) + "\\t" + str(0) + "\"" +"," +"\\")

print("\"" + str(streams-1) + "\\t" + str(0) + "\\t" + str(0)+ "\"" + "};")

