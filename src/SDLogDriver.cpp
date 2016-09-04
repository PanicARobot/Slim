#include "SDLogDriver.h"
#include <SD.h>

void read_file(const char* filename)
{
	File f = SD.open(filename);
	if(!f) Serial.println("No such file");
	else while(f.available())
		Serial.write(f.read());
	f.close();
}

void append_file(const char* filename, const char* entry)
{
	File f = SD.open(filename, FILE_WRITE);
	if(!f) Serial.println("Some error");
	f.println(entry);
	f.close();
}
