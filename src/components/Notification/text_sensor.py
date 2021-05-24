import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import text_sensor, GCodeQueue
from esphome.const import (
  CONF_ID,
)

DEPENDENCIES = ["GCodeQueue"]

action_ns = cg.global_ns.namespace("action")
NotificationSensor = action_ns.class_("Notification", cg.Component, text_sensor.TextSensor)

CONFIG_SCHEMA = (
  text_sensor.TEXT_SENSOR_SCHEMA
  .extend(
    {
      cv.GenerateID(CONF_ID): cv.declare_id(NotificationSensor),
    }
  )
  .extend(GCodeQueue.GCODE_ANALYZER_SCHEMA)
  .extend(cv.COMPONENT_SCHEMA)
)

def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    yield cg.register_component(var, config)
    yield GCodeQueue.register_gcode_analyzer(var, config)
    yield text_sensor.register_text_sensor(var, config)