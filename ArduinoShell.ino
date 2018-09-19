/**
 * @brief Command Line Interpreter for arduino
 * 
 * This is the building block of OpenOrganigram software,
 * it's a command line interpreter with possibility of extention with custom plugin.
 * The basic commands are provided in this file, all other plugin should be in a seperate ino file and should follow the indication provided in the documentation.
 * 
 * @file ArduinoShell.ino
 * @author Nicolas Jarnoux
 * @version 1.0
 */

#include "ShellPlugin.h"

//==========================================================
//      MAIN
//==========================================================

void setup() {
	Serial.setTimeout(100);
	Serial.begin(9600);
}

void loop() {
	ShellPlugin::process();
}