
#include <string>
#include <string.h>
//-----------------------
#include <stdio.h>
#include <MobilityModule.cpp> 

using namespace std;

void start(string module_id,string host,int port);

int main(int argc ,const char* args[])
{   
    string module_id="mobility_module";
    string host="ws://localhost";
    int port=9090;
    for (int i=0;i<argc;i++){
        if (strcmp (args[i],"-h")==0&& i+1<argc){
            host=args[i+1];
			//host="ws://"+host;
			//host="ws://"+host;
			
        }else if (strcmp (args[i],"-n")==0&& i+1<argc){
                    module_id=(args[i+1]);
        }else if (strcmp (args[i],"-p")==0&& i+1<argc){
                    port=stoi(args[i+1]);
        }
        
    }

    cout<<module_id<<" "<<host<<" "<<port<<endl;
    start(module_id,host,port);
    return 0;
}

void start(string module_id,string host,int port){
  
  MobilityModuleInfo moduleInfo(module_id);
  MobilityModule* mobilityModule = new MobilityModule(host, port,&moduleInfo);
  mobilityModule->start();
  while(true);
}
