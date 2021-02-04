## Micro Python Notes
> Note: this section was discontinued. It is still documented here for reference

Exploring micro python on the ESP32. See docs here https://docs.micropython.org/en/latest/esp32/quickref.html#general-board-control. These notes were written while testing on an ESP32 Huzzah board from Adafruit.

## Installation 

Currently using the **LoBo port** of micro python. See https://github.com/loboris/MicroPython_ESP32_psRAM_LoBo for build and instructions. This offers some additional funcitonality - in particular, access to high speed ADC using the I2S peripheral. Build instructions are in the repo - be patient because it's a little tricky. You'll also need the ESP IDF which can be downloaded from https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/. 

Once you've done all the setup, the last step is to flash the the micropython firmware onto your ESP (once off procedure). I used 
```
esptool.py --chip esp32 --port /dev/cu.SLAB_USBtoUART --baud 460800 --before default_reset --after no_reset write_flash -z --flash_mode dio --flash_freq 40m --flash_size detect 0x1000 bootloader/bootloader.bin 0xf000 phy_init_data.bin 0x10000 MicroPython.bin 0x8000 partitions_mpy.bin
```
Note that you'll likely need to change `/dev/cu.SLAB_USBtoUART` to match *your serial port* to which your ESP is connected.

### Interactive REPL
This is a really cool bit of funcitonality - you can open a micropython REPL (read-evaluate-println) that allows you to evaluate python commands on the MCU in real time and get feedback. Useful for testing and prototyping. This is accessed over serial. You can use a serial console client (like Putty) but in my case (unix), I just used 
```
screen /dev/cu.SLAB_USBtoUART 115200
```
to make a connection to the serial port and access the REPL over UART. To exit the screen session, use `Crtl-a Ctrl-d` although this seems buggy.

##### Using Picocom
Install `picocom` using `brew install picocom` (linux, mac) - not sure about windows. Then, similarly to `screen` command:

```
picocom /dev/cu.SLAB_USBtoUART -b115200
```

### Flashing Code
There are several ways to do this but a very convenient one is to use the `ampy` python library from Adafruit. Install using `pip install adafruit-ampy`. Then, simply create a file like `main.py` in your favourite text edit (VS Code, Atom, etc) and run 
```
ampy --port /dev/tty.SLAB_USBtoUART put main.py
```
where `main.py` is the file that you're looking to flash.

**note**: as you may have guessed, you'll need to replace the serial port wherever it used above with your version.
