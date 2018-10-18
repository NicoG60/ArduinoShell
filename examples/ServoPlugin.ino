/**
 * @brief Class handling servos on the shell.
 *
 * @author Nicolas Jarnoux
 * @version 1.0
 */
#include <Servo.h>

#define MAX 6

class ServoPlugin : public ShellPlugin
{
public:
	ServoPlugin() : ShellPlugin(), currentIndex(0)
	{
		memset(pinUsed, -1, MAX*sizeof(int));
	}

	/**
	 * @brief parse the current commant
	 *
	 * @return a code telling if the command have been handle to main event loop
	 */
	int parse() {
		BEGIN_PARSE

		ASSOC(c,config)	//command c <pin> <mode>
		ASSOC(sw,write)	//command sw <pin> <value>, write an angle
		ASSOC(sr,read)	//command sr <pin> read the current angle

		END_PARSE
	  }

protected:
	/**
	 * @brief the function associated to the c command
	 * 
	 * Configures a pin in a particular SRV mode
	 * @return if it goes well, write "done" on the terminal and retrun done. otherwise return next.
	 */
	int config() {
		GET_DPIN(dpin) //Get the digital pin id as first arg

		char* _mode = getNextArg(); //get the mode

		CHECK_END_ARGS //Check if it's the last arg

		if(!_mode) //If no second args, return next, maybe nother plugin can handle that
			return next();

		//If mode is a SRV input
		if(COMP(_mode,SRV))
			return add(dpin);
		else //Else, it should be something else, maybe another plugin can handle that
			return next();
	}

	/**
	 * @brief the function associated to the sw command
	 * 
	 * Write an angle to a given servo
	 *
	 * @return if it goes well, write "done" on the terminal and retrun done. otherwise return next.
	 */
	int write() {
		GET_DPIN(dpin)

		char* _value = getNextArg();

		if(!_value)
			return next();

		CHECK_END_ARGS

		int i = isUsed(dpin);

		if(i != -1)
		{
			servos[i].write(atoi(_value));
			return done();
		}
		else
			return next();
	}

	/**
	 * @brief the function associated to the sr command
	 * 
	 * Read an angle from a given servo
	 *
	 * @return if it goes well, write the value on the terminal and retrun done. otherwise return next.
	 */
	int read() {
		GET_DPIN(dpin)

		CHECK_END_ARGS

		int i = isUsed(dpin);

		if(i != -1)
			return value(servos[i].read());
		else
			return next();
	}

	/**
	 * @brief add a servo to the list
	 *
	 * @return if it goes well, write "done" on the terminal and retrun done. otherwise return next.
	 */
	int add(int pin)
	{
		if(currentIndex == MAX)
			return next();

		if(isUsed(pin) == -1)
		{
			servos[currentIndex].attach(pin);
			pinUsed[currentIndex] = pin;
			currentIndex++;
		}

		return done();
	}

	/**
	 * @brief check if the pin is already used by a servo
	 *
	 * @return the index of the servo in the list if found, -1 otherwise
	 */
	int isUsed(int pin)
	{
		int r;

		for(r = 0; r < MAX && pinUsed[r] != pin; r++);

		return r == MAX ? -1 : r;
	}

private:
	int currentIndex;
	int pinUsed[MAX];
	Servo servos[MAX];
};

REGISTER_SHELLPLUGIN(ServoPlugin)