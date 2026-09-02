#ifndef AST_H
#define AST_H

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <map>

using namespace std;

class ASTNode {
public:
    virtual ~ASTNode() {}
    virtual string generate_code(ofstream& outcode, map<string, string>& symbol_to_temp, int& temp_count, int& label_count) const = 0;
};

// Expression node types

class ExprNode : public ASTNode {
protected:
    string node_type; // Type information (int, float, void, etc.)
public:
    ExprNode(string type) : node_type(type) {}
    virtual string get_type() const { return node_type; }
};

// Variable node (for ID references)

class VarNode : public ExprNode {
private:
    string name;
    ExprNode* index; // For array access, nullptr for simple variables

public:
    VarNode(string name, string type, ExprNode* idx = nullptr)
        : ExprNode(type), name(name), index(idx) {}
    
    ~VarNode() { if(index) delete index; }
    
    bool has_index() const { return index != nullptr; }
    
    string generate_index_code(ofstream& outcode, map<string, string>& symbol_to_temp,
                              int& temp_count, int& label_count) const {
        // TODO: Implement this method
        // Should generate code to calculate the array index and return the temp variable
        if(index){
            string index_code = index->generate_code(outcode, symbol_to_temp, temp_count, label_count);
            if(index_code.empty()){
                string temp_idx_zero = "t" + to_string(temp_count++);
                outcode << temp_idx_zero << " = 0" << endl; 
                return temp_idx_zero;
            }

            int index_size = 4; 
            if(node_type == "float"){ 
                index_size = 8; 
            }
            else if(node_type == "char" || node_type == "bool"){ 
                index_size = 1; 
            }

            if(index_size == 1){ 
                return index_code; 
            }

            string temp_offset = "t" + to_string(temp_count++);
            outcode << temp_offset << " = " << index_code << " * " << index_size << endl;
            return temp_offset;
        }
        return "";
    }
    
    string generate_code(ofstream& outcode, map<string, string>& symbol_to_temp,
                        int& temp_count, int& label_count) const override {
        // TODO: Implement this method
        // Should generate code for variable access or array access
        if (has_index()) {
            string index_offset_temp = generate_index_code(outcode, symbol_to_temp, temp_count, label_count);
            string result_temp = "t" + to_string(temp_count++);
            outcode << result_temp << " = " << name << "[" << index_offset_temp << "]" << endl;
            return result_temp;
        } else {
        auto it = symbol_to_temp.find(name);
        if (it != symbol_to_temp.end()) {
            temp_count++;
            return it->second;
        }
        string new_temp = "t" + to_string(temp_count++);
        outcode << new_temp << " = " << name << endl;
        symbol_to_temp[name] = new_temp;
        return new_temp;
        }    
    }
    
    string get_name() const { return name; }
};

// Constant node

class ConstNode : public ExprNode {
private:
    string value;

public:
    ConstNode(string val, string type) : ExprNode(type), value(val) {}
    
    string generate_code(ofstream& outcode, map<string, string>& symbol_to_temp,
                        int& temp_count, int& label_count) const override {
        // TODO: Implement this method
        // Should generate code for constant values
        string temp = "t" + to_string(temp_count);
        temp_count++;
        outcode << temp << " = " << value;
        if(node_type == "float" && value.find('.') == string::npos){
            outcode << ".0"; // Add .0 for float constants
        }
        outcode << endl;
        return temp;
    }
};

// Binary operation node

class BinaryOpNode : public ExprNode {
private:
    string op;
    ExprNode* left;
    ExprNode* right;

public:
    BinaryOpNode(string op, ExprNode* left, ExprNode* right, string result_type)
        : ExprNode(result_type), op(op), left(left), right(right) {}
    
    ~BinaryOpNode() {
        delete left;
        delete right;
    }
    
    string generate_code(ofstream& outcode, map<string, string>& symbol_to_temp,
                        int& temp_count, int& label_count) const override {
        // TODO: Implement this method
        // Should generate code for binary operations
        string left_code = left->generate_code(outcode, symbol_to_temp, temp_count, label_count);
        string right_code = right->generate_code(outcode, symbol_to_temp, temp_count, label_count);
        string temp = "t" + to_string(temp_count);
        temp_count++;
        outcode << temp << " = " << left_code << " " << op << " " << right_code << endl;
        return temp;
    }
};

// Unary operation node

class UnaryOpNode : public ExprNode {
private:
    string op;
    ExprNode* expr;

public:
    UnaryOpNode(string op, ExprNode* expr, string result_type)
        : ExprNode(result_type), op(op), expr(expr) {}
    
    ~UnaryOpNode() { delete expr; }
    
    string generate_code(ofstream& outcode, map<string, string>& symbol_to_temp,
                        int& temp_count, int& label_count) const override {
        // TODO: Implement this method
        // Should generate code for unary operations
        string expr_code = expr->generate_code(outcode, symbol_to_temp, temp_count, label_count);
        string temp = "t" + to_string(temp_count);
        temp_count++;
        if(op == "+"){
            outcode << temp << " = " << expr_code << endl;
        }
        else{
            outcode << temp << " = " << op << expr_code << endl;
        }
        return temp;
    }
};

// Assignment node

class AssignNode : public ExprNode {
private:
    VarNode* lhs;
    ExprNode* rhs;

public:
    AssignNode(VarNode* lhs, ExprNode* rhs, string result_type)
        : ExprNode(result_type), lhs(lhs), rhs(rhs) {}
    
    ~AssignNode() {
        delete lhs;
        delete rhs;
    }
    
    string generate_code(ofstream& outcode, map<string, string>& symbol_to_temp,
                        int& temp_count, int& label_count) const override {
        string rhs_val_temp = rhs->generate_code(outcode, symbol_to_temp, temp_count, label_count);

        if (!lhs->has_index()) {
            string lhs_var_name = lhs->get_name();
            outcode << lhs_var_name << " = " << rhs_val_temp << endl;
            symbol_to_temp[lhs_var_name] = rhs_val_temp;
            return rhs_val_temp; // Return the temporary holding the assigned value.
        } else {
            string index_offset_temp = lhs->generate_index_code(outcode, symbol_to_temp, temp_count, label_count);
            outcode << lhs->get_name() << "[" << index_offset_temp << "]" << " = " << rhs_val_temp << endl;
            // Note: symbol_to_temp for individual array elements is not explicitly tracked here.
            return rhs_val_temp;
        }
        // TODO: Implement this method
        // Should generate code for assignment operations
    }
};

// Statement node types

class StmtNode : public ASTNode {
public:
    virtual string generate_code(ofstream& outcode, map<string, string>& symbol_to_temp,
                                int& temp_count, int& label_count) const = 0;
};

// Expression statement node

class ExprStmtNode : public StmtNode {
private:
    ExprNode* expr;

public:
    ExprStmtNode(ExprNode* e) : expr(e) {}
    ~ExprStmtNode() { if(expr) delete expr; }
    
    string generate_code(ofstream& outcode, map<string, string>& symbol_to_temp,
                        int& temp_count, int& label_count) const override {
        // TODO: Implement this method
        // Should generate code for expression statements
        if(expr){
            return expr->generate_code(outcode, symbol_to_temp, temp_count, label_count);
        }
        return ""; // No code generated for expression statements
    }
};

// Block (compound statement) node

class BlockNode : public StmtNode {
private:
    vector<StmtNode*> statements;

public:
    ~BlockNode() {
        for (auto stmt : statements) {
            delete stmt;
        }
    }
    
    void add_statement(StmtNode* stmt) {
        if (stmt) statements.push_back(stmt);
    }
    
    string generate_code(ofstream& outcode, map<string, string>& symbol_to_temp,
                        int& temp_count, int& label_count) const override {
        // TODO: Implement this method
        // Should generate code for all statements in the block
        for (auto stmt : statements){
            if(stmt){
                stmt->generate_code(outcode, symbol_to_temp, temp_count, label_count);
            }
        }
        return "";
    }
};

// If statement node

class IfNode : public StmtNode {
private:
    ExprNode* condition;
    StmtNode* then_block;
    StmtNode* else_block; // nullptr if no else part

public:
    IfNode(ExprNode* cond, StmtNode* then_stmt, StmtNode* else_stmt = nullptr)
        : condition(cond), then_block(then_stmt), else_block(else_stmt) {}
    
    ~IfNode() {
        delete condition;
        delete then_block;
        if (else_block) delete else_block;
    }
    
    string generate_code(ofstream& outcode, map<string, string>& symbol_to_temp,
                        int& temp_count, int& label_count) const override {
        // TODO: Implement this method
        // Should generate code for if-else statements
        string label_then = "L" + to_string(label_count++);
        string label_else = "L" + to_string(label_count++);
        string label_end = "L" + to_string(label_count++);        
        
        // Generate code for the condition using the outer scope's symbol table
        string cond_code = condition->generate_code(outcode, symbol_to_temp, temp_count, label_count);
        
        outcode << "if " << cond_code << " goto " << label_then << endl;
        outcode << "goto " << label_else << endl;

        // Then block with a copy of the symbol table
        map<string, string> then_symbol_to_temp = symbol_to_temp;
        outcode << label_then << ":" << endl;
        if (then_block) {
            then_block->generate_code(outcode, then_symbol_to_temp, temp_count, label_count);
        }
        outcode << "goto " << label_end << endl;

        // Else block with a copy of the symbol table
        map<string, string> else_symbol_to_temp = symbol_to_temp;
        outcode << label_else << ":" << endl;
        if (else_block) {
            else_block->generate_code(outcode, else_symbol_to_temp, temp_count, label_count);
        }

        outcode << label_end << ":" << endl;
        return "";
    }
};

// While statement node

class WhileNode : public StmtNode {
private:
    ExprNode* condition;
    StmtNode* body;

public:
    WhileNode(ExprNode* cond, StmtNode* body_stmt)
        : condition(cond), body(body_stmt) {}
    
    ~WhileNode() {
        delete condition;
        delete body;
    }
    
    string generate_code(ofstream& outcode, map<string, string>& symbol_to_temp,
                        int& temp_count, int& label_count) const override {
        // TODO: Implement this method
        // Should generate code for while loops
        string label_cond = "L" + to_string(label_count++);
        string label_body = "L" + to_string(label_count++);
        string label_exit = "L" + to_string(label_count++);
        outcode << label_cond << ":" << endl;
        string cond_temp = condition->generate_code(outcode, symbol_to_temp, temp_count, label_count);
        outcode << "if " << cond_temp << " goto " << label_body << endl;
        outcode << "goto " << label_exit << endl;
        outcode << label_body << ":" << endl;
        if (body) {
            body->generate_code(outcode, symbol_to_temp, temp_count, label_count);
        }
        outcode << "goto " << label_cond << endl;
        outcode << label_exit << ":" << endl;
        return "";
    }
};

// For statement node

class ForNode : public StmtNode {
private:
    ExprNode* init;
    ExprNode* condition;
    ExprNode* update;
    StmtNode* body;

public:
    ForNode(ExprNode* init_expr, ExprNode* cond_expr, ExprNode* update_expr, StmtNode* body_stmt)
        : init(init_expr), condition(cond_expr), update(update_expr), body(body_stmt) {}
    
    ~ForNode() {
        if (init) delete init;
        if (condition) delete condition;
        if (update) delete update;
        delete body;
    }
    
    string generate_code(ofstream& outcode, map<string, string>& symbol_to_temp,
                        int& temp_count, int& label_count) const override {
        // TODO: Implement this method
        // Should generate code for for loops

        // Initialization (e.g., t11 = 0; i = t11;)
        if (init) {
            init->generate_code(outcode, symbol_to_temp, temp_count, label_count);
        }
        string label_cond = "L" + to_string(label_count++);
        string label_body = "L" + to_string(label_count++);
        string label_exit = "L" + to_string(label_count++);
        outcode << label_cond << ":" << endl;
        symbol_to_temp.clear();
        string cond_temp;
        if (condition) {
            cond_temp = condition->generate_code(outcode, symbol_to_temp, temp_count, label_count);
        } else {
            cond_temp = "1";
        }
        outcode << "if " << cond_temp << " goto " << label_body << endl;
        outcode << "goto " << label_exit << endl;
        outcode << label_body << ":" << endl;
        if (body) {
            body->generate_code(outcode, symbol_to_temp, temp_count, label_count);
        }
        if (update) {
            update->generate_code(outcode, symbol_to_temp, temp_count, label_count);
        }
        outcode << "goto " << label_cond << endl;
        outcode << label_exit << ":" << endl;
        return "";
    }
};

// Return statement node

class ReturnNode : public StmtNode {
private:
    ExprNode* expr;

public:
    ReturnNode(ExprNode* e) : expr(e) {}
    ~ReturnNode() { if (expr) delete expr; }
    
    string generate_code(ofstream& outcode, map<string, string>& symbol_to_temp,
                        int& temp_count, int& label_count) const override {
        // TODO: Implement this method
        // Should generate code for return statements
        if (expr){
            string expr_code = expr->generate_code(outcode, symbol_to_temp, temp_count, label_count);
            outcode << "return " << expr_code << endl;
        } else {
            outcode << "return" << endl;
        }
        return "";
    }
};

// Declaration node

class DeclNode : public StmtNode {
private:
    string type;
    vector<pair<string, int>> vars; // Variable name and array size (0 for regular vars)

public:
    DeclNode(string t) : type(t) {}
    
    void add_var(string name, int array_size = 0) {
        vars.push_back(make_pair(name, array_size));
    }
    
    string generate_code(ofstream& outcode, map<string, string>& symbol_to_temp,
                        int& temp_count, int& label_count) const override {
        // TODO: Implement this method
        // Should generate code for variable declarations
        for(const auto & var : vars){
            outcode << "// Declaration: " << type << " " << var.first;
            if(var.second > 0) {
                outcode << "[" << var.second << "]";
            }
            outcode << endl;
            if(var.second > 0){
                symbol_to_temp[var.first] = var.first + "_array";
            }
        }
        return "";
    }
    
    string get_type() const { return type; }
    const vector<pair<string, int>>& get_vars() const { return vars; }
};

// Function declaration node

class FuncDeclNode : public ASTNode {
private:
    string return_type;
    string name;
    vector<pair<string, string>> params; // Parameter type and name
    BlockNode* body;

public:
    FuncDeclNode(string ret_type, string n) : return_type(ret_type), name(n), body(nullptr) {}
    ~FuncDeclNode() { if (body) delete body; }
    
    void add_param(string type, string name) {
        params.push_back(make_pair(type, name));
    }
    
    void set_body(BlockNode* b) {
        body = b;
    }
    
    string generate_code(ofstream& outcode, map<string, string>& symbol_to_temp,
                        int& temp_count, int& label_count) const override {
        // TODO: Implement this method
        // Should generate code for function declarations
        outcode << "\n// Function: " << return_type << " " << name << "(";
        for (size_t i = 0; i < params.size(); ++i) {
            outcode << params[i].first << " " << params[i].second;
            if (i < params.size() - 1) {
                outcode << ", ";
            }
        }
        outcode << ")" << endl;

        map<string, string> local_symbol_to_temp;
        for (size_t i = 0; i < params.size(); ++i){
            string temp = "t" + to_string(temp_count + i);
            outcode << temp << " = " << params[i].second << endl;
            local_symbol_to_temp[params[i].second] = temp; // Store the parameter name
        }

        if (body) {
            body->generate_code(outcode, local_symbol_to_temp, temp_count, label_count);
        }
        return "";
    }
};

// Helper class for function arguments

class ArgumentsNode : public ASTNode {
private:
    vector<ExprNode*> args;

public:
    ~ArgumentsNode() {
        // Don't delete args here - they'll be transferred to FuncCallNode
    }
    
    void add_argument(ExprNode* arg) {
        if (arg) args.push_back(arg);
    }
    
    ExprNode* get_argument(int index) const {
        if (index >= 0 && index < args.size()) {
            return args[index];
        }
        return nullptr;
    }
    
    size_t size() const {
        return args.size();
    }
    
    const vector<ExprNode*>& get_arguments() const {
        return args;
    }
    
    string generate_code(ofstream& outcode, map<string, string>& symbol_to_temp,
                        int& temp_count, int& label_count) const override {
        // This node doesn't generate code directly
        return "";
    }
};

// Function call node

class FuncCallNode : public ExprNode {
private:
    string func_name;
    vector<ExprNode*> arguments;

public:
    FuncCallNode(string name, string result_type)
        : ExprNode(result_type), func_name(name) {}
    
    ~FuncCallNode() {
        for (auto arg : arguments) {
            delete arg;
        }
    }
    
    void add_argument(ExprNode* arg) {
        if (arg) arguments.push_back(arg);
    }
    
    string generate_code(ofstream& outcode, map<string, string>& symbol_to_temp,
                        int& temp_count, int& label_count) const override {
        // TODO: Implement this method
        // Should generate code for function calls
        vector<string> arg_codes;

        for (auto arg : arguments) {
            auto var_node = dynamic_cast<VarNode*>(arg);

            if (var_node) {
                string temp_code = "t" + to_string(temp_count++);
                outcode << temp_code << " = " << var_node->get_name() << endl;
                arg_codes.push_back(temp_code);
                outcode << "param " << temp_code << endl;
            }
            else {
                string temp = arg->generate_code(
                    outcode,
                    symbol_to_temp,
                    temp_count,
                    label_count
                );

                arg_codes.push_back(temp);
                outcode << "param " << temp << endl;
            }
        }
        string result_temp = "t" + to_string(temp_count);
        temp_count++;
        outcode << result_temp << " = call " << func_name << ", " << arguments.size() << endl;
        return result_temp;
    }
};

// Program node (root of AST)

class ProgramNode : public ASTNode {
private:
    vector<ASTNode*> units;

public:
    ~ProgramNode() {
        for (auto unit : units) {
            delete unit;
        }
    }
    
    void add_unit(ASTNode* unit) {
        if (unit) units.push_back(unit);
    }
    
    string generate_code(ofstream& outcode, map<string, string>& symbol_to_temp,
                        int& temp_count, int& label_count) const override {
        // TODO: Implement this method
        // Should generate code for the entire program
        for(auto unit : units){
            unit->generate_code(outcode, symbol_to_temp, temp_count, label_count);
        }
        return "";
    }
};

#endif // AST_H