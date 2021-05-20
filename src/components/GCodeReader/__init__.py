import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID
from esphome.components import uart
from esphome.components.GCodeQueue import CONF_GCODE_QUEUE, GCodeQueueComponent
from esphome.components.GCodeSender import CONF_GCODE_SENDER, GCodeSenderComponent

DEPENDENCIES = ["uart", "GCodeSender", "GCodeQueue"]

communication_ns = cg.global_ns.namespace("core::communication")

GCodeReaderComponent = communication_ns.class_("GCodeReader", cg.Component)

CONFIG_SCHEMA = cv.Schema(
  {
    cv.GenerateID(CONF_ID): cv.declare_id(GCodeReaderComponent),
    cv.GenerateID(CONF_GCODE_QUEUE): cv.use_id(GCodeQueueComponent),
    cv.GenerateID(CONF_GCODE_SENDER): cv.use_id(GCodeSenderComponent),
  }
).extend(uart.UART_DEVICE_SCHEMA)

def to_code(config):
  sender = yield cg.get_variable(config[CONF_GCODE_SENDER])
  queue = yield cg.get_variable(config[CONF_GCODE_QUEUE])

  var = cg.new_Pvariable(config[CONF_ID], sender, queue)
  yield cg.register_component(var, config)
  yield uart.register_uart_device(var, config)