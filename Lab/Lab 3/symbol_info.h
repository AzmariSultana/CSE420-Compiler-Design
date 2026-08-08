#include<bits/stdc++.h>
using namespace std;

// #define endl "\r\n"

class symbol_info
{
private:
    string name; //for variable, function, array name
    string type; 
    symbol_info *next; // pointer to the next symbol in the linked list for collision resolution

    int array_size; // for arrays
    vector<pair<string, string>> parameters; // for functions, pair of parameter name and type

    int type_id; // 0 for variable, 1 for array, 2 for function, -1 for undefined
    bool assign_op;

public:
    symbol_info(string name, string type)
    {
        this->name = name;
        this->type = type;
        this->next = NULL;
        this->type_id = 0; // 0 for variable
        this->array_size = 0;
    }


    //for array
    symbol_info(string name, string type, int array_size){
        this->name = name;
        this->type = type;
        this->array_size = array_size;
        this->next = NULL;
        this->type_id = 1; // 1 for array
    }
    //for function
    symbol_info(string name, string return_type, vector<pair<string, string>> parameters){
        this->name = name;
        this->type = return_type;
        this->parameters = parameters;
        this->next = NULL;
        this->type_id = 2; // 2 for function
        this->array_size = 0;
    }


    string get_name()
    {
        return name;
    }
    string get_type()
    {
        return type;
    }
    symbol_info *get_next()
    {
        return next;
    }
    int get_array_size()
    {
        return array_size;
    }
    vector<pair<string, string>> get_parameters()
    {
        return parameters;
    }
    int get_type_id()
    {
        return type_id;
    }

    bool get_is_array()
    {
        if(type_id == 1){
            return true;
        }
        return false;
    }


    bool get_is_function()
    {
        if(type_id == 2){
            return true;
        }
        return false;
    }

    bool get_assign_op(){
        return assign_op;
    }

    bool get_undefined(){
        if(type_id == -1){
            return true;
        }
        return false;
    }


    



    void set_name(string name)
    {
        this->name = name;
    }
    void set_type(string type)
    {
        this->type = type;
    }

    void set_is_array(bool is_array){
        if(is_array){
            this->type_id = 1; // 1 for array
        }
        else{
            this->type_id = -1; // -1 for undefined
        }
    }



    void set_next(symbol_info *next)
    {
        this->next = next;
    }

    void set_array_size(int array_size)
    {
        this->array_size = array_size;
        this->type_id = 1; // 1 for array
    }

    void set_as_function(string return_type, vector<pair<string, string>> parameters){
        this->type = return_type;
        this->parameters = parameters;
        this->type_id = 2; // 2 for function
    }

    void set_assign_op(bool assign_op){
        this->assign_op = assign_op;
    }

    void set_as_function(){ //overloaded function for undefined function
        this->type_id = -1; // -1 for undefined

    }

    void set_undefined(bool undefined){
        if(undefined){
            set_as_function();
        }
    }


    ~symbol_info()
    {
        next = NULL; // Set next pointer to NULL to avoid dangling pointer        
    }
};