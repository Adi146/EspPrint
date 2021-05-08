#!/usr/bin/python3

import re
import os
import requests
import sys
import logging
import threading

expected_gcodes = []
REGEX_PATTERN = "^\[.*\]:\sSEND:\sN(\d+)\s(.+)\*(\d+)$"

def uploadFile():
  files = {
    'file': ('integration.gcode', open(sys.argv[1], 'rb'), 'file/application'),
  }
  requests.post('http://espprint_dev/api/files/local', files=files, data={'print': 'true'})

if __name__ == "__main__":
  logging.basicConfig(filename='send_check.log', filemode='w', format='%(name)s - %(levelname)s - %(message)s')

  uploadThread = threading.Thread(target=uploadFile)
  uploadThread.start()

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
