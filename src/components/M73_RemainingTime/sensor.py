import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.components.GCodeQueue import CONF_GCODE_QUEUE, GCodeQueueComponent
from esphome.const import (
  CONF_ID,
  DEVICE_CLASS_EMPTY,
  ICON_TIMELAPSE,
  UNIT_MINUTE,
)

DEPENDENCIES = ["GCodeQueue"]

analyzer_ns = cg.global_ns.namespace("core::analyzer")

M73_RemainingTime = analyzer_ns.class_("M73_RemainingTime", cg.Component, sensor.Sensor)

CONFIG_SCHEMA = cv.Schema(
  sensor.sensor_schema(UNIT_MINUTE, ICON_TIMELAPSE, 0, DEVICE_CLASS_EMPTY)
  .extend(
    {
      cv.GenerateID(CONF_ID): cv.declare_id(M73_RemainingTime),
      cv.GenerateID(CONF_GCODE_QUEUE): cv.use_id(GCodeQueueComponent),
    }
  ).extend(cv.COMPONENT_SCHEMA)
)

def to_code(config):
  queue = yield cg.get_variable(config[CONF_GCODE_QUEUE])

  var = cg.new_Pvariable(config[CONF_ID])
  yield cg.register_component(var, config)
  yield sensor.register_sensor(var, config)

  cg.add(queue.addSensor(var))