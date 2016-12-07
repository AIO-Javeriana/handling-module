#ifndef MODULE_H
#define MODULE_H

#include <json.hpp>
#include <string>
#include<ModuleInfo.cpp>
#include <CommunicationChannel.cpp>

using json = nlohmann::json;
using namespace std;


class Module{
    
    protected:
        Services* services;
        CommunicationChannel* communicationChannel;
        ModuleInfo* moduleInfo;
    public:
        Module( string host, int port, ModuleInfo *moduleInfo){
            this->moduleInfo = moduleInfo;
            this->services = new Services();
            this->communicationChannel = new CommunicationChannel(host, port, this->services,moduleInfo);
        }
        
        void addService(Service* service){
            this->services->addService(service);
        }
        
		void addSensorService(SensorService* sensorService){
            this->services->addSensorService(sensorService);
		}
        void start(){
            json module_info = this->services->getInfo();
            module_info["MODULE_ID"] =  this->moduleInfo->getModule_id();
            this->communicationChannel->start(module_info.dump());
        }
        
        string getModule_id(){
            return this->moduleInfo->getModule_id();
        }
        void setModule_id(string module_id){
            this->moduleInfo->setModule_id(module_id);
        }
};

#endif
