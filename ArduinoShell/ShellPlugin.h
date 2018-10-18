/**
 * This file is the main interface for the arduino command line interpreter.
 * 
 * It defines the base class which every plugin has to derive, macros a utility function to write a good command plugin.
 * 
 * The principle is simple, the process() function is called every loop.
 * It waits for an input on the serial port. the input has the following format:
 *   - <command> <arg1> <arg2> <...>;
 *   - it has to begin by the command
 *   - every args is seperated by a space
 *   - it has to finish bu a semi-colon ;
 * When the function finds this, it will call every parse() function of every plugin until one of them return the code DONE, which means that it has handle the command and there is no need to look forward.
 * When a plugin does not handle a function, for any reason such as:
 *   - the command is unknonwn for the plugin
 *   - the command is known but does not have the right number of arguments
 *   - the command is known, has the right number of arguments but the type of the argument is not unexpected
 *   - any other reason, if the plugin does not handle the command
 * Just return the NEXT code, which means that the next plugin has to check if it can handle this or not.
 * If none of the plugins can handle the command, the process() function will write "UNKNOWN" in the Serial.
 * 
 * The plugin commandment:
 *   - A plugin should be defined in a separate .ino file. It should include this header and devie from the ShellPlugin class.
 *   - A plugin has to be regitred in the system. Just after the class declaration, you should call the REGISTER_SHELLPLUGIN macro
 *   - A plugin has to implement a parse() function with the following format:
 *     - begin with the BEGIN_PARSE macro
 *     - declare every command it can handle with the ASSOC macro
 *     - end with the END_PARSE macro
 *     
 * These are the basis of plugin writing. Please, read the documentation of the macros and functions provided in this class for further information. Look at the BasicShell.ino file for a real example of how to write properly a plugin.
 */

#ifndef SHELLPLUGIN_H
#define SHELLPLUGIN_H
#include <Arduino.h>

/**
 * @brief Register a plugin in the system
 *
 * @param c : the name of the class
 */
#define REGISTER_SHELLPLUGIN(c) c reg##c();

/**
 * @brief The 2 return codes. DONE and NEXT. Normally, you should never use these directly.
 */
#define DONE 0
#define NEXT -1

/**
 * @brief Convinience macro to compare the value of a C-style string (char*) variable.
 *
 * @param v : the variable
 * @param c : the value to compare, without the quotes.
 * 
 * @exemple
 * @code
 * if(COMP(myVar,testString))
 * @endcode
 *
 * @return true if they match, false otherwise.
 */
#define COMP(v,c) strcmp(v, #c ) == 0

/**
 * @brief Convinience macro that extract a digital pin id from the serial and declare it as an int;
 * Analogic pins labaled as A0, A1, A2, are automatically converted to digital pin id.
 * It automatically return next() if the pin id is not ok.
 */
#define GET_DPIN(v) \
	char* _##v = getNextArg();\
	if(!_##v) return next();\
	int v = getDPin(_##v);\
	if(v == -1) return next();

/**
 * @brief Convinience macro that extract a analog pin id from the serial and declare it as an int;
 * analog pin id is the number after the A in arduino layout.
 * It automatically return next() if the pin id is not ok.
 */
#define GET_APIN(v) \
	char* _##v = getNextArg();\
	if(!_##v) return next();\
	int v = getDPin(_##v);\
	if(v == -1) return next();

/**
 * @brief Convinience macro that check if we reach the ends of the args.
 * This should be called when you think have get all your args and don't expect to extract more. It will retrun next() if there is more arguments.
 * If not called, there is no more possibility to extend the behavior of a command.
 */
#define CHECK_END_ARGS \
	char* _ShellPluginEndArgs = getNextArg();\
	if(_ShellPluginEndArgs) return next();

/**
 * @brief Begin a parse function.
 * Should be called in every plugin, once at the beginning of the parse function before the ASSOC macros
 */
#define BEGIN_PARSE char* cmd = getCmd(); if(!cmd) return NEXT;

/**
 * @brief Associate a command with a callback member function
 * 
 * @param c : the command without the quotes
 * @param f : the member function name
 *
 * The callback member function should return an int, a classic return code and shouldn't take arguments.
 * 
 * @example
 * @code 
 * ASSOC(c,config) //associate the c command with the config() member function
 * @endcode
 */
#define ASSOC(c,f) if(COMP(cmd,c)) return f();

/**
 * @brief End a parse function
 * Should be called in every plugin, once at the end of the parse function after the ASSOC macros.
 */
#define END_PARSE return NEXT;

#define printV(v) _s->print(F("V=")); _s->println(v); return DONE;
#define printVF(v,f) _s->print(F("V=")); _s->println(v,f); return DONE;

#define bufSize 33
#define strSize bufSize-1

/**
 * @brief base Class for shell plugin.
 * 
 * Every plugin should derived from this class.
 */
class ShellPlugin
{
public:
	/**
	 * @brief this is the default constructor. every plugin should implement a default constructor and call this one.
	 */
	ShellPlugin(); //Contruct the plugin by register it in the chain

	/**
	 * @brief the function that every plugin has to implement.
	 * 
	 * @example
	 * @code
	 * int parse() {
	 *     BEGIN_PARSE
	 *     
	 *     ASSOC(myCommand,myCallback)
	 *     
	 *     END_PARSE
	 * }
	 * @endcode
	 *
	 * @return a return code that indicates if the command was handle or not.
	 */
	virtual int parse() = 0; //the callback function

	/**
	 * @brief The main event loop
	 * It is called in the loop function of arduino
	 */
	static void process(); //main loop

protected:
	/**
	 * @brief get a digital pin id from the string arg
	 *
	 * @param pin : The pin
	 *
	 * @return The pin id.
	 * 
	 * You should use the GET_DPIN macro instead. it perfoms also safty check.
	 */
	int getDPin(char* pin);

	/**
	 * @brief get a analog pin id from the string arg
	 *
	 * @param pin : The pin
	 *
	 * @return The pin id.
	 * 
	 * You should use the GET_APIN macro instead. it perfoms also safty check.
	 */
	int getAPin(char* pin);

	/**
	 * @brief Gets the command.
	 *
	 * @return The command.
	 * 
	 * shouldn't be called directly. It is called in BEGIN_PARSE macro
	 */
	char* getCmd();

	/**
	 * @brief Gets the next argument.
	 *
	 * @return The next argument.
	 * 
	 * Juste after calling getCmd(), get the followings aguments, or return null if no more args.
	 */
	char* getNextArg();

	/**
	 * @brief the done function
	 *
	 * @return the DONE return code
	 * 
	 * This function should be call to end the command parsing saying "Hey, I got it".
	 */
	int done();

	/**
	 * @brief the next function
	 *
	 * @return the NEXT return code
	 * 
	 * This function should be call to end the command parsing saying "Sorry man, I don't know this one".
	 */
	int next() { return NEXT; }

	/**
	 * @brief the value function
	 *
	 * @return the DONE return code
	 * 
	 * This function should be call to end the command parsing saying "Hey, I got it and here is the value I've just read!".
	 */
	int value(const __FlashStringHelper * p) 	{ printV(p); }
	int value(const String & p) 				{ printV(p); }
	int value(const char p[]) 					{ printV(p); }
	int value(char p) 							{ printV(p); }
	int value(unsigned char p, int f = DEC) 	{ printVF(p,f); }
	int value(int p, int f = DEC) 				{ printVF(p,f); }
	int value(unsigned int p, int f = DEC) 		{ printVF(p,f); }
	int value(long p, int f = DEC) 				{ printVF(p,f); }
	int value(unsigned long p, int f = DEC) 	{ printVF(p,f); }
	int value(double p, int f = 2) 				{ printVF(p,f); }
	int value(const Printable& p) 				{ printV(p); }

private:

	static ShellPlugin* 	_begin; //begining of the chain
	static char 			_buffer[bufSize]; //input buffer
	static char 			_cmd[bufSize];
	static HardwareSerial* 	_s;
	ShellPlugin* 			_next; //the next plugin to call if not parsed
};

#endif //SHELLPLUGIN_H