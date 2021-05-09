#!/usr/bin/python3

import re
import os
import requests
import sys
import logging
import threading

expected_gcodes = []
REGEX_PATTERN = "^\[.*\]:\sSEND:\sN(\d+)\s(.+)\*(\d+)$"

def setup_logger(name, log_file, format , level=logging.INFO):
    formatter = logging.Formatter(format)

    fileHandler = logging.FileHandler(log_file, mode='w')        
    fileHandler.setFormatter(formatter)

    streamHandler = logging.StreamHandler()
    streamHandler.setFormatter(formatter)

    logger = logging.getLogger(name)
    logger.setLevel(level)
    logger.addHandler(fileHandler)
    logger.addHandler(streamHandler)

    return logger


def uploadFile():
  files = {
    'file': ('integration.gcode', open(sys.argv[1], 'rb'), 'file/application'),
  }
  requests.post('http://espprint_dev/api/files/local', files=files, data={'print': 'true'})

if __name__ == "__main__":
  log = setup_logger(name='main', log_file='send_check.log', format='[%(asctime)s][%(levelname)s]: %(message)s')
  espprint_log = setup_logger(name='espprint', log_file='espprint.log', format='%(message)s')

  uploadThread = threading.Thread(target=uploadFile)
  uploadThread.start()

  with open(sys.argv[1]) as file:
    expected_gcodes = file.readlines()

  expected_gcodes = [gcode.strip().split(";", 1)[0] for gcode in expected_gcodes]
  expected_gcodes = [gcode for gcode in expected_gcodes if gcode]

  for line in sys.stdin:
    espprint_log.info(line.strip())
  
    match = re.match(REGEX_PATTERN, line)
    if match:
      lineNumber = int(match.group(1))
      gcode = match.group(2)

      if expected_gcodes[0] != gcode:
        log.warning("send unexpected gcode: " + gcode + " expected gcode: " + expected_gcodes[0])
      else:
        expected_gcodes = expected_gcodes[1:]
