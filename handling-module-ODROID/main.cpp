#include "./libs/src/sio_client.h"
#include "./libs/json.hpp"
#include <functional>
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <string>
#include <queue>
#include <map>

#ifdef WIN32

#define HIGHLIGHT(__O__) std::cout<<__O__<<std::endl
#define EM(__O__) std::cout<<__O__<<std::endl
#include <stdio.h>
#include <tchar.h>

#else

#define HIGHLIGHT(__O__) std::cout<<"\e[1;31m"<<__O__<<"\e[0m"<<std::endl
#define EM(__O__) std::cout<<"\e[1;30;1m"<<__O__<<"\e[0m"<<std::endl

#endif

using json = nlohmann::json;
using namespace sio;
using namespace std;

std::mutex _lock;
std::condition_variable_any _cond;
bool connect_finish = false;
string module_id="gripper_module";



class connection_listener
{
    sio::client &handler;

public:

    connection_listener(sio::client& h):
    handler(h)
    {
    }


    void on_connected()
    {
        _lock.lock();
        _cond.notify_all();
        connect_finish = true;
        _lock.unlock();
    }
    void on_close(client::close_reason const& reason)
    {
        std::cout<<"sio closed "<<std::endl;
        exit(0);
    }

    void on_fail()
    {
        std::cout<<"sio failed "<<std::endl;
        exit(0);
    }
};

int participants = -1;

socket::ptr current_socket;

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
//8p9vn5
class Services{
    map<long,queue<CommandInfo>> commandsInfo;
    bool working;

  public:
      Services(){
          this->working=false;
      }

      bool assessWork(json command){
          if (command["COMMAND"]=="BLINK") {
              this->commandsInfo[command["GROUP_ID"]].push(CommandInfo(command));
              return true;
          }else if (command["COMMAND"]=="DECIR") {
              this->commandsInfo[command["GROUP_ID"]].push(CommandInfo(command));
              return true;
          }
          return false;
      }

      CommandInfo makeWorkFront(long group_id){
            CommandInfo command=this->commandsInfo[group_id].front();
            this->commandsInfo[group_id].pop();
            command.completedWork();
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

};



class CommunicationChannel{
    //int participants ;
    sio::client h;
    connection_listener l;
    socket::ptr current_socket;
    Services services;
    bool subscribed;
    string url="";
public:


    CommunicationChannel(string host,int port,Services services):l(h){
       this->subscribed=false;
        this->url=host+":"+to_string(port);
        this->services=services;
    }

    void start(){
        h.set_open_listener(std::bind(&connection_listener::on_connected, &l));
        h.set_close_listener(std::bind(&connection_listener::on_close, &l,std::placeholders::_1));
        h.set_fail_listener(std::bind(&connection_listener::on_fail, &l));
        h.connect(url);
        _lock.lock();
        if(!connect_finish)
        {
            _cond.wait(_lock);
        }
        _lock.unlock();
        current_socket = h.socket();
		bind_events();
    }

   void subscription(string &module_info){
            current_socket->emit("SUBSCRIPTION", module_info);
    }

    void bind_events(){
		   current_socket->on("SUBSCRIPTION-REPLY", sio::socket::event_listener_aux([&](string const& name, message::ptr const& data, bool isAck,message::list &ack_resp){
                _lock.lock();
        				json dataJSON=json::parse((data->get_string()));
        				module_id=dataJSON["MODULE_ID"];
                        HIGHLIGHT("MODULE SUBSCRIBED \n ID "<<module_id);//;
        				// EM(user<<":"<<message);
        				this->subscribed=true;

        				_cond.notify_all();
                _lock.unlock();
                current_socket->off("login");
            }));
			current_socket->on("WORK-ASSIGN", sio::socket::event_listener_aux([&](string const& name, message::ptr const& data, bool isAck,message::list &ack_resp){
                _lock.lock();
        				json dataJSON=json::parse((data->get_string()));
                EM("\t WORK-ASSING "<<dataJSON);
        				json reply_info={
        					  {"MODULE_ID","gripper_module"},
        					  {"REPLY","ACCEPTED"},
                    {"COMMAND_ID",dataJSON["COMMAND_ID"]},
                    {"GROUP_ID",dataJSON["GROUP_ID"]}
                };
                if (!this->services.assessWork(dataJSON)){
                  reply_info["REPLY"]="REFUSE";
                }
        				current_socket->emit("WORK-ASSIGN-REPLY", reply_info.dump() );
    		        _lock.unlock();
            }));
      current_socket->on("ALL-BEGINS", sio::socket::event_listener_aux([&](string const& name, message::ptr const& data, bool isAck,message::list &ack_resp){
                _lock.lock();
                json dataJSON=json::parse((data->get_string()));
                EM("\t ALL-BEGINS "<<dataJSON);
                json reply_info={
                    {"MODULE_ID","gripper_module"},
                    {"STATUS","DONE"},//error
                    {"MSG",""}
                    //{"COMMAND_ID",dataJSON["COMMAND_ID"]}

                };
                string reply="";
                this->services.setWorking(true);
                while (!services.isEmptyCommands(dataJSON["GROUP_ID"])){
                      //_lock.lock();
                      CommandInfo command=services.makeWorkFront(dataJSON["GROUP_ID"]);
                      reply_info["COMMAND_ID"]=command.getCommand()["COMMAND_ID"];
                      reply_info["GROUP_ID"]=command.getCommand()["GROUP_ID"];
                      if(!command.isError()){
                        reply_info["STATUS"]="DONE";
                      }else{
                          reply_info["STATUS"]="ERROR";
                      }
                      current_socket->emit("ACTION-FINISH", reply_info.dump() );
                      //_lock.unlock();
                }
                std::cout << "ACTION-FINISH" << std::endl;
                this->services.setWorking(false);
                _lock.unlock();
            }));
            current_socket->on("WORK-STATUS", sio::socket::event_listener_aux([&](string const& name, message::ptr const& data, bool isAck,message::list &ack_resp){
                      _lock.lock();
              				json dataJSON=json::parse((data->get_string()));
                      EM("\t WORK-STATUS "<<dataJSON);
              				json reply_info={
              					  {"MODULE_ID","gripper_module"},
              					  {"STATUS","ERROR"},
                          {"COMMAND_ID",dataJSON["COMMAND_ID"]},
                          {"GROUP_ID",dataJSON["GROUP_ID"]},
                          {"MSG",""}
                      };
                      if (!this->services.isWorking()){
                        reply_info["STATUS"]="WORKING";
                      }
              				current_socket->emit("WORK-STATUS-REPLY", reply_info.dump() );
          		        _lock.unlock();
                  }));


        }

    void setURL(string url){
        this->url=url;
    }

    connection_listener getConnection_listenert(){
        return l;
    }

    socket::ptr getCurrent_socket(){
        return current_socket;
    }

    void endConnection(){
      h.sync_close();
      h.clear_con_listeners();
	  current_socket->off_all();
      current_socket->off_error();
    }

    ~CommunicationChannel(){
      cout<<"Destructor CommunicationChannel"<<endl;
      current_socket->off_all();
      current_socket->off_error();
      h.sync_close();
      h.clear_con_listeners();
      //delete h;
    }

    };


int main(int argc ,const char* args[])
{

    json module_info={
      {"MODULE_ID",module_id},
      {"COMMANDS",json::array({
            {
              {"COMMAND","BLINK"},
              {"PARAMS",json::array({""})}
            },
            {
              {"COMMAND","DECIR"},
              {"PARAMS",json::array({"TEXTO","TONO"})}
            }
          })
      }
    };
    //{"COMMANDS":[["COMMAND","BLINK"],["PARAMS",[""]]],"ID":"gripper_module"}
    //{"ID":"audiovisual_module","COMMANDS":[{"COMMAND":"BLINK","PARAMS":[]}]}
    Services services;
    CommunicationChannel st("ws://10.220.16.18",9090,services);
	//CommunicationChannel st("ws://localhost",9090);

	st.start();
    string info=module_info.dump();
    st.subscription(info);
	current_socket=st.getCurrent_socket();
    HIGHLIGHT("Start to Gripper Module\n'$exit' : Shut Down Module\n");
    for (std::string line; std::getline(std::cin, line);) {
        if(line.length()>0)
        {
            if(line == "$exit")
            {
                break;
            }
			_lock.lock();
			_lock.unlock();


        }
    }


    HIGHLIGHT("Closing...");
	st.endConnection();
	return 0;
}
