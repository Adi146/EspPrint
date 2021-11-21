import esphome.codegen as cg

filesystem_ns = cg.global_ns.namespace("storage::filesystem")

def to_code(config):
  cg.add_library("FS", None)