import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID
from esphome.components import uart
from esphome.components.GCodeQueue import CONF_GCODE_QUEUE, GCodeQueueComponent

DEPENDENCIES = ["uart", "GCodeQueue"]

CONF_GCODE_SENDER = "gcode_sender"
CONF_RESEND_BUFFER_SIZE = "resend_buffer_size"

communication_ns = cg.global_ns.namespace("core::communication")

GCodeSenderComponent = communication_ns.class_("GCodeSender", cg.Component)

CONFIG_SCHEMA = cv.Schema(
  {
    cv.GenerateID(CONF_ID): cv.declare_id(GCodeSenderComponent),
    cv.GenerateID(CONF_GCODE_QUEUE): cv.use_id(GCodeQueueComponent),
    cv.Optional(CONF_RESEND_BUFFER_SIZE): cv.positive_not_null_int,
  }
).extend(uart.UART_DEVICE_SCHEMA)

def to_code(config):
  queue = yield cg.get_variable(config[CONF_GCODE_QUEUE])

  var = cg.new_Pvariable(config[CONF_ID], queue)
  yield cg.register_component(var, config)
  yield uart.register_uart_device(var, config)

  if CONF_RESEND_BUFFER_SIZE in config:
    cg.add(var.setResendBufferSize(config[CONF_RESEND_BUFFER_SIZE]))