import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID
from esphome.components import GCodeQueue

DEPENDENCIES = ["GCodeQueue"]

GCodeEventsComponent = GCodeQueue.analyzer_ns.class_("GCodeEvents", cg.PollingComponent)

CONFIG_SCHEMA = (
  cv.Schema(
    {
      cv.GenerateID(CONF_ID): cv.declare_id(GCodeEventsComponent),
    }
  )
  .extend(GCodeQueue.GCODE_ANALYZER_SCHEMA)
  .extend(cv.polling_component_schema("1s"))
)


def to_code(config):
  var = cg.new_Pvariable(config[CONF_ID])
  yield cg.register_component(var, config)

  yield GCodeQueue.register_gcode_analyzer(var, config)
  cg.add_library("ArduinoJson-esphomelib", "6.15.2")