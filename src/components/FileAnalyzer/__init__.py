import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import (
  CONF_ID,
)

storage_ns = cg.global_ns.namespace("storage")
FileAnalyzerComponent = storage_ns.class_("FileAnalyzer", cg.Component)

CONF_FILE_ANALYZER_ID = "file_analyzer_id"

CONFIG_SCHEMA = (
  cv.Schema(
    {
      cv.GenerateID(CONF_ID): cv.declare_id(FileAnalyzerComponent),
    }
  )
  .extend(cv.COMPONENT_SCHEMA)
)

def to_code(config):
  var = cg.new_Pvariable(config[CONF_ID])
  yield cg.register_component(var, config)
