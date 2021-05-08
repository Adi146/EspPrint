#!/usr/bin/python3

import re
import sys
import logging

expected_gcodes = []
REGEX_PATTERN = "^\[.*\]:\sSEND:\sN(\d+)\s(.+)\*(\d+)$"

if __name__ == "__main__":
  with open(sys.argv[1]) as file:
    expected_gcodes = file.readlines()

  expected_gcodes = [gcode.strip().split(";", 1)[0] for gcode in expected_gcodes]
  expected_gcodes = [gcode for gcode in expected_gcodes if gcode]

  for line in sys.stdin:
    print(line, end='')
  
    match = re.match(REGEX_PATTERN, line)
    if match:
      lineNumber = int(match.group(1))
      gcode = match.group(2)

      if expected_gcodes[0] != gcode:
        logging.warning("send unexpected gcode: " + gcode + " expected gcode: " + expected_gcodes[0])
      else:
        expected_gcodes = expected_gcodes[1:]
    
