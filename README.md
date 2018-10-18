# ArduinoShell

An arduino command line interpreter with a plugin system for exend the range of commands

## Shell basis
It is simple, the `process()` method is called every loop. It checks for an input on the serial port. The input has the following format:
 * `<command> <arg1> <arg2> <...>;`
 * every args is seperated by a space
 * it has to finish by a semi-colon `;`

When the `process()` method finds something like this, it will call every `parse()` method of every plugin until one of them return the code DONE, which means that it has handle the command and there is no need to look forward.

When a plugin does not handle a command, for any reason such as:
 * the command is unknonwn for the plugin
 * the command is known but does not have the right number of arguments
 * the command is known, has the right number of arguments but the type of the argument is unexpected
 * any other reason, if the plugin does not handle the command

Just return the NEXT code, which means that the next plugin has to check if it can handle this or not.

If none of the plugins can handle the command, the process() method will write "UNKNOWN" in the Serial.

The ShellPlugin class doesn't handle any command. It's just a building block.

## Shell main ino file
The main ino file is really simple, it initialize the Serial object in `setup()` and simply call `ShellPlugin::process()` in `loop()`. See the ArduinoShell.ino file but it's really self-explanatory.

## Write a Shell Plugin

#### Begin a new plugin
To write a plugin, just create a new ino file in the ArduinoShell sketch folder and declare a class that inherit from the ShellPLugin class. It must have a default constructor that call the default ShellPlugin constructor. At the end of the declaration, call the macro `REGISTER_SHELLPLUGIN(ClassName)`

the class implementation can be done directly in the declaration, Java style.

Example : 
```cpp
class BasicShell : public ShellPlugin
{
public:
    BasicShell() : ShellPlugin() {;}
    
    //     ...
    //Rest of the plugin, see below
    //     ...
};
REGISTER_SHELLPLUGIN(BasicShell)
```

#### The parse() function
Every plugin must implement the public `int parse()` method. For elegant programming, a serie of macros are defined to help you. Juste begin the function by calling `BEGIN_PARSE` macro, next associate a command to a callback member function with the `ASSOC` macro. And simlply end the function with the `END_PARSE` macro.

In the `ASSOC` macro, you dont need to quote the command `"cmd"`, just write `cmd`. You don't need to give a function pointer too, juste the name of the function is ok.
`ASSOC` is just syntactic sugar for a serie of `if else` statement.

Example :
```cpp
int parse() {
    BEGIN_PARSE

    ASSOC(c,config) //command c <pin> <mode>; configure a pin in a mode ("DI" for digital input, etc...)
    ASSOC(w,write)  //command w <pin> <value>; write a digital output
    ASSOC(r,read)   //command r <pin>; read a digital input
    ASSOC(a,aread)  //command a <pin>; read a analog input
    ASSOC(p,pwm)    //command p <pin> <duty cycle>; write a pwm signal

    END_PARSE
}
```

#### The user-defined callback function
After declare the `ASSOC` in the `parse()` method. You have to defined a callback. It must return an `int` and not have any arguments. I will take the example of the `w <pin> <value>` I've defined above, associated with the `write` callback. The prototype of the function must be : `int write()`.

The callback method must call one of the three functions below in its return statement :
* `done()` if the command was handled but did not require to write a value back in the serial. like a `digitalWrite`. It just write `DONE` in the serial to tell that the command goes well.
* `value(SOMETHING)` if the command was handled and require to send something back to the serial. like a `analogRead`. It writes `V=YOUR_VALUE` in the serial. you can use it like the `print()` method of the Serial arduino object.
* `next()` if the command was not handled. it tells the main loop to try with the next plugin.

You can use some methods and macros to help :
* `getNextArg()` method return a `char*` null terminated string with the next argument provided in the command line. Or a null pointer if ther no more arg.
* `COMP(v,s)` macro compare a `char*` null terminated string variable `v` with the string `s`. Don't quote the string, just write it like `COMP(myVar,value)`.
* `GET_DPIN(v)` macro take the next arg and parse it as an arduino digital pin id. It declare an int variable named after the  `v`parameter. If the user send an analog pin like `A2`, the macro convert it to a valid digital pin id. It automatically return next() if it does not work.
* `GET_APIN(v)` macro take the next arg and parse it as an arduino analog pin id. It declare an int variable named after the  `v`parameter. An analog pin id is the number after the `A` in the arduino layout. the user can provide it as `A3` for example, it will return 3.
* `CHECK_END_ARGS` macro check if you reach the end of the args in the command line. It return next if there is more args to be parsed. This macro should be called after you parsed all the args you need to be sure that it is the command you are expecting and not another.

Here is an example of the `write` callback with the use of these helpers :
```cpp
// is associated with the w <pin> <value> command.
int write() {
    GET_DPIN(dpin)

    char* _value = getNextArg();

    if(!_value)
        return next();

    CHECK_END_ARGS

    int value = atoi(_value) > 0 ? HIGH : LOW;

    digitalWrite(dpin, value);

    return done();
}
```

Here is an example that returns a value in the serial port :
```cpp
int aread() {
    GET_APIN(apin)
    CHECK_END_ARGS

    return value(analogRead(apin));
}
```

You can find the `examples` folder for full example. Just copy them into the main sketch folder and compile with the arduino IDE.