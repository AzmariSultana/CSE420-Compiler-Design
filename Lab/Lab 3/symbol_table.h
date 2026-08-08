#include "scope_table.h"

class symbol_table
{
private:
    scope_table *current_scope;
    int table_size;
    int current_scope_id;

public:
    symbol_table(int table_size){
        this->table_size = table_size;
        this->current_scope_id = 0;
        current_scope = NULL;
        enter_scope();
    }
    
    void enter_scope(){
        current_scope_id++;
        scope_table *new_scope = new scope_table(table_size, current_scope_id, current_scope);
        current_scope = new_scope;
    }
    void exit_scope(){
        if(current_scope != NULL){
            scope_table *temp = current_scope;
            current_scope = current_scope->get_parent_scope();
            delete temp;
        }
    }

    bool insert(symbol_info* symbol){
        if(current_scope == NULL){
            return false; // no current scope to insert
        }
        if(current_scope->insert_in_scope(symbol)){
            return true; // symbol inserted
        } 
        else {
            return false; // symbol already exists
        }
    }

    
    symbol_info* lookup(symbol_info* symbol){
        if(current_scope == NULL){
            return NULL; // no current scope to lookup
        }
        scope_table *temp = current_scope;
        while(temp != NULL){
            symbol_info* found_symbol = temp->lookup_in_scope(symbol);
            if(found_symbol != NULL){
                return found_symbol; // symbol found in current scope
            }
            temp = temp->get_parent_scope(); // move to parent scope
        }
        return NULL; // symbol not found in any scope
    }

    void print_current_scope(){
        if(current_scope != NULL){
            outlog << "################################" << endl << endl;

            //print all scope from current scope to top
            scope_table *temp = current_scope;
            while(temp != NULL){
                temp->print_scope_table(outlog);
                temp = temp->get_parent_scope(); // move to parent scope
            }

            outlog << "################################" << endl << endl;
        }
    }

    void print_all_scopes(ofstream& outlog){
        // outlog << "Symbol Table" << endl << endl;
        outlog << "################################" << endl << endl;

        scope_table *temp = current_scope;
        while(temp != NULL){
            temp->print_scope_table(outlog);
            temp = temp->get_parent_scope(); // move to parent scope
        }

        outlog << "################################" << endl << endl;
    }

    // you can add more methods if you need 

    symbol_info *lookup_current_scope(symbol_info *symbol){
        if(current_scope == NULL){
            return NULL; // no current scope to lookup
        }
        return current_scope->lookup_in_scope(symbol); // lookup in current scope only
    }
};
