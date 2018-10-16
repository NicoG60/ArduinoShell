#include "ShellPlugin.h"

ShellPlugin* 	ShellPlugin::_begin 			= NULL;
char 			ShellPlugin::_buffer[bufSize] 	= {};
char 			ShellPlugin::_cmd[bufSize] 		= {};
HardwareSerial* ShellPlugin::_s 				= &Serial;

ShellPlugin::ShellPlugin() :
	_next(NULL)
{	
	if(_begin) //if there is a chain, looking for the last instance;
	{
		ShellPlugin* end = _begin;
		while(end->_next)
			end = end->_next;

		end->_next = this;
	}
	else //Else, start a new one;
		_begin = this;

}

void ShellPlugin::process()
{
	//If bytes available to read
	while(_s->available() > 0)
	{
		//clear the buffer
		memset(_buffer, 0, bufSize*sizeof(char));
		//read them until terminator char
		_s->readBytesUntil('\r', _buffer, bufSize);

		//if the buffer is full, ignore because it shouldn't. Never.
		if(_buffer[strSize] == 0 && _begin)
		{
			ShellPlugin* current  =_begin;
			while(current)
			{
				if(current->parse() == 0)
					break;

				current = current->_next;
			}

			if(!current)
				_s->println(F("UNKNOWN"));
		}
	}
}

char* ShellPlugin::getCmd()
{
	memcpy(_cmd, _buffer, bufSize*sizeof(char));
	return strtok(_cmd, " ;");
}

char* ShellPlugin::getNextArg()
{
	return strtok(NULL, " ;");
}

int ShellPlugin::done()
{
	_s->print(F("DONE "));
	_s->println(_buffer);
	return DONE;
}

int ShellPlugin::getDPin(char* pin)
{
	if(!pin)
		return -1;

	int p = 0;

	if(pin[0] == 'A')
	{
		pin++;
		p = analogInputToDigitalPin(atoi(pin));
	}
	else
		p = atoi(pin);

	return p;
}

int ShellPlugin::getAPin(char* pin)
{
	if(!pin)
		return -1;

	if(pin[0] == 'A')
		pin++;
	
	return atoi(pin);
}