#include <stdio.h>
#include <wiringPi.h>

int main(){
	
	wiringPiSetup();
	pinMode(0,OUTPUT);
	digitalWrite(0,1);
	wiringPiSetup();

	return 0;
}
