# Cosplay TKL

![Cosplay TKL](https://i.imgur.com/K33kCEl.jpg)

Hotswap F13 TKL PCB

Keyboard Maintainer: Smoll
Hardware Supported: Cosplay TKL rev1 PCB

Hardware Availability: [Cosplay TLK](https://smollkeebs.com)

Make example for this keyboard (after setting up your build environment):

    make cosplay_tkl:default

See [build environment setup](https://docs.qmk.fm/#/getting_started_build_tools) then the [make instructions](https://docs.qmk.fm/#/getting_started_make_guide) for more information.

## Bootloader

Enter the bootloader in 3 ways:

* **Bootmagic reset**: Hold down the key at (0,0) in the matrix (usually the top left key or Escape) and plug in the keyboard
* **Physical reset button**: Briefly short the `RST` and `GND` pads on the SWD header twice, or short the `BOOT` header and plug in keyboard
* **Keycode in layout**: Press the key mapped to `QK_BOOT` if it is available
