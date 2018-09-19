/**
 * @brief      Class for basic shell.
 *
 * This class provide basic commands to interact whit arduino.
 *
 * @author Nicolas Jarnoux
 * @version 1.0
 */
class BasicShell : public ShellPlugin
{
public:
  BasicShell() : ShellPlugin() {;}

  /**
   * @brief      parse the current commant
   *
   * @return     a code telling if the command have been handle to main event loop
   */
  int parse() {
    BEGIN_PARSE

    ASSOC(c,config) //command c <pin> <mode>
    ASSOC(w,write)  //command w <pin> <value>
    ASSOC(r,read)   //command r <pin>
    ASSOC(a,aread)  //command a <pin>
    ASSOC(p,pwm)    //command p <pin> <duty cycle>

    END_PARSE
  }

protected:
  /**
   * @brief      the function associated to the c command
   * 
   * Configures a pin in a particular mode
   * It need a digital pin id and a mode as arguments.
   * mode can be one of those :
   *   - DI for digital input
   *   - DIP for digital input with a pullup resistor
   *   - DO for digital output
   *   - AI for analogic input
   *   - PWM for a pwm output
   *
   * @return     if it goes well, write "done" on the terminal and retrun done. otherwise return next.
   */
  int config() {
    GET_DPIN  //Get the digital pin id as first arg

    char* _mode = getNextArg(); //get the mode

    CHECK_END_ARGS //Check if it's the last arg

    if(!_mode) //If no second args, return next, maybe nother plugin can handle that
      return next();

    //If mode is a santard input
    if(COMP(_mode,DI) || COMP(_mode,AI))
    {
      pinMode(dpin, INPUT);
      return done();
    }
    else if(COMP(_mode,DIP)) //Else if it's an input with pullup
    {
      pinMode(dpin, INPUT_PULLUP);
      return done();
    }
    else if(COMP(_mode,DO) || COMP(_mode,PWM)) //Else if it's a standard output
    {
      pinMode(dpin, OUTPUT);
      return done();
    }
    else //Else, it should be something else, maybe another plugin can handle that
      return next();
  }

  /**
   * @brief      the function associated to the w command
   * 
   * Writes a digital output on a pin.
   * It need a digital pin id and a value to send
   *   - if value is a character or a string => pin is set to LOW
   *   - if value is a number => pin is set to LOW if <= 0, to HIGH if > 0
   *
   * @return     if it goes well, write "done" on the terminal and retrun done. otherwise return next.
   */
  int write() {
    GET_DPIN

    char* _value = getNextArg();

    if(!_value)
      return next();

    CHECK_END_ARGS

    int value = atoi(_value) > 0 ? HIGH : LOW;

    digitalWrite(dpin, value);

    return done();
  }

  /**
   * @brief      the function associated to the r command
   * 
   * Reads a digital input
   * need a digital pin id as only arguments
   *
   * @return     if it goes well, write the value on the terminal and return done. otherwise return next.
   */
  int read() {
    GET_DPIN
    CHECK_END_ARGS

    return value(digitalRead(dpin));
  }

  /**
   * @brief      the function associated to the a command
   * 
   * Reads an analogic input
   * need a analogic pin id as only arg
   *
   * @return     if it goes well, write the value on the terminal and return done. otherwise return next.   */
  int aread() {
    GET_APIN
    CHECK_END_ARGS

    return value(analogRead(apin));
  }

  /**
   * @brief      the function associated to the p command
   * 
   * Writes a pwm signal
   * need a digital pin id and a value of the duty cycle.
   * value has to be a number between 0 and 255
   *
   * @return     if it goes well, write the value on the terminal and return done. otherwise return next.   */
  int pwm() {
    GET_DPIN

    char* _value = getNextArg();

    if(!_value)
      return next();

    CHECK_END_ARGS

    analogWrite(dpin, atoi(_value));

    return done();
  }
};

REGISTER_SHELLPLUGIN(BasicShell)