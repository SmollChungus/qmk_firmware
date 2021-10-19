# MCU name
MCU = atmega32u4

# BOOTLOADER SECTION

BOOTLOADER = atmel-dfu

# BUILD OPTIONS
#

BOOTMAGIC_ENABLE = lite	# Virtual DIP switch configuration(+1000)
MOUSEKEY_ENABLE = no	# Mouse keys(+4700)
EXTRAKEY_ENABLE = yes	# Audio control and System control(+450)
CONSOLE_ENABLE = no	# Console for debug(+400)
COMMAND_ENABLE = no    # Commands for debug and configuration
SLEEP_LED_ENABLE = no  # Breathing sleep LED during USB suspend
NKRO_ENABLE = no		# USB Nkey Rollover - if this doesn't work, see here: https://github.com/tmk/tmk_keyboard/wiki/FAQ#nkro-doesnt-work
BACKLIGHT_ENABLE = no  # Enable keyboard backlight functionality
AUDIO_ENABLE = no
RGBLIGHT_ENABLE = no
LTO_ENABLE = yes

OLED_ENABLE = yes
OLED_DRIVER = SSD1306

DEFAULT_FOLDER = leftie/rev2
