/**
 * @brief Class handling distance reading with ultrasound on the shell.
 *
 * @author Nicolas Jarnoux
 * @version 1.0
 */
class BuzzPlugin : public ShellPlugin
{
public:
	BuzzPlugin() : ShellPlugin() {;}

	/**
	 * @brief parse the current commant
	 *
	 * @return a code telling if the command have been handle to main event loop
	 */
	int parse() {
		BEGIN_PARSE

		ASSOC(b,buzz) //command b <pin> <freq> <duration>

		END_PARSE
	}

protected:
	/**
	 * @brief the function associated to the b command
	 * 
	 * Buzz a pin at given freq and duration
	 * @return if it goes well, write "done" on the terminal and retrun done. otherwise return next.
	 */
	int buzz() {
		GET_DPIN(pin)  //Get the pulse pin

		//Get the frequency and check if it is ok
		char* _freq = getNextArg();
		if(!_freq) return next();

		int freq = atoi(_freq);
		if(freq <= 31 || freq > 9999) return next();

		//Get the duration and check if it is ok
		char* _length = getNextArg();
		if(!_length) return next();

		int length = atoi(_length);
		if(length < 0 || length > 9999) return next();

		CHECK_END_ARGS

		tone(pin, freq, length);

		return done();
	}
};

REGISTER_SHELLPLUGIN(BuzzPlugin)
