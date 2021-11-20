import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID
from esphome.core import coroutine

analyzer_ns = cg.global_ns.namespace("core::analyzer")

CONF_GCODE_QUEUE = "gcode_queue"

GCodeQueueComponent = analyzer_ns.class_("GCodeQueue", cg.Component)

CONFIG_SCHEMA = cv.Schema(
  {
    cv.GenerateID(CONF_ID): cv.declare_id(GCodeQueueComponent),
  }
).extend(cv.COMPONENT_SCHEMA)

GCODE_ANALYZER_SCHEMA = cv.Schema(
  {
    cv.GenerateID(CONF_GCODE_QUEUE): cv.use_id(GCodeQueueComponent),
  }
)

def to_code(config):
  var = cg.new_Pvariable(config[CONF_ID])
  yield cg.register_component(var, config)

@coroutine
def register_gcode_analyzer(var, config):
  queue = yield cg.get_variable(config[CONF_GCODE_QUEUE])
  cg.add(queue.addSensor(var))