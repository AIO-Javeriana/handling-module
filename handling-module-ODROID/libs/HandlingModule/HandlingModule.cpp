
#ifndef MOBILITYMODULE_H
#define MOBILITYMODULE_H 

#include <Module.cpp>
#include <HandlingModuleInfo.cpp>
#include <wiringPi.h>
#include <CloseService.cpp>
#include <OpenService.cpp>

class HandlingModule: public Module{

    public:
        HandlingModule(std::string host, int port,HandlingModuleInfo *moduleInfo) :
            Module(host, port,moduleInfo) {
			   wiringPiSetup();
			   this->addService(new CloseService());
			   this->addService(new OpenService());
        }
};

#endif
