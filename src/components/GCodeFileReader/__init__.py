import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import GCodeSender
from esphome.const import (
  CONF_ID,
)

DEPENDENCIES = ["GCodeSender"]

storage_ns = cg.global_ns.namespace("storage")
GCodeReaderComponent = storage_ns.class_("GCodeFileReader", cg.Component)

CONF_CANCEL_GCODES = "cancel_gcodes"

CONFIG_SCHEMA = (
  cv.Schema(
    {
      cv.GenerateID(CONF_ID): cv.declare_id(GCodeReaderComponent),
      cv.GenerateID(GCodeSender.CONF_GCODE_SENDER): cv.use_id(GCodeSender.GCodeSenderComponent),
      cv.Required(CONF_CANCEL_GCODES): cv.All([str], cv.Length(min=1)),
    }
  )
  .extend(cv.COMPONENT_SCHEMA)
)

def to_code(config):
  sender = yield cg.get_variable(config[GCodeSender.CONF_GCODE_SENDER])

  var = cg.new_Pvariable(config[CONF_ID], sender, config[CONF_CANCEL_GCODES])
  yield cg.register_component(var, config)
