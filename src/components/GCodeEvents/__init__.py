import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID
from esphome.components.GCodeQueue import CONF_GCODE_QUEUE, GCodeQueueComponent

DEPENDENCIES = ["GCodeQueue"]

analyzer_ns = cg.global_ns.namespace("core::analyzer")

GCodeEventsComponent = analyzer_ns.class_("GCodeEvents", cg.Component)

CONFIG_SCHEMA = cv.Schema(
  {
    cv.GenerateID(CONF_ID): cv.declare_id(GCodeEventsComponent),
    cv.GenerateID(CONF_GCODE_QUEUE): cv.use_id(GCodeQueueComponent),
  }
).extend(cv.COMPONENT_SCHEMA)

def to_code(config):
  queue = yield cg.get_variable(config[CONF_GCODE_QUEUE])

  var = cg.new_Pvariable(config[CONF_ID])
  yield cg.register_component(var, config)

  cg.add(queue.addSensor(var))