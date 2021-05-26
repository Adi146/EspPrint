import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID
from esphome.components import Filesystem

AUTO_LOAD = ["Filesystem"]
CONFLICTS_WITH = ["SD"]

SD_MMC_FilesystemComponent = Filesystem.filesystem_ns.class_("SD_MMC_Filesystem", cg.Component)

CONFIG_SCHEMA = (
  cv.Schema(
    {
      cv.GenerateID(CONF_ID): cv.declare_id(SD_MMC_FilesystemComponent),
    }
  )
  .extend(cv.COMPONENT_SCHEMA)
)


def to_code(config):
  var = cg.new_Pvariable(config[CONF_ID])
  yield cg.register_component(var, config)