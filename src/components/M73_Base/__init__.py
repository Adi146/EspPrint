import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.components.GCodeQueue import CONF_GCODE_QUEUE, GCodeQueueComponent
from esphome.const import (
  CONF_ID,
  DEVICE_CLASS_EMPTY,
  ICON_PERCENT,
  UNIT_PERCENT,
)
from esphome.core import coroutine

DEPENDENCIES = ["GCodeQueue"]

analyzer_ns = cg.global_ns.namespace("core::analyzer")

M73_Sensor = analyzer_ns.class_("M73_Sensor", cg.Component, sensor.Sensor)

BASE_SCHEMA = cv.Schema(
  {
    cv.GenerateID(CONF_ID): cv.declare_id(M73_Sensor),
    cv.GenerateID(CONF_GCODE_QUEUE): cv.use_id(GCodeQueueComponent),
  }
).extend(cv.COMPONENT_SCHEMA)

BASE_SCHEMA = cv.Schema(
  {
      cv.GenerateID(CONF_ID): cv.declare_id(M73_Sensor),
      cv.GenerateID(CONF_GCODE_QUEUE): cv.use_id(GCodeQueueComponent),
  }
).extend(cv.COMPONENT_SCHEMA)

@coroutine
def new_M73_sensor(config, regexGroup):
  queue = yield cg.get_variable(config[CONF_GCODE_QUEUE])

  var = cg.new_Pvariable(config[CONF_ID], regexGroup)
  yield cg.register_component(var, config)
  yield sensor.register_sensor(var, config)

  cg.add(queue.addSensor(var))