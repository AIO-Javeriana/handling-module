/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#ifndef MODULEIFO_H
#define MODULEIFO_H
using json = nlohmann::json;
using namespace std;
class ModuleInfo{
    protected:
        string module_id;
    public:
        ModuleInfo(string module_id){
            this->module_id=module_id;
        }
        string getModule_id(){
            return this->module_id;
        }
        void setModule_id(string module_id){
            this->module_id=module_id;
        }
        
        virtual ~ModuleInfo(){
        
        }
};
#endif