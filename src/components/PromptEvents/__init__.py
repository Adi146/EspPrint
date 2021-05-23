import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor, GCodeQueue
from esphome.const import (
  CONF_ID,
)

DEPENDENCIES = ["GCodeQueue"]

action_ns = cg.global_ns.namespace("action")
PromptEventsComponent = action_ns.class_("PromptEvents", cg.Component)

CONF_PRINTER_NAME = "printer_name"

CONFIG_SCHEMA = (
  cv.Schema(
    {
      cv.GenerateID(CONF_ID): cv.declare_id(PromptEventsComponent),
      cv.Optional(CONF_PRINTER_NAME): cv.string,
    }
  )
  .extend(GCodeQueue.GCODE_ANALYZER_SCHEMA)
  .extend(cv.COMPONENT_SCHEMA)
)

def to_code(config):
  var = cg.new_Pvariable(config[CONF_ID])
  if CONF_PRINTER_NAME in config:
    cg.add(var.setPrinterName(config[CONF_PRINTER_NAME]))

  yield cg.register_component(var, config)
  yield GCodeQueue.register_gcode_analyzer(var, config)
  cg.add_library("ArduinoJson-esphomelib", "6.15.2")