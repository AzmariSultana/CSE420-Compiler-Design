#include<bits/stdc++.h>
using namespace std;

class symbol_info
{
private:
    string name; //for variable, function, array name
    string type; 
    string data_type;
    symbol_info *next; // pointer to the next symbol in the linked list for collision resolution

    int array_size; // for arrays
    vector<pair<string, string>> parameters; // for functions, pair of parameter name and type

    int type_id; // 0 for variable, 1 for array, 2 for function

    // Write necessary attributes to store what type of symbol it is (variable/array/function)
    // Write necessary attributes to store the type/return type of the symbol (int/float/void/...)
    // Write necessary attributes to store the parameters of a function
    // Write necessary attributes to store the array size if the symbol is an array

public:
    symbol_info(string name, string type)
    {
        this->name = name;
        this->type = type;
        this->next = NULL;
        this->type_id = 0; // 0 for variable
        this->array_size = 0;
    }

    symbol_info(string name, string type, string data_type){
        this->name = name;
        this->type = type;
        this->data_type = data_type;
        this->next = NULL;
        this->type_id = 0; // 0 for variable
        this->array_size = 0;
    }

    //for array
    symbol_info(string name, string type, string data_type, int array_size){
        this->name = name;
        this->type = type;
        this->data_type = data_type;
        this->array_size = array_size;
        this->next = NULL;
        this->type_id = 1; // 1 for array
    }
    //for function
    symbol_info(string name, string type, string return_type, vector<pair<string, string>> parameters){
        this->name = name;
        this->type = type;
        this->data_type = return_type;
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

    bool get_is_array(){
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

    string get_return_type(){
        return data_type; 
    }

    string get_data_type(){
        return data_type;
    }



    void set_name(string name)
    {
        this->name = name;
    }
    void set_type(string type)
    {
        this->type = type;
    }
    // Write necessary functions to set and get the attributes
    void set_data_type(string data_type){
        this->data_type = data_type;
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
        this->data_type = return_type;
        this->parameters = parameters;
        this->type_id = 2; // 2 for function
    }


    ~symbol_info()
    {
        // Write necessary code to deallocate memory, if necessary
        if(next != NULL) {
            delete next;
        }
    }
};