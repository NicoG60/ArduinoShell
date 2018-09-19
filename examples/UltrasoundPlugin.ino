/**
 * @brief      Class handling distance reading with ultrasound on the shell.
 *
 * @author Nicolas Jarnoux
 * @version 1.0
 */
class UltrasoundPlugin : public ShellPlugin
{
public:
  UltrasoundPlugin() : ShellPlugin() {;}

  /**
   * @brief      parse the current commant
   *
   * @return     a code telling if the command have been handle to main event loop
   */
  int parse() {
    BEGIN_PARSE

    ASSOC(d,read) //command d <pinPulse> <pinEcho>

    END_PARSE
  }

protected:
  /**
   * @brief      the function associated to the d command
   * 
   * Read the distance, 255 = infinite;
   * @return     if it goes well, write "done" on the terminal and retrun done. otherwise return next.
   */
  int read() {
    GET_DPIN(pPin)  //Get the pulse pin
    GET_DPIN(ePin)  //Get the echo pin
    CHECK_END_ARGS

    digitalWrite(pPin, HIGH);
    delayMicroseconds(1000);
    digitalWrite(pPin, LOW);

    int v = (pulseIn(ePin,HIGH)/2)/29.1+2;
    if (v > 255) { v = 255; }

    return value(v);
  }
};

REGISTER_SHELLPLUGIN(UltrasoundPlugin)