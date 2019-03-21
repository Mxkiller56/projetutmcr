# Projet tutoré MyConnectedRoom 2018/2019 (fr)
Création d'un système permettant d'afficher la disponibilité d'une salle.

# MyConnectedRoom 2018/2019 (en)
Room state display IoT firmware, compatible with ESP32 and iCalendar.

# How to build (en)
There are two ways of building the project, depending on what you would like to do. First way: you want to run this on the ESP32. Second way: you want to run this on your computer, to test some code.

## How to build this project for ESP32
What you'll need: the official Arduino-ide, and the ESP32 board support package for the Arduino-ide from Espressif. If you haven't already, download and install them. The type of hardware we use is an ESP32 Wrover Module, but you may use everything else (although we can't test every board, the program should be compatible with all the ESP32s-based ones).

1. Make a new empty sketch and save it. Keep its location in mind (here, we'll take `/home/user/src/arduino_sketches/sketch_mar20a` as an example path). Close the Arduino-ide.
2. Delete the main program file in the sketch folder (in our example, it is `sketch_mar20a.ino`).
3. Copy every file in the `src/` folder of the project in the sketch directory.
4. Delete uneeded files (else the Arduino-ide will try to open and compile them, compilation will fail). Those files are:
   - Arduino_testing.cpp
   - Arduino_testing.h
   - Doxyfile
   - example.ics
   - Makefile
   - simrun.cpp
   - testing.cpp
   - util.c
   - util.h
5. Create the file secrets.h in the arduino sketch directory with an external text editor (for example, notepad.exe, vi, ed, whatever). It will contain your 802.1x PEAP credentials, in this form (here we take example creds):
   ```c++
   char _MCR_EAP_IDENTITY[] = "login"; // 802.1x EAP login
   char _MCR_EAP_PASSWORD[] = "password"; // 802.1x EAP password
   char _MCR_SSID[] = "ssid";
   ```
   If you want to connect to networks like eduroam, no problem ;) We have already tested that and it works.
6. Rename the main.ino after the name of the sketch directory (in our example, it will be renamed `sketch_mar20a.ino`).
7. From the Arduino-ide, open the project folder again. This time, it should show all the others files in tabs. Select the appropriated board type ("ESP32 Wrover Module", for example) in Tools>Board type and the serial port for flashing it. You're now ready to build & flash, congrats !

## How to run this project on PC
First, we'd like to warn you that building on computer has only been tested on Debian. We haven't tried this on others OSes (*BSD for example). The firmware is not intended to use PC as a platform, building on PC is just a way to test and tinker with the sources in a faster way. All the binaries produced will have debug symbols, so you can use you favorite debugger on them. What you'll need:
- The g++ compiler. It's in the package of the same name in Debian. You can use another compiler if you wish, but then you'll have to modify the Makefile.
- You'll also need GnuMake (`make` package in Debian)

How to proceed:
1. cd into the `src` directory of the project
2. type `make`

Alternatively, you can use one of the Makefile targets. You'll then obtain two binaries. `simrun` is meant to run the firmware in simulation mode. Here the interactions with the network, user, etc, are emulated through the compatibility files. Source code compatibility is provided by `Arduino_testing` files. The other binary, `test` is meant for unit-testing the modules of the firmware. It's principally built around the `testing.cpp` file.

# How to build the doc
This project has also some documentation included in the source code, etc. You can generate HTML documentation for it using the Doxygen program (package doxygen in Debian).

Here is how to proceed:
1. cd into the `src` directory of the project
2. type `make doc`

The documentation is then outputted in the folder `doc/html`. After this, you may want to open the file `doc/html/index.hmtl` with a web browser.
