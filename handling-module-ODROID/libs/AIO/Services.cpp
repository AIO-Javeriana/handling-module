
#ifndef SERVICES_H
#define SERVICES_H

#include <json.hpp>
#include <queue>
#include <map>
#include <list>
#include <Module.cpp>
#include <mutex>
#include <sio_client.h>
#include <CommunicationEvents.cpp>

using json = nlohmann::json;
using namespace sio;
using namespace std;

class CommandInfo{
    json command;
    bool completed;
    bool error;
    
  public:
      CommandInfo(json command){
        this->command=command;
        this->completed=false;
        this->error=false;
      }
      void setCommand(json command){
        this->command=command;
      }
      json getCommand(){
        return this->command;
      }
      void completedWork(){
          this->completed=true;
      }
      void errorWork(){
          this->completed=true;
      }
      bool isCompletedTask(){
          return this->completed;
      }
      bool isError(){
          return this->error;
      }
};
 
class SensorService{
    protected:
		string name;
		thread* sensor_thread;
		void (*execute_function)(void);
	public:
		static string module_ID;
		static socket::ptr current_socket;
		static mutex* _lock;
        SensorService(){          
   
        }

		SensorService(string module_ID){
            this->module_ID = module_ID;
   		}

		string getName(){
            return this->name;
        }
        
		void start(socket::ptr& current_socket, mutex* _lock){
			this->current_socket = current_socket;
			this->_lock = _lock;
			cout << "STARTING SERVICE: " << this->name << endl;
			this->sensor_thread = new thread(execute_function);
		}

		json getInfo(){
            json info;
            info["COMMAND"] = this->name;
            info["PARAMS"] = json::array();
            info["INTERRUPTIBLE"] = false;
            info["SERVICE"] = true;
            return info;
        }

};

string SensorService::module_ID ="";
socket::ptr SensorService::current_socket = NULL;
mutex* SensorService::_lock= NULL;

class Service{
    protected:
        typedef Service* (*crfnptr)(void);
        typedef map<string, crfnptr> CreatorMap;
        string name;
        list<string> params;
        bool interruptible;
        bool service;
        static CreatorMap creators;
    public:
        Service(){          
            
        }
        
        Service(string name, list<string> params, bool interruptible, bool service){
            this->name = name;
            this->params = params;
            this->interruptible = interruptible;
            this->service = service;
        }
        
        virtual bool execute(json params, string &msg,ModuleInfo* moduleInfo) = 0;
        
        string getName(){
            return this->name;
        }

        json getInfo(){
            json info;
            info["COMMAND"] = this->name;
            info["PARAMS"] = json::array();
            for(string param: params){
                info["PARAMS"].push_back(param);
            }
            info["INTERRUPTIBLE"] = this->interruptible;
            info["SERVICE"] = this->service;
            return info;
        }
        
        static Service* createFromString(string name)
        {
          CreatorMap::const_iterator it = creators.find(name);
          return it == creators.end() ? NULL : it->second();
        }
        
        static void registerClass(string name, crfnptr f)
        {
            cout << "Clase " << name << " registrada" << endl;
            creators[name] = f;
        }
};
map<string, Service* (*)(void)> Service::creators = map<string, Service* (*)(void)>();

class Services{
    map<long,queue<CommandInfo>> commandsInfo;
    map<string, Service*> services;  
	map<string, SensorService*> sensorServices;
    bool working;

  public:
      Services(){
          this->working=false;
      }

      bool assessWork(json command){
        map<string, Service*>::iterator it = services.find(command["COMMAND"]);
            if(it != services.end()){
              this->commandsInfo[command["GROUP_ID"]].push(CommandInfo(command));
              return true;
            }else{
              return false;
        }
      }

      CommandInfo getNextWork(long group_id){
            CommandInfo command=this->commandsInfo[group_id].front();
            this->commandsInfo[group_id].pop();
            return command;
      }

      bool isEmptyCommands(long group_id){
            return this->commandsInfo[group_id].empty();
      }

      void setWorking(bool working){
          this->working=working;
      }

      bool isWorking(){
          return this->working;
      }
      
      void addService(Service* service){
          this->services[service->getName()] = service;
      }
      
      void addSensorService(SensorService* sensorService){
		  cout << "SENSOR " << sensorService->getName() << " agregado" << endl;
          this->sensorServices[sensorService->getName()] = sensorService;
      }

      json getInfo(){
          json module_info;
          module_info["COMMANDS"] = json::array();
          map<string, Service*>::iterator it = services.begin();
          for(; it != services.end(); it++){
              Service* service = it->second;
              module_info["COMMANDS"].push_back(service->getInfo());
          }

		  map<string, SensorService*>::iterator its = sensorServices.begin();
  		  for(; its != sensorServices.end(); its++){
              SensorService* sensorService = its->second;
              module_info["COMMANDS"].push_back(sensorService->getInfo());
          }
          return module_info;
      }

	  void start(socket::ptr current_socket, mutex* _lock){
		cout << "Starting Sensor Services " << sensorServices.size() << endl;
		map<string, SensorService*>::iterator it = sensorServices.begin();
	  	for(; it != sensorServices.end(); it++){
              it->second->start(current_socket, _lock);
        }
	  }
};

#endif
