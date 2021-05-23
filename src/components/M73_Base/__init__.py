import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor, GCodeQueue
from esphome.components.GCodeQueue import CONF_GCODE_QUEUE, GCodeQueueComponent
from esphome.const import (
  CONF_ID,
  DEVICE_CLASS_EMPTY,
  ICON_PERCENT,
  UNIT_PERCENT,
)
from esphome.core import coroutine

DEPENDENCIES = ["GCodeQueue"]

M73_Sensor = GCodeQueue.analyzer_ns.class_("M73_Sensor", cg.Component, sensor.Sensor)

BASE_SCHEMA = (
  cv.Schema(
    {
      cv.GenerateID(CONF_ID): cv.declare_id(M73_Sensor),
      cv.GenerateID(CONF_GCODE_QUEUE): cv.use_id(GCodeQueueComponent),
    }
  )
  .extend(GCodeQueue.GCODE_ANALYZER_SCHEMA)
  .extend(cv.COMPONENT_SCHEMA)
)

BASE_SCHEMA = cv.Schema(
  {
      cv.GenerateID(CONF_ID): cv.declare_id(M73_Sensor),
      cv.GenerateID(CONF_GCODE_QUEUE): cv.use_id(GCodeQueueComponent),
  }
).extend(cv.COMPONENT_SCHEMA)

@coroutine
def new_M73_sensor(config, regexGroup):
  var = cg.new_Pvariable(config[CONF_ID], regexGroup)
  yield cg.register_component(var, config)
  yield sensor.register_sensor(var, config)
  yield GCodeQueue.register_gcode_analyzer(var, config)