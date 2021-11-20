import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import climate, GCodeQueue, GCodeSender
from esphome.const import (
  CONF_ID,
)

DEPENDENCIES = ["GCodeQueue", "GCodeSender"]

GCodeClimate = GCodeQueue.analyzer_ns.class_("GCodeClimate", cg.Component, climate.Climate)
ClimateToken = GCodeQueue.analyzer_ns.enum("ClimateToken")

CONF_HEATER_ELEMENTS = "heater_element"
CONF_TOOL_INDEX = "tool_index"

HEATER_ELEMENTS = {
  "TOOL": ClimateToken.TOOL,
  "BED": ClimateToken.BED,
  "CHAMBER": ClimateToken.CHAMBER,
}

CONFIG_SCHEMA = (
  climate.CLIMATE_SCHEMA
  .extend(
    {
      cv.GenerateID(CONF_ID): cv.declare_id(GCodeClimate),
      cv.GenerateID(GCodeSender.CONF_GCODE_SENDER): cv.use_id(GCodeSender.GCodeSenderComponent),
      cv.Required(CONF_HEATER_ELEMENTS): cv.enum(
        HEATER_ELEMENTS, upper=True
      ),
      cv.Optional(CONF_TOOL_INDEX, default=0): cv.positive_int,
    }
  )
  .extend(GCodeQueue.GCODE_ANALYZER_SCHEMA)
  .extend(cv.COMPONENT_SCHEMA)
)

def to_code(config):
  sender = yield cg.get_variable(config[GCodeSender.CONF_GCODE_SENDER])

  var = cg.new_Pvariable(config[CONF_ID], sender, config[CONF_HEATER_ELEMENTS], config[CONF_TOOL_INDEX])
  yield cg.register_component(var, config)
  yield GCodeQueue.register_gcode_analyzer(var, config)
  yield climate.register_climate(var, config)