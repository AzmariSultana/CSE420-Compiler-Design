%{

#include"symbol_info.h"

#define YYSTYPE symbol_info*

int yyparse(void);
int yylex(void);

extern FILE *yyin;


ofstream outlog;

string line_num;

// declare any other variables or functions needed here

%}

%token IF ELSE FOR

%%

start : program
	{
		outlog<<"At line no: "<<line_num<<" start : program "<<endl<<endl;
	}
	;

program : program unit
	{
		outlog<<"At line no: "<<line_num<<" program : program unit "<<endl<<endl;
		outlog<<$1->getnameofsymbol()+"\n"+$2->getnameofsymbol()<<endl<<endl;
		
		$$ = new symbol_info($1->getnameofsymbol()+"\n"+$2->getnameofsymbol(),"program");
	}
	| unit
	{

	}
	;

func_definition : type_specifier ID LPAREN param_list RPAREN compound_statement
		{	

		}
		| type_specifier ID LPAREN RPAREN compound_statement
		{
			
			outlog<<"At line no: "<<line_num<<" func_definition : type_specifier ID LPAREN RPAREN compound_statement "<<endl<<endl;
			outlog<<$1->getnameofsymbol()<<" "<<$2->getnameofsymbol()<<"()\n"<<$5->getnameofsymbol<<endl<<endl;
			
			$$ = new symbol_info($1->getnameofsymbol+" "+$2->getnameofsymbol()+"()\n"+$5->getnameofsymbol(),"func_def");	
		}
 		;

statement : FOR LPAREN expression_statement expression_statement expression RPAREN statement
	  {
	    	outlog<<"At line no: "<<line_num<<" statement : FOR LPAREN expression_statement expression_statement expression RPAREN statement "<<endl<<endl;
			outlog<<"for("<<$3->getnameofsymbol()<<$4->getnameofsymbol()<<$5->getnameofsymbol()<<")\n"<<$7->getnameofsymbol()<<endl<<endl;
			
			$$ = new symbol_info("for("+$3->getnameofsymbol()+$4->getnameofsymbol()+$5->getnameofsymbol()+")\n"+$7->getnameofsymbol(),"stmnt");
	  }

%%

int main(int c, char *v[])
{
	if(c != 2) 
	{
		  // check if filename given
	}
	yyin = fopen(v[1], "r");
	outlog.open("my_log.txt", ios::trunc);
	
	if(yyin == NULL)
	{
		cout<<"Couldn't open file"<<endl;
		return 0;
	}
    
	yyparse();
	
	//print number of total lines
	
	outlog.close();
	
	fclose(yyin);
	
	return 0;
}