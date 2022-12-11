import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import pins
from esphome.components import display, light
from esphome.const import (
    CONF_COLOR_PALETTE,
    CONF_DC_PIN,
    CONF_ID,
    CONF_LAMBDA,
    CONF_MODEL,
    CONF_RESET_PIN,
    CONF_CS_PIN,
    CONF_MISO_PIN,
    CONF_MOSI_PIN,
    CONF_CLK_PIN,
    CONF_PAGES,
    CONF_ROTATION,
    CONF_DIMENSIONS,
    PLATFORM_ESP8266,
    PLATFORM_RP2040
)
from esphome.core import HexInt
from esphome.automation import LambdaAction
from esphome.core import CORE, EsphomeError, Lambda, coroutine_with_priority
from esphome.components.esp32 import add_idf_sdkconfig_option, get_esp32_variant
from esphome.components.esp32.const import (
    VARIANT_ESP32,
    VARIANT_ESP32S2,
    VARIANT_ESP32C3,
    VARIANT_ESP32S3,
)
from esphome.schema_extractors import (
    schema_extractor_typed,
)

CONF_PAR_8 = "8BIT_PARALEL"
CONF_PAR_16 = "16BIT_PARALEL"
CONF_LCD_8 = "8BIT_LCD_MODE"
CONF_LCD_16 = "16BIT_LCD_MODE"
CONF_SPI = "SPI"
CONF_DATABUS = "databus"
CONF_BACKLIGHT = "backlight"
CONF_WR_PIN = "WR_pin"
CONF_D0_PIN = "D0_pin"
CONF_D1_PIN = "D1_pin"
CONF_D2_PIN = "D2_pin"
CONF_D3_PIN = "D3_pin"
CONF_D4_PIN = "D4_pin"
CONF_D5_PIN = "D5_pin"
CONF_D6_PIN = "D6_pin"
CONF_D7_PIN = "D7_pin"
CONF_D8_PIN = "D8_pin"
CONF_D9_PIN = "D9_pin"
CONF_D10_PIN = "D10_pin"
CONF_D11_PIN = "D11_pin"
CONF_D12_PIN = "D12_pin"
CONF_D13_PIN = "D13_pin"
CONF_D14_PIN = "D14_pin"
CONF_D15_PIN = "D15_pin"
CONF_IS_IPS_DISPLAY = "is_ips_display"

CODEOWNERS = ["@nielsnl68"]

GRAPHICS_ns = cg.esphome_ns.namespace("Graphics")
GraphicDisplay = GRAPHICS_ns.class_(
    "GraphicDisplay", cg.PollingComponent, display.DisplayBuffer
)

# Arduino_GC9A01(*bus, rst, r, ips [, w, h]);
# Arduino_GC9106(*bus, rst, r, ips [, w, h]);
# Arduino_GC9107(*bus, rst, r, ips [, w, h]);
# Arduino_HX8347C(*bus, rst, r, ips [, w, h]);
# Arduino_HX8347D(*bus, rst, r, ips [, w, h]);
# Arduino_HX8352C(*bus, rst, r, ips [, w, h]);
# Arduino_HX8357A(*bus, rst, r, ips);
# Arduino_HX8357B(*bus, rst, r, ips);
# Arduino_HX8369A(*bus, rst, r, ips [, w, h]);
# Arduino_ILI9225(*bus, rst, r)
# Arduino_ILI9331(*bus, rst, r, ips);
# Arduino_ILI9341(*bus, rst, r, ips);
# Arduino_ILI9342(*bus, rst, r, ips);
# Arduino_ILI9481(*bus, rst, r, ips);
# Arduino_ILI9486(*bus, rst, r, ips);
# Arduino_ILI9488(*bus, rst, r, ips);
# Arduino_ILI9806(*bus, rst, r, ips);
# Arduino_JBT6K71(*bus, rst, r, ips [, w, h]);
# Arduino_NT35310(*bus, rst, r, ips [, w, h]);
# Arduino_NT35510(*bus, rst, r, ips [, w, h]);
# Arduino_NT39125(*bus, rst, r, ips [, w, h]);
# Arduino_NV3041A(*bus, rst, r, ips [, w, h]);
# Arduino_R61529(*bus, rst, r, ips);
# Arduino_SEPS525(*bus, rst, r, [, w, h]);
# Arduino_SSD1283A(*bus, rst, r, [, w, h]);
# Arduino_SSD1331(*bus, rst, r, [, w, h]);
# Arduino_SSD1351(*bus, rst, r, [, w, h]);
# Arduino_ST7735(*bus, rst, r, ips [, w, h]);
# Arduino_ST7789(*bus, rst, r, ips [, w, h]);
# Arduino_ST7796(*bus, rst, r, ips [, w, h]);



MODELS = {
    "GC9106": [cg.global_ns.class_("Arduino_GC9106"),0],
    "GC9107": [cg.global_ns.class_("Arduino_GC9107"),0],
    "GC9A01": [cg.global_ns.class_("Arduino_GC9A01"),0],
    "HX8347C": [cg.global_ns.class_("Arduino_HX8347C"),0],
    "HX8347D": [cg.global_ns.class_("Arduino_HX8347D"),0],
    "HX8352C": [cg.global_ns.class_("Arduino_HX8352C"),0],
    "HX8357A": [cg.global_ns.class_("Arduino_HX8357A"),1],
    "HX8357B": [cg.global_ns.class_("Arduino_HX8357B"),1],
    "HX8369A": [cg.global_ns.class_("Arduino_HX8369A"),0],
    "ILI9225": [cg.global_ns.class_("Arduino_ILI9225"),3],
    "ILI9331": [cg.global_ns.class_("Arduino_ILI9331"),1],
    "ILI9341": [cg.global_ns.class_("Arduino_ILI9341"),1],
    "ILI9342": [cg.global_ns.class_("Arduino_ILI9342"),1],
    "ILI9481": [cg.global_ns.class_("Arduino_ILI9481_18bit"),1],
    "ILI9486": [cg.global_ns.class_("Arduino_ILI9486"),1],
    "ILI9486_18bit": [cg.global_ns.class_("Arduino_ILI9486_18bit"),1],
    "18bit_ILI9486": [cg.global_ns.class_("Arduino_ILI9486_18bit"),1],
    "ILI9488": [cg.global_ns.class_("Arduino_ILI9488"),1],
    "ILI9488_18bit": [cg.global_ns.class_("Arduino_ILI9488_18bit"),1],
    "18bit_ILI9488": [cg.global_ns.class_("Arduino_ILI9488_18bit"),1],
    "ILI9806": [cg.global_ns.class_("Arduino_ILI9806"),1],
    "JBT6K71": [cg.global_ns.class_("Arduino_JBT6K71"),0],
    "NT35310": [cg.global_ns.class_("Arduino_NT35310"),0],
    "NT35510": [cg.global_ns.class_("Arduino_NT35510"),0],
    "NT39125": [cg.global_ns.class_("Arduino_NT39125"),0],
    "NV3041A": [cg.global_ns.class_("Arduino_NV3041A"),0],
    "R61529": [cg.global_ns.class_("Arduino_R61529"),1],
    "SEPS525": [cg.global_ns.class_("Arduino_SEPS525"),2],
    "SSD1283A": [cg.global_ns.class_("Arduino_SSD1283A"),2],
    "SSD1331": [cg.global_ns.class_("Arduino_SSD1331"),2],
    "SSD1351": [cg.global_ns.class_("Arduino_SSD1351"),2],
    "ST7735": [cg.global_ns.class_("Arduino_ST7735"),0],
    "ST7789": [cg.global_ns.class_("Arduino_ST7789"),0],
    "ST7796": [cg.global_ns.class_("Arduino_ST7796"),0],
}

DATABUSES = {
    PLATFORM_ESP8266: {
        CONF_SPI:    [cg.global_ns.class_("Arduino_ESP32SPI"),0],
    },
    VARIANT_ESP32: {
        CONF_SPI:    [cg.global_ns.class_("Arduino_ESP32SPI"),1], 
        CONF_PAR_8:  [cg.global_ns.class_("Arduino_ESP32PAR8"),2], 
        CONF_PAR_16: [cg.global_ns.class_("Arduino_ESP32PAR16"),3],
    },
    VARIANT_ESP32S2: {
        CONF_SPI:    [cg.global_ns.class_("Arduino_ESP32SPI"),1], 
        CONF_PAR_8:  [cg.global_ns.class_("Arduino_ESP32S2PAR8"),4], 
        CONF_PAR_16: [cg.global_ns.class_("Arduino_ESP32S2PAR16"),4],
    },
    VARIANT_ESP32S3: {
        CONF_SPI:    [cg.global_ns.class_("Arduino_ESP32SPI"),1], 
        CONF_PAR_8:  [cg.global_ns.class_("Arduino_ESP32LCD8"),2], 
        CONF_PAR_16: [cg.global_ns.class_("Arduino_ESP32LCD16"),2],
    },
    VARIANT_ESP32C3: {
        CONF_SPI:    [cg.global_ns.class_("Arduino_ESP32SPI"),1], 
        CONF_PAR_8:  [cg.global_ns.class_("Arduino_SWPAR8"),2], 
    },
    PLATFORM_RP2040: {
        CONF_SPI:    [cg.global_ns.class_("Arduino_RPiPicoSPI"),5], 
        CONF_PAR_8:  [cg.global_ns.class_("Arduino_RPiPicoPAR8"),4], 
        CONF_PAR_16: [cg.global_ns.class_("Arduino_RPiPicoPAR16"),4],
    }
}

@schema_extractor_typed
def databus_schema(schemas, **kwargs):
    """Create a schema that has a key to distinguish between schemas"""
    key = kwargs.pop("key", CONF_DATABUS)
    default_schema_option = kwargs.pop("default", None)

    if CORE.is_esp32:
        variant = get_esp32_variant()
        if variant in DATABUSES:
            key_validator = cv.enum(DATABUSES[variant], space="_", upper=True)
        else:
            raise NotImplementedError
    elif CORE.is_esp8266:
        key_validator =  cv.enum(DATABUSES[PLATFORM_ESP8266], space="_", upper=True)
    elif CORE.is_rp2040:
        key_validator =  cv.enum(DATABUSES[PLATFORM_RP2040], space="_", upper=True)
    else:
        raise NotImplementedError

    def validator(value):
        if not isinstance(value, dict):
            raise cv.Invalid("Value must be dict")
        value = value.copy()
        schema_option = value.pop(key, default_schema_option)
        if schema_option is None:
            raise cv.Invalid(f"{key} not specified!")
        key_v = key_validator(schema_option)
        value = cv.Schema(schemas[key_v])(value)
        value[key] = key_v
        return value

    return validator

DISPLAY_ROTATIONS = {
    0: 0,
    90: 1,
    180: 2,
    270: 3,
}


def validate_rotation(value):
    value = cv.string(value)
    if value.endswith("°"):
        value = value[:-1]
    return cv.enum(DISPLAY_ROTATIONS, int=True)(value)

COMMON_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(GraphicDisplay),
    cv.Optional(CONF_ROTATION): validate_rotation,

    cv.Required(CONF_MODEL): cv.enum(MODELS, upper=True, space="_"),
    cv.Optional(CONF_CS_PIN, default= -1)  : pins.gpio_output_pin_schema, 
    cv.Optional(CONF_DC_PIN, default= -1): pins.gpio_output_pin_schema,
    cv.Optional(CONF_RESET_PIN, default= -1): pins.gpio_output_pin_schema,
    cv.Optional(CONF_BACKLIGHT): pins.gpio_output_pin_schema,
    cv.Optional(CONF_IS_IPS_DISPLAY, default=False):cv.boolean,
    cv.Optional(CONF_DIMENSIONS): cv.dimensions,
})


CONFIG_SCHEMA = cv.All(
    display.FULL_DISPLAY_SCHEMA.extend(
        databus_schema({
            CONF_SPI: COMMON_SCHEMA.extend({
                    cv.Optional(CONF_CLK_PIN, default= -1) : pins.gpio_output_pin_schema, 
                    cv.Optional(CONF_MOSI_PIN, default= -1): pins.gpio_output_pin_schema,
                    cv.Optional(CONF_MISO_PIN, default= -1): pins.gpio_output_pin_schema,
            }), 
            CONF_PAR_8: COMMON_SCHEMA.extend({
                    cv.Optional(CONF_WR_PIN, default= -1) : pins.gpio_output_pin_schema, 
                    cv.Optional(CONF_D0_PIN, default= -1) : pins.gpio_output_pin_schema, 
                    cv.Optional(CONF_D1_PIN, default= -1) : pins.gpio_output_pin_schema, 
                    cv.Optional(CONF_D2_PIN, default= -1) : pins.gpio_output_pin_schema, 
                    cv.Optional(CONF_D3_PIN, default= -1) : pins.gpio_output_pin_schema, 
                    cv.Optional(CONF_D4_PIN, default= -1) : pins.gpio_output_pin_schema, 
                    cv.Optional(CONF_D5_PIN, default= -1) : pins.gpio_output_pin_schema, 
                    cv.Optional(CONF_D6_PIN, default= -1) : pins.gpio_output_pin_schema, 
                    cv.Optional(CONF_D7_PIN, default= -1) : pins.gpio_output_pin_schema, 
            }), 
            CONF_PAR_16: COMMON_SCHEMA.extend({
                    cv.Optional(CONF_WR_PIN, default= -1) : pins.gpio_output_pin_schema, 
                    cv.Optional(CONF_D0_PIN, default= -1) : pins.gpio_output_pin_schema, 
                    cv.Optional(CONF_D1_PIN, default= -1) : pins.gpio_output_pin_schema, 
                    cv.Optional(CONF_D2_PIN, default= -1) : pins.gpio_output_pin_schema, 
                    cv.Optional(CONF_D3_PIN, default= -1) : pins.gpio_output_pin_schema, 
                    cv.Optional(CONF_D4_PIN, default= -1) : pins.gpio_output_pin_schema, 
                    cv.Optional(CONF_D5_PIN, default= -1) : pins.gpio_output_pin_schema, 
                    cv.Optional(CONF_D6_PIN, default= -1) : pins.gpio_output_pin_schema, 
                    cv.Optional(CONF_D7_PIN, default= -1) : pins.gpio_output_pin_schema, 
                    cv.Optional(CONF_D8_PIN, default= -1) : pins.gpio_output_pin_schema, 
                    cv.Optional(CONF_D9_PIN, default= -1) : pins.gpio_output_pin_schema, 
                    cv.Optional(CONF_D10_PIN, default= -1) : pins.gpio_output_pin_schema, 
                    cv.Optional(CONF_D11_PIN, default= -1) : pins.gpio_output_pin_schema, 
                    cv.Optional(CONF_D12_PIN, default= -1) : pins.gpio_output_pin_schema, 
                    cv.Optional(CONF_D13_PIN, default= -1) : pins.gpio_output_pin_schema, 
                    cv.Optional(CONF_D14_PIN, default= -1) : pins.gpio_output_pin_schema, 
                    cv.Optional(CONF_D15_PIN, default= -1) : pins.gpio_output_pin_schema, 
            }),
        }),
    ).extend(cv.polling_component_schema("1s")),
    cv.has_at_most_one_key(CONF_PAGES, CONF_LAMBDA),
)

@coroutine_with_priority(90.0)
async def to_code(config):
    print(config)
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    cg.add_library("moononournation/Arduino_GFX", "1.3.1")

    databus = config[CONF_DATABUS]
    match databus[1]:
        case 0:
            # Arduino_ESP8266SPI( dc,  cs); // Constructor
            bus = databus[0].new(config[CONF_DC_PIN], config[CONF_CS_PIN] )
        case 1:
            # Arduino_ESP32SPI( dc,  cs,  sck,  mosi,  miso, spi_num = VSPI, bool is_shared_interface = true); // Constructor
            bus = databus[0].new(config[CONF_DC_PIN], config[CONF_CS_PIN], config[CONF_CLK_PIN], 
                                 config[CONF_MOSI_PIN], config[CONF_MISO_PIN])
        case 2:
            # Arduino_ESP32PAR8( dc,  cs,  wr,  rd,  d0,  d1,  d2,  d3,  d4,  d5,  d6,  d7); // Constructor
            # Arduino_ESP32LCD8( dc,  cs,  wr,  rd, d0,  d1,  d2,  d3,  d4,  d5,  d6,  d7); // Constructor
            # Arduino_SWPAR8( dc,  cs,  wr,  rd, d0,  d1,  d2,  d3,  d4,  d5,  d6,  d7); // Constructor
            bus = databus[0].new(config[CONF_DC_PIN], config[CONF_CS_PIN], config[CONF_WR_PIN], -1, 
                                config[CONF_D0_PIN], config[CONF_D1_PIN], config[CONF_D2_PIN], config[CONF_D3_PIN],
                                config[CONF_D4_PIN], config[CONF_D5_PIN], config[CONF_D6_PIN], config[CONF_D7_PIN])
        case 3:
            # Arduino_ESP32PAR16(dc,  cs,  wr,  rd, d0,  d1,  d2,  d3,  d4,  d5,  d6,  d7,  d8,  d9,  d10,  d11,  d12,  d13,  d14,  d15);
            # Arduino_ESP32LCD16(dc,  cs,  wr,  rd, d0,  d1,  d2,  d3,  d4,  d5,  d6,  d7,  d8,  d9,  d10,  d11,  d12,  d13,  d14,  d15);
            bus = databus[0].new(config[CONF_DC_PIN], config[CONF_CS_PIN], config[CONF_WR_PIN], -1, 
                                config[CONF_D0_PIN], config[CONF_D1_PIN], config[CONF_D2_PIN], config[CONF_D3_PIN],
                                config[CONF_D4_PIN], config[CONF_D5_PIN], config[CONF_D6_PIN], config[CONF_D7_PIN], 
                                config[CONF_D8_PIN], config[CONF_D9_PIN], config[CONF_D10_PIN], config[CONF_D11_PIN],
                                config[CONF_D12_PIN], config[CONF_D13_PIN], config[CONF_D14_PIN], config[CONF_D15_PIN])

        case 4:
            # Arduino_ESP32S2PAR8( dc,  cs,  wr,  rd); // Constructor
            # Arduino_ESP32S2PAR16( dc,  cs,  wr,  rd); // Constructor
            # Arduino_RPiPicoPAR8( dc,  cs,  wr,  rd); // Constructor
            # Arduino_RPiPicoPAR16( dc,  cs,  wr,  rd); // Constructor
            bus = databus[0].new(config[CONF_DC_PIN], config[CONF_CS_PIN], config[CONF_WR_PIN], -1)

        case 5:
            # Arduino_RPiPicoSPI( dc,  cs,  sck = PIN_SPI0_SCK,  mosi = PIN_SPI0_MOSI,  miso = PIN_SPI0_MISO, spi_inst_t *spi = spi0); // Constructor
            if config[CONF_CLK_PIN] == -1: config[CONF_CLK_PIN] = cg.RawExpression("PIN_SPI0_SCK") 
            if config[CONF_MOSI_PIN] == -1: config[CONF_MOSI_PIN] = cg.RawExpression("PIN_SPI0_MOSI") 
            if config[CONF_MISO_PIN] == -1: config[CONF_MISO_PIN] = cg.RawExpression("PIN_SPI0_MISO") 
            bus = databus[0].new(config[CONF_DC_PIN], config[CONF_CS_PIN], config[CONF_CLK_PIN], config[CONF_MOSI_PIN], config[CONF_MISO_PIN])

    model = config[CONF_MODEL]
    
    if model[1] == 0 and CONF_DIMENSIONS in config:
        gfx = model[0].new( bus,config[CONF_RESET_PIN], config[CONF_ROTATION], config[CONF_IS_IPS_DISPLAY], config[CONF_DIMENSIONS][0], config[CONF_DIMENSIONS][1])
    elif model[1] <= 1:
        gfx = model[0].new( bus,config[CONF_RESET_PIN], config[CONF_ROTATION], config[CONF_IS_IPS_DISPLAY])
    elif model[1] == 2 and CONF_DIMENSIONS in config:
        gfx = model[0].new( bus,config[CONF_RESET_PIN], config[CONF_ROTATION], config[CONF_DIMENSIONS][0], config[CONF_DIMENSIONS][1])
    else:
        gfx = model[0].new( bus,config[CONF_RESET_PIN], config[CONF_ROTATION])

    del config[CONF_ROTATION]
    
    await display.register_display(var, config)

    if CONF_LAMBDA in config:
        lambda_ = await cg.process_lambda(
            config[CONF_LAMBDA], [(display.DisplayBufferRef, "it")], return_type=cg.void
        )
        cg.add(var.set_writer(lambda_))

    if CONF_BACKLIGHT in config:
        reset = await cg.get_variable(config[CONF_BACKLIGHT])
        cg.add(var.set_backlight(reset))
