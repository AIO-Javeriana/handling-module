#include <iostream>
#include <stdlib.h>

using namespace std;

string  PWM_PATH= "sys/devices/platform/pwm-ctrl/";
///sys/devices/platform/pwm-ctrl

void execute(string s){
	system(s.c_str());
}

int main(){

	cout<<"Strared Gripper Module"<<PWM_PATH<<endl;
	execute("sudo modprobe pwm-meson");
	execute("sudo modprobe pwm-ctrl");
	
	execute("echo 102 > /sys/devices/platform/pwm-ctrl/duty0");
	execute("echo 50 > /sys/devices/platform/pwm-ctrl/freq0");
	execute("echo 1 > /sys/devices/platform/pwm-ctrl/enable0");

	return 0;
}
