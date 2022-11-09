import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import lcd_base, i2c
from esphome.const import CONF_ID, CONF_LAMBDA

DEPENDENCIES = ["i2c"]
AUTO_LOAD = ["lcd_base"]

AiP31068L_ns = cg.esphome_ns.namespace("AiP31068L")
AiP31068LLCDDisplay = AiP31068L_ns.class_(
    "AiP31068LLCDDisplay", lcd_base.LCDDisplay, i2c.I2CDevice
)

CONFIG_SCHEMA = lcd_base.LCD_SCHEMA.extend(
    {
        cv.GenerateID(): cv.declare_id(AiP31068LLCDDisplay),
    }
).extend(i2c.i2c_device_schema(0x3E))


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await lcd_base.setup_lcd_display(var, config)
    await i2c.register_i2c_device(var, config)

    if CONF_LAMBDA in config:
        lambda_ = await cg.process_lambda(
            config[CONF_LAMBDA],
            [(AiP31068LLCDDisplay.operator("ref"), "it")],
            return_type=cg.void,
        )
        cg.add(var.set_writer(lambda_))
