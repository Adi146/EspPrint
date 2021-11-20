import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor, M73_Base
from esphome.const import (
  CONF_ID,
  DEVICE_CLASS_EMPTY,
  ICON_PERCENT,
  UNIT_PERCENT,
)

AUTO_LOAD = ["M73_Base"]

CONFIG_SCHEMA = M73_Base.BASE_SCHEMA

CONFIG_SCHEMA = (
  sensor.sensor_schema(UNIT_PERCENT, ICON_PERCENT, 0, DEVICE_CLASS_EMPTY)
  .extend(M73_Base.BASE_SCHEMA)
)

def to_code(config):
  yield M73_Base.new_M73_sensor(config, 1)