import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import (
  web_server_base,
  GCodeFileReader,
  FileAnalyzer,
)
from esphome.const import (
  CONF_ID,
)

DEPENDENCIES = ["GCodeFileReader", "FileAnalyzer"]
AUTO_LOAD = ["web_server_base"]

storage_ns = cg.global_ns.namespace("storage")
UploadServerComponent = storage_ns.class_("UploadServer", cg.Component)

CONFIG_SCHEMA = (
  cv.Schema(
    {
      cv.GenerateID(CONF_ID): cv.declare_id(UploadServerComponent),
      cv.GenerateID(web_server_base.CONF_WEB_SERVER_BASE_ID): cv.use_id(web_server_base.WebServerBase),
      cv.GenerateID(GCodeFileReader.CONF_GCODE_FILE_READER_ID): cv.use_id(GCodeFileReader.GCodeFileReaderComponent),
      cv.GenerateID(FileAnalyzer.CONF_FILE_ANALYZER_ID): cv.use_id(FileAnalyzer.FileAnalyzerComponent),
    }
  )
  .extend(cv.COMPONENT_SCHEMA)
)

def to_code(config):
  server = yield cg.get_variable(config[web_server_base.CONF_WEB_SERVER_BASE_ID])
  reader = yield cg.get_variable(config[GCodeFileReader.CONF_GCODE_FILE_READER_ID])
  analyzer = yield cg.get_variable(config[FileAnalyzer.CONF_FILE_ANALYZER_ID])

  var = cg.new_Pvariable(config[CONF_ID], server, reader, analyzer)
  yield cg.register_component(var, config)