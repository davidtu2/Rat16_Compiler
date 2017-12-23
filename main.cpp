/*
main.cpp

CPSC 323, Spring 2016, Prof. Song Choi, Assignment 3

NAME:			David Tu, Section 2 (david.tu2@csu.fullerton.edu)
DESCRIPTION:	Rat16 Compiler - Symbol table handling and assembly code generation implementation (Assignment 3)
REVISIONS:		Version 2
MODIFIED BY:	2016/05/09
*/

#include "LexicalAnalyzer.h"
#include <stack>

LexicalAnalyzer NewLA;
bool printSwitch = false;
//switch to indicate if I am declaring variables
bool declarationInitialize;
//switch to indicate if I am dealing with negative identifiers
bool isNegative = false;
int memory_address = 5000;
//save symbol (lexeme)
string save;
int addr;
int instr_addr;
string op;
//save symbol's (lexeme's) type
string type;

//prototypes
void rat16S();
void optionalFunctionDefinitions();
void functionDefinitions();
void functionDefinitionsPrime();
void function();
void optionalParameterList();
void parameterList();
void parameterListPrime();
void parameter();
void qualifier();
void body();
void optionalDeclarationList();
void declarationList();
void declarationListPrime();
void declaration();
void IDs();
void IDsPrime();
void statementList();
void statementListPrime();
void statement();
void compound();
void assign();
void ifRule();
void ifRulePrime();
void returnRule();
void returnRulePrime();
void write();
void read();
void whileRule();
void condition();
void relop();
void expression();
void expressionPrime();
void term();
void termPrime();
void factor();
void primary();
void primaryPrime();
void empty();
bool checkSym_Table(string lexeme);
void checkType(string op);
void insertSym(string type, string lexeme);
void printSym_Table();
int get_address(string lexeme);
string get_type(string lexeme);
void gen_instr(string instr, int address);
void back_patch(int jump_addr);
void printInstr();

//100 x 2 matrix. Column 1 and 2 are symbol (lexeme) and type, respectively. The Memory Address = index + memory address offset
string symTable[100][2];
//symbol table's index
int s = 0;

//300 x 2 matrix. Column 1 and 2 are OP and Oprd, respectively
string instrTable[301][2];
//instructions will start at index 1. So, Row 0 will be ignored
int d = 1;

stack <int> jumpStack;
//stack used for type checking
stack <string> typeStack;

int main()
{
	if (NewLA.readFileOpen()) {
		NewLA.lexer();
		rat16S();
	}
	else {
		cout << "ERROR: Cannot open the file" << endl;
		exit(1);
	}
	printSym_Table();
	printInstr();
	NewLA.closeReadFile();
	NewLA.closeOutputFile();
	return 0;
}

void rat16S() {
	if (printSwitch) {
		NewLA.myOutputFile << "<Rat16S> -> <Opt Function Definitions> $$ <Opt Declaration List> $$ <Statement List> $$" << endl;
	}
	optionalFunctionDefinitions();
	if (NewLA.getLexeme() == "$$") {
		NewLA.lexer();
		//set to true since we are declaring variables (if any)
		declarationInitialize = true;
		optionalDeclarationList();
	}
	else {
		NewLA.myOutputFile << "ERROR at rat16S(): $$ expected. Received Token: " <<NewLA.getToken() << " and Lexeme: " << NewLA.getLexeme() << endl;
		NewLA.closeReadFile();
		NewLA.closeOutputFile();
		exit(1);
	}
	if (NewLA.getLexeme() == "$$") {
		NewLA.lexer();
		//set to false since we are done declaring variables
		declarationInitialize = false;
		statementList();
	}
	else {
		NewLA.myOutputFile << "ERROR at rat16S(): $$ expected. Received Token: " << NewLA.getToken() << " and Lexeme: " << NewLA.getLexeme() << endl;
		NewLA.closeReadFile();
		NewLA.closeOutputFile();
		exit(1);
	}
	if (NewLA.getLexeme() == "$$") {
		if (printSwitch) {
			NewLA.myOutputFile << "End of Source" << endl;
		}
	}
	else {
		NewLA.myOutputFile << "ERROR at rat16S(): Source doesn't end with $$. Received Token: " << NewLA.getToken() << " and Lexeme: " << NewLA.getLexeme() << endl;
		NewLA.closeReadFile();
		NewLA.closeOutputFile();
		exit(1);
	}
}

void optionalFunctionDefinitions() {
	if (printSwitch) {
		NewLA.myOutputFile << "<Opt Function Definitions> -> <Function Definitions> | <Empty>" << endl;
	}
	if (NewLA.getLexeme() == "function") {
		functionDefinitions();
	}
	else {
		empty();
	}
}

void functionDefinitions() {
	if (printSwitch) {
		NewLA.myOutputFile << "<Function Definitions> -> <Function> <Function Definitions'>" << endl;
	}
	function();
	functionDefinitionsPrime();
}

void functionDefinitionsPrime() {
	if (printSwitch) {
		NewLA.myOutputFile << "<Function Definitions'> -> <Empty> | <Funtion Definitions>" << endl;
	}
	if (NewLA.getLexeme() == "$$") {
		empty();
	}
	else {
		functionDefinitions();
	}
}

void function() {
	if (printSwitch) {
		NewLA.myOutputFile << "<Function> -> function <Identifier> ( <Opt Parameter List> ) <Opt Declaration List> <Body>" << endl;
	}
	if (NewLA.getLexeme() == "function") {
		NewLA.lexer();
		if (NewLA.getToken() == "Identifier") {
			if (printSwitch) {
				NewLA.myOutputFile << "<Identifier> -> " << NewLA.getLexeme() << endl;
			}
			NewLA.lexer();
			if (NewLA.getLexeme() == "(") {
				NewLA.lexer();
				optionalParameterList();
				if (NewLA.getLexeme() == ")") {
					NewLA.lexer();
					optionalDeclarationList();
					body();
				}
				else {
					NewLA.myOutputFile << "ERROR at function(): ) expected. Received Token: " << NewLA.getToken() << " and Lexeme: " << NewLA.getLexeme() << endl;
					NewLA.closeReadFile();
					NewLA.closeOutputFile();
					exit(1);
				}
			}
			else {
				NewLA.myOutputFile << "ERROR at function(): ( expected. Received Token: " << NewLA.getToken() << " and Lexeme: " << NewLA.getLexeme() << endl;
				NewLA.closeReadFile();
				NewLA.closeOutputFile();
				exit(1);
			}
		}
		else {
			NewLA.myOutputFile << "ERROR at function(): Identifier expected. Received Token: " << NewLA.getToken() << " and Lexeme: " << NewLA.getLexeme() << endl;
			NewLA.closeReadFile();
			NewLA.closeOutputFile();
			exit(1);
		}
	}
	else {
		NewLA.myOutputFile << "ERROR at function(): 'function' expected. Received Token: " << NewLA.getToken() << " and Lexeme: " << NewLA.getLexeme() << endl;
		NewLA.closeReadFile();
		NewLA.closeOutputFile();
		exit(1);
	}
}

void optionalParameterList() {
	if (printSwitch) {
		NewLA.myOutputFile << "<Opt Parameter List> -> <Parameter List> | <Empty>" << endl;
	}
	if (NewLA.getToken() == "Identifier") {
		parameterList();
	}
	else {
		empty();
	}
}

void parameterList() {
	if (printSwitch) {
		NewLA.myOutputFile << "<Parameter List> -> <Parameter> <Parameter List'>" << endl;
	}
	parameter();
	parameterListPrime();
}

void parameterListPrime() {
	if (printSwitch) {
		NewLA.myOutputFile << "<Parameter List'> -> <Empty> | , <Parameter List>" << endl;
	}
	if (NewLA.getLexeme() == ",") {
		NewLA.lexer();
		parameterList();
	}
	else {
		empty();
	}
}

void parameter() {
	if (printSwitch) {
		NewLA.myOutputFile << "<Parameter> -> <IDs> : <Qualifier>" << endl;
	}
	IDs();
	if (NewLA.getLexeme() == ":") {
		NewLA.lexer();
		qualifier();
	}
}

void qualifier() {
	if (printSwitch) {
		NewLA.myOutputFile << "<Qualifier> -> integer | boolean | real" << endl;
	}
	//save type of identifier for symbol table
	type = NewLA.getLexeme();
	if (NewLA.getLexeme() == "integer") {
		if (printSwitch) {
			NewLA.myOutputFile << "<Qualifier> -> " << NewLA.getLexeme() << endl;
		}
		NewLA.lexer();
	}
	else if (NewLA.getLexeme() == "boolean") {
		if (printSwitch) {
			NewLA.myOutputFile << "<Qualifier> -> " << NewLA.getLexeme() << endl;
		}
		NewLA.lexer();
	}
	else if (NewLA.getLexeme() == "real") {
		if (printSwitch) {
			NewLA.myOutputFile << "<Qualifier> -> " << NewLA.getLexeme() << endl;
		}
		NewLA.lexer();
	}
	else {
		NewLA.myOutputFile << "ERROR at qualifier(): integer, boolean or real expected. Received Token: " << NewLA.getToken() << " and Lexeme: " << NewLA.getLexeme() << endl;
		NewLA.closeReadFile();
		NewLA.closeOutputFile();
		exit(1);
	}
}

void body() {
	if (printSwitch) {
		NewLA.myOutputFile << "<Body> -> { <Statement List> }" << endl;
	}
	if (NewLA.getLexeme() == "{") {
		NewLA.lexer();
		statementList();
		if (NewLA.getLexeme() == "}") {
			NewLA.lexer();
		}
		else {
			NewLA.myOutputFile << "ERROR at body(): } expected in body(). Received Token: " << NewLA.getToken() << " and Lexeme: " << NewLA.getLexeme() << endl;
			NewLA.closeReadFile();
			NewLA.closeOutputFile();
			exit(1);
		}
	}
	else {
		NewLA.myOutputFile << "ERROR at body(): { expected in body(). Received Token: " << NewLA.getToken() << " and Lexeme: " << NewLA.getLexeme() << endl;
		NewLA.closeReadFile();
		NewLA.closeOutputFile();
		exit(1);
	}
}

void optionalDeclarationList() {
	if (printSwitch) {
		NewLA.myOutputFile << "<Opt Declaration List> -> <Declaration List> | <Empty>" << endl;
	}
	if (NewLA.getLexeme() == "integer" | NewLA.getLexeme() == "boolean" | NewLA.getLexeme() == "real") {
		declarationList();
	}
	else {
		empty();
	}
}

void declarationList() {
	if (printSwitch) {
		NewLA.myOutputFile << "<Declaration List> -> <Declaration> ; <Declaration List'>" << endl;
	}
	declaration();
	if (NewLA.getLexeme() == ";") {
		NewLA.lexer();
		declarationListPrime();
	}
	else {
		NewLA.myOutputFile << "ERROR at declarationList(): ; expected at end of declaration at declarationList(). Received Token: " << NewLA.getToken() << " and Lexeme: " << NewLA.getLexeme() << endl;
		NewLA.closeReadFile();
		NewLA.closeOutputFile();
		exit(1);
	}
}

void declarationListPrime() {
	if (printSwitch) {
		NewLA.myOutputFile << "<Declaration List'> -> <Empty> | <Declaration List>" << endl;
	}
	if (NewLA.getLexeme() == "integer" | NewLA.getLexeme() == "boolean" | NewLA.getLexeme() == "real") {
		declarationList();
	}
	else {
		empty();
	}
}

void declaration() {
	if (printSwitch) {
		NewLA.myOutputFile << "<Declaration> -> <Qualifier> <IDs>" << endl;
	}
	qualifier();
	IDs();
}

void IDs() {
	if (printSwitch) {
		NewLA.myOutputFile << "<IDs> -> <Identifier> <IDs'>" << endl;
	}
	if (NewLA.getToken() == "Identifier") {
		if (printSwitch) {
			NewLA.myOutputFile << "<Identifier> -> " << NewLA.getLexeme() << endl;
		}
		//If true, we are declaring variables
		if (declarationInitialize) {
			if (checkSym_Table(NewLA.getLexeme())) {
				NewLA.myOutputFile << "ERROR at IDs(): Duplicate Symbol found" << endl;
				NewLA.closeReadFile();
				NewLA.closeOutputFile();
				exit(1);
			}
			else {
				insertSym(type, NewLA.getLexeme());
			}
		}//else, we are not declaring variables so we are in the body of the source - We need to check if symbol is declared and in the symbol table
		else {
			if (checkSym_Table(NewLA.getLexeme())) {//If true, the variables are in the table. Proceed parsing
				//SEMANTIC ACTION: for read() function which accesses IDs():
				gen_instr("STDIN", -999);
				addr = get_address(NewLA.getLexeme());
				gen_instr("POPM", addr);
			}
			else {
				NewLA.myOutputFile << "ERROR at IDs(): Symbol not found. Please declare your variable first" << endl;
				NewLA.closeReadFile();
				NewLA.closeOutputFile();
				exit(1);
			}
		}
		NewLA.lexer();
		IDsPrime();
	}
	else {
		NewLA.myOutputFile << "ERROR at IDs(): Identifier expected at IDs(). Received Token: " << NewLA.getToken() << " and Lexeme: " << NewLA.getLexeme() << endl;
		NewLA.closeReadFile();
		NewLA.closeOutputFile();
		exit(1);
	}
}

void IDsPrime() {
	if (printSwitch) {
		NewLA.myOutputFile << "<IDs'> -> <Empty> | , <IDs>" << endl;
	}
	if (NewLA.getLexeme() == ",") {
		NewLA.lexer();
		IDs();
	}
	else {
		empty();
	}
}

void statementList() {
	if (printSwitch) {
		NewLA.myOutputFile << "<Statement List> -> <Statement> <Statement List'>" << endl;
	}
	statement();
	statementListPrime();
}

void statementListPrime() {
	if (printSwitch) {
		NewLA.myOutputFile << "<Statement List'> -> <Empty> | <Statement List>" << endl;
	}
	//end of file marker for body and function declaration are $$ and }, respectively
	if (NewLA.getLexeme() == "$$" | NewLA.getLexeme() == "}") {
		empty();
	}
	else {
		statementList();
	}
}

void statement() {
	if (printSwitch) {
		NewLA.myOutputFile << "<Statement> -> <Compound> | <Assign> | <If> | <Return> | <Write> | <Read> | <While>" << endl;
	}
	if (NewLA.getLexeme() == "{") {
		compound();
	}
	else if (NewLA.getToken() == "Identifier") {
		assign();
	}
	else if (NewLA.getLexeme() == "if") {
		ifRule();
	}
	else if (NewLA.getLexeme() == "return") {
		returnRule();
	}
	else if (NewLA.getLexeme() == "printf") {
		write();
	}
	else if (NewLA.getLexeme() == "scanf") {
		read();
	}
	else if (NewLA.getLexeme() == "while") {
		whileRule();
	}
	else {
		NewLA.myOutputFile << "ERROR at statement(): {, Identifier, if, return, printf, scanf, or while expected at statement(). Received Token: " << NewLA.getToken() << " and Lexeme: " << NewLA.getLexeme() << endl;
		NewLA.closeReadFile();
		NewLA.closeOutputFile();
		exit(1);
	}
}

void compound() {
	if (printSwitch) {
		NewLA.myOutputFile << "<Compound> -> { <Statement List> }" << endl;
	}
	if (NewLA.getLexeme() == "{") {
		NewLA.lexer();
		statementList();
		if (NewLA.getLexeme() == "}") {
			NewLA.lexer();
		}
		else {
			NewLA.myOutputFile << "ERROR at compound(): } expected at compound(). Received Token: " << NewLA.getToken() << " and Lexeme: " << NewLA.getLexeme() << endl;
			NewLA.closeReadFile();
			NewLA.closeOutputFile();
			exit(1);
		}
	}
	else {
		NewLA.myOutputFile << "ERROR at compound(): { expected at compound(). Received Token: " << NewLA.getToken() << " and Lexeme: " << NewLA.getLexeme() << endl;
		NewLA.closeReadFile();
		NewLA.closeOutputFile();
		exit(1);
	}
}

void assign() {
	if (printSwitch) {
		NewLA.myOutputFile << "<Assign> -> <Identifier> := <Expression> ;" << endl;
	}
	if (NewLA.getToken() == "Identifier") {
		if (printSwitch) {
			NewLA.myOutputFile << "<Identifier> -> " << NewLA.getLexeme() << endl;
		}
		//SEMANTIC ACTION: save assignment symbol (lexeme)
		save = NewLA.getLexeme();
		NewLA.lexer();
		if (NewLA.getLexeme() == ":=") {
			NewLA.lexer();
			expression();
			//SEMANTIC ACTION: get the address and type of the saved symbol (lexeme)
			addr = get_address(save);
			type = get_type(save);
			//push into typeStack for checkType()
			typeStack.push(type);
			checkType("assignment");
			gen_instr("POPM", addr);
			if (NewLA.getLexeme() == ";") {
				NewLA.lexer();
			}
			else {
				NewLA.myOutputFile << "ERROR at assign(): ; expected at assign(). Received Token: " << NewLA.getToken() << " and Lexeme: " << NewLA.getLexeme() << endl;
				NewLA.closeReadFile();
				NewLA.closeOutputFile();
				exit(1);
			}
		}
		else {
			NewLA.myOutputFile << "ERROR at assign(): := expected at assign(). Received Token: " << NewLA.getToken() << " and Lexeme: " << NewLA.getLexeme() << endl;
			NewLA.closeReadFile();
			NewLA.closeOutputFile();
			exit(1);
		}
	}
	else {
		NewLA.myOutputFile << "ERROR at assign(): Identifier expected at assign(). Received Token: " << NewLA.getToken() << " and Lexeme: " << NewLA.getLexeme() << endl;
		NewLA.closeReadFile();
		NewLA.closeOutputFile();
		exit(1);
	}
}

void ifRule() {
	if (printSwitch) {
		NewLA.myOutputFile << "<If> -> if ( <Condition> ) <Statement> <If'>" << endl;
	}
	if (NewLA.getLexeme() == "if") {
		NewLA.lexer();
		if (NewLA.getLexeme() == "(") {
			NewLA.lexer();
			condition();
			if (NewLA.getLexeme() == ")") {
				NewLA.lexer();
				statement();
				ifRulePrime();
			}
			else {
				NewLA.myOutputFile << "ERROR at ifRule(): ) expected at ifRule(). Received Token: " << NewLA.getToken() << " and Lexeme: " << NewLA.getLexeme() << endl;
				NewLA.closeReadFile();
				NewLA.closeOutputFile();
				exit(1);
			}
		}
		else {
			NewLA.myOutputFile << "ERROR at ifRule(): ( expected at ifRule(). Received Token: " << NewLA.getToken() << " and Lexeme: " << NewLA.getLexeme() << endl;
			NewLA.closeReadFile();
			NewLA.closeOutputFile();
			exit(1);
		}
	}
	else {
		NewLA.myOutputFile << "ERROR at ifRule(): if expected at ifRule(). Received Token: " << NewLA.getToken() << " and Lexeme: " << NewLA.getLexeme() << endl;
		NewLA.closeReadFile();
		NewLA.closeOutputFile();
		exit(1);
	}
}

void ifRulePrime() {
	if (printSwitch) {
		NewLA.myOutputFile << "<If'> -> endif | else <Statement> endif" << endl;
	}
	if (NewLA.getLexeme() == "endif") {
		//SEMANTIC ACTION: go back and patch with the current instruction address, d
		back_patch(d);
		//LABEL indicates endif
		gen_instr("LABEL", -999);
		NewLA.lexer();
	} else if(NewLA.getLexeme() == "else") {
		//SEMANTIC ACTION: save the current instruction address, d to update absolute jump instruction. The absolute jump will skip over "else" part of the if statement
		jumpStack.push(d);
		gen_instr("JUMP", -999);
		//save the current instruction address, d, to update JUMPZ instruction
		instr_addr = d;
		NewLA.lexer();
		statement();
		if (NewLA.getLexeme() == "endif") {
			//SEMANTIC ACTION: update the JUMP instruction then the JUMPZ instruction
			back_patch(d);
			back_patch(instr_addr);
			//LABEL indicates endif
			gen_instr("LABEL", -999);
			NewLA.lexer();
		}
		else {
			NewLA.myOutputFile << "ERROR at ifRulePrime(): endif expected at ifRulePrime(). Received Token: " << NewLA.getToken() << " and Lexeme: " << NewLA.getLexeme() << endl;
			NewLA.closeReadFile();
			NewLA.closeOutputFile();
			exit(1);
		}
	}
	else {
		NewLA.myOutputFile << "ERROR at ifRulePrime(): endif or else expected at ifRulePrime(). Received Token: " << NewLA.getToken() << " and Lexeme: " << NewLA.getLexeme() << endl;
		NewLA.closeReadFile();
		NewLA.closeOutputFile();
		exit(1);
	}
}

void returnRule() {
	if (printSwitch) {
		NewLA.myOutputFile << "<Return> -> return <Return'>" << endl;
	}
	if (NewLA.getLexeme() == "return") {
		NewLA.lexer();
		returnRulePrime();
	}
	else {
		NewLA.myOutputFile << "ERROR at returnRule(): return expected in returnRule(). Received Token: " << NewLA.getToken() << " and Lexeme: " << NewLA.getLexeme() << endl;
		NewLA.closeReadFile();
		NewLA.closeOutputFile();
		exit(1);
	}
}

void returnRulePrime() {
	if (printSwitch) {
		NewLA.myOutputFile << "<Return'> -> ; | <Expression> ;" << endl;
	}
	if (NewLA.getLexeme() == ";") {
		NewLA.lexer();
	}
	else {
		expression();
		if (NewLA.getLexeme() == ";") {
			NewLA.lexer();
		}
		else {
			NewLA.myOutputFile << "ERROR at returnRulePrime(): ; expected in returnRulePrime(). Received Token: " << NewLA.getToken() << " and Lexeme: " << NewLA.getLexeme() << endl;
			NewLA.closeReadFile();
			NewLA.closeOutputFile();
			exit(1);
		}
	}
}

void write() {
	if (printSwitch) {
		NewLA.myOutputFile << "<Write> -> printf ( <Expression> ) ;" << endl;
	}
	if (NewLA.getLexeme() == "printf") {
		NewLA.lexer();
		if (NewLA.getLexeme() == "(") {
			NewLA.lexer();
			expression();
			//SEMANTIC ACTION: STDOUT pops an item off the stack to display
			gen_instr("STDOUT", -999);
			if (!typeStack.empty()) {
				typeStack.pop();
			}
			else {
				NewLA.myOutputFile << "ERROR at write(): typeStack is empty" << endl;
				NewLA.closeReadFile();
				NewLA.closeOutputFile();
				exit(1);
			}
			if (NewLA.getLexeme() == ")") {
				NewLA.lexer();
				if (NewLA.getLexeme() == ";") {
					NewLA.lexer();
				}
				else {
					NewLA.myOutputFile << "ERROR at write(): ; expected in write(). Received Token: " << NewLA.getToken() << " and Lexeme: " << NewLA.getLexeme() << endl;
					NewLA.closeReadFile();
					NewLA.closeOutputFile();
					exit(1);
				}
			}
			else {
				NewLA.myOutputFile << "ERROR at write(): ) expected in write(). Received Token: " << NewLA.getToken() << " and Lexeme: " << NewLA.getLexeme() << endl;
				NewLA.closeReadFile();
				NewLA.closeOutputFile();
				exit(1);
			}
		}
		else {
			NewLA.myOutputFile << "ERROR at write(): ( expected in write(). Received Token: " << NewLA.getToken() << " and Lexeme: " << NewLA.getLexeme() << endl;
			NewLA.closeReadFile();
			NewLA.closeOutputFile();
			exit(1);
		}
	}
	else {
		NewLA.myOutputFile << "ERROR at write(): printf expected in write(). Received Token: " << NewLA.getToken() << " and Lexeme: " << NewLA.getLexeme() << endl;
		NewLA.closeReadFile();
		NewLA.closeOutputFile();
		exit(1);
	}
}

void read() {
	if (printSwitch) {
		NewLA.myOutputFile << "<Read> -> scanf ( <IDs> ) ;" << endl;
	}
	if (NewLA.getLexeme() == "scanf") {
		NewLA.lexer();
		if (NewLA.getLexeme() == "(") {
			NewLA.lexer();
			IDs();
			if (NewLA.getLexeme() == ")") {
				NewLA.lexer();
				if (NewLA.getLexeme() == ";") {
					NewLA.lexer();
				}
				else {
					NewLA.myOutputFile << "ERROR at read(): ; expected in read(). Received Token: " << NewLA.getToken() << " and Lexeme: " << NewLA.getLexeme() << endl;
					NewLA.closeReadFile();
					NewLA.closeOutputFile();
					exit(1);
				}
			}
			else {
				NewLA.myOutputFile << "ERROR at read(): ) expected in read(). Received Token: " << NewLA.getToken() << " and Lexeme: " << NewLA.getLexeme() << endl;
				NewLA.closeReadFile();
				NewLA.closeOutputFile();
				exit(1);
			}
		}
		else {
			NewLA.myOutputFile << "ERROR at read(): ( expected in read(). Received Token: " << NewLA.getToken() << " and Lexeme: " << NewLA.getLexeme() << endl;
			NewLA.closeReadFile();
			NewLA.closeOutputFile();
			exit(1);
		}
	}
	else {
		NewLA.myOutputFile << "ERROR at read(): scanf expected in read(). Received Token: " << NewLA.getToken() << " and Lexeme: " << NewLA.getLexeme() << endl;
		NewLA.closeReadFile();
		NewLA.closeOutputFile();
		exit(1);
	}
}

void whileRule() {
	if (printSwitch) {
		NewLA.myOutputFile << "<While> -> while ( <Condition> ) <Statement>" << endl;
	}
	if (NewLA.getLexeme() == "while") {
		//SEMANTIC ACTION: save instruction address, d
		instr_addr = d;
		gen_instr("LABEL", -999);
		NewLA.lexer();
		if (NewLA.getLexeme() == "(") {
			NewLA.lexer();
			condition();
			if (NewLA.getLexeme() == ")") {
				NewLA.lexer();
				statement();
				//SEMANTIC ACTION: JUMP to LABEL's Instruction Address
				gen_instr("JUMP", instr_addr);
				//go back and patch with the current instruction address, d
				back_patch(d);
			}
			else {
				NewLA.myOutputFile << "ERROR at whileRule(): ) expected at whileRule(). Received Token: " << NewLA.getToken() << " and Lexeme: " << NewLA.getLexeme() << endl;
				NewLA.closeReadFile();
				NewLA.closeOutputFile();
				exit(1);
			}
		}
		else {
			NewLA.myOutputFile << "ERROR at whileRule(): ( expected at whileRule(). Received Token: " << NewLA.getToken() << " and Lexeme: " << NewLA.getLexeme() << endl;
			NewLA.closeReadFile();
			NewLA.closeOutputFile();
			exit(1);
		}
	}
	else {
		NewLA.myOutputFile << "ERROR at whileRule(): while expected at whileRule(). Received Token: " << NewLA.getToken() << " and Lexeme: " << NewLA.getLexeme() << endl;
		NewLA.closeReadFile();
		NewLA.closeOutputFile();
		exit(1);
	}
}

void condition() {
	if (printSwitch) {
		NewLA.myOutputFile << "<Condition> -> <Expression> <Relop> <Expression>" << endl;
	}
	expression();
	relop();
	expression();
	//Check to see if the operands match
	checkType("condition");
	//check for type to see if operands are booleans
	//SEMANTIC ACTION: Cases of Op:
	if (op == "<") {
		gen_instr("LES", -999);
		//save the current instruction address, d
		jumpStack.push(d);
		gen_instr("JUMPZ", -999);
	}
	else if (op == ">") {
		gen_instr("GRT", -999);
		//save the current instruction address, d
		jumpStack.push(d);
		gen_instr("JUMPZ", -999);
	}
	else if (op == "=") {
		gen_instr("EQU", -999);
		//save the current instruction address, d
		jumpStack.push(d);
		gen_instr("JUMPZ", -999);
	}
	else if (op == "!=") {
		gen_instr("NEQ", -999);
		//save the current instruction address, d
		jumpStack.push(d);
		gen_instr("JUMPZ", -999);
	}
	else if (op == "=>") {
		gen_instr("GEQ", -999);
		//save the current instruction address, d
		jumpStack.push(d);
		gen_instr("JUMPZ", -999);
	}
	else if (op == "<=") {
		gen_instr("LEQ", -999);
		//save the current instruction address, d
		jumpStack.push(d);
		gen_instr("JUMPZ", -999);
	}
	else {
		NewLA.myOutputFile << "ERROR at condition(): legal op expected" << endl;
		NewLA.closeReadFile();
		NewLA.closeOutputFile();
		exit(1);
	}
}

void relop() {
	if (printSwitch) {
		NewLA.myOutputFile << "<Relop> -> = | /= | > | < | => | <=" << endl;
	}
	if (NewLA.getLexeme() == "=") {
		if (printSwitch) {
			NewLA.myOutputFile << "<Relop> -> "<< NewLA.getLexeme() << endl;
		}
		//SEMANTIC ACTION: save the operation
		op = NewLA.getLexeme();
		NewLA.lexer();
	}
	else if (NewLA.getLexeme() == "/=") {
		if (printSwitch) {
			NewLA.myOutputFile << "<Relop> -> " << NewLA.getLexeme() << endl;
		}
		//SEMANTIC ACTION: save the operation
		op = NewLA.getLexeme();
		NewLA.lexer();
	}
	else if (NewLA.getLexeme() == ">") {
		if (printSwitch) {
			NewLA.myOutputFile << "<Relop> -> " << NewLA.getLexeme() << endl;
		}
		//SEMANTIC ACTION: save the operation
		op = NewLA.getLexeme();
		NewLA.lexer();
	}
	else if (NewLA.getLexeme() == "<") {
		if (printSwitch) {
			NewLA.myOutputFile << "<Relop> -> " << NewLA.getLexeme() << endl;
		}
		//SEMANTIC ACTION: save the operation
		op = NewLA.getLexeme();
		NewLA.lexer();
	}
	else if (NewLA.getLexeme() == "=>") {
		if (printSwitch) {
			NewLA.myOutputFile << "<Relop> -> " << NewLA.getLexeme() << endl;
		}
		//SEMANTIC ACTION: save the operation
		op = NewLA.getLexeme();
		NewLA.lexer();
	}
	else if (NewLA.getLexeme() == "<=") {
		if (printSwitch) {
			NewLA.myOutputFile << "<Relop> -> " << NewLA.getLexeme() << endl;
		}
		//SEMANTIC ACTION: save the operation
		op = NewLA.getLexeme();
		NewLA.lexer();
	}
	else {
		NewLA.myOutputFile << "ERROR at relop(): =, /=, >, <, =>, or <= expected at relop(). Received Token: " << NewLA.getToken() << " and Lexeme: " << NewLA.getLexeme() << endl;
		NewLA.closeReadFile();
		NewLA.closeOutputFile();
		exit(1);
	}
}

void expression() {
	if (printSwitch) {
		NewLA.myOutputFile << "<Expression> -> <Term> <Expression'>" << endl;
	}
	term();
	expressionPrime();
}

void expressionPrime() {
	if (printSwitch) {
		NewLA.myOutputFile << "<Expression'> -> + <Term> <Expression'> | - <Term> <Expression'> | <Empty>" << endl;
	}
	if (NewLA.getLexeme() == "+" | NewLA.getLexeme() == "-") {
		//SEMANTIC ACTION: save the operation
		string op2 = NewLA.getLexeme();
		NewLA.lexer();
		term();
		checkType("add/sub");
		//SEMANTIC ACTION
		if (op2 == "+") {
			gen_instr("ADD", -999);
		}
		else if (op2 == "-") {
			gen_instr("SUB", -999);
		}
		//push the type of the sum/difference into the stack
		typeStack.push(type);
		expressionPrime();
	}
	else {
		empty();
	}
}

void term() {
	if (printSwitch) {
		NewLA.myOutputFile << "<Term> -> <Factor> <Term'>" << endl;
	}
	factor();
	termPrime();
}

void termPrime() {
	if (printSwitch) {
		NewLA.myOutputFile << "<Term'> -> * <Factor> <Term'> | / <Factor> <Term'> | <Empty>" << endl;
	}
	if (NewLA.getLexeme() == "*" | NewLA.getLexeme() == "/") {
		//SEMANTIC ACTION: save the operation
		string op1 = NewLA.getLexeme();
		NewLA.lexer();
		factor();
		checkType("mul/div");
		//check for type to see if it's a boolean
		//SEMANTIC ACTION: Instructions generate BEFORE addition/subtraction, to preserve the order of operations
		if (op1 == "*") {
			//check for type to see if it's boolean
			gen_instr("MUL", -999);
		}
		else if (op1 == "/") {
			//check for type to see if it's boolean
			gen_instr("DIV", -999);
		}
		//push the type of the product/quotient into the stack
		typeStack.push(type);
		termPrime();
	}
	else {
		empty();
	}
}

void factor() {
	if (printSwitch) {
		NewLA.myOutputFile << "<Factor> -> - <Primary> | <Primary>" << endl;
	}
	if (NewLA.getLexeme() == "-") {
		//indicate that the identifier is a negative symbol (lexeme)
		isNegative = true;
		NewLA.lexer();
		primary();
	}
	else {
		primary();
	}
}

void primary() {
	if (printSwitch) {
		NewLA.myOutputFile << "<Primary> -> <Identifier> <Primary'> | <Integer> | ( <Expression> ) | <Real> | true | false" << endl;
	}
	if (NewLA.getToken() == "Identifier") {
		if (printSwitch) {
			NewLA.myOutputFile << "<Identifier> -> " << NewLA.getLexeme() << endl;
		}
		//SEMANTIC ACTION: gets the memory address and type of the symbol (lexeme) to push into the stack
		addr = get_address(NewLA.getLexeme());
		gen_instr("PUSHM", addr);
		//check if the identifier is negative via isNegative switch. If so, generate additional instructions to multiply it by -1, then reset the switch
		if (isNegative) {
			gen_instr("PUSHI", -1);
			gen_instr("MUL", -999);
			isNegative = false;
		}
		type = get_type(NewLA.getLexeme());
		typeStack.push(type);
		NewLA.lexer();
		primaryPrime();
	}
	else if (NewLA.getToken() == "Integer") {
		if (printSwitch) {
			NewLA.myOutputFile << "<Integer> ->" << NewLA.getLexeme() << endl;
		}
		//SEMANTIC ACTION: Push the integer into the stack. Note: needed to convert the symbol (lexeme) from a string to an integer
		gen_instr("PUSHI", stoi(NewLA.getLexeme()));
		//check if the integer is negative via isNegative switch. If so, generate additional instructions to multiply it by -1, then reset the switch
		if (isNegative) {
			gen_instr("PUSHI", -1);
			gen_instr("MUL", -999);
			isNegative = false;
		}
		typeStack.push("integer");
		NewLA.lexer();
	}
	else if (NewLA.getLexeme() == "(") {
		NewLA.lexer();
		expression();
		if (NewLA.getLexeme() == ")") {
			NewLA.lexer();
		}
		else {
			NewLA.myOutputFile << "ERROR at primary(): ) expected after <Expression> at primary(). Received Token: " << NewLA.getToken() << " and Lexeme: " << NewLA.getLexeme() << endl;
			NewLA.closeReadFile();
			NewLA.closeOutputFile();
			exit(1);
		}
	}
	else if (NewLA.getToken() == "Real") {
		if (printSwitch) {
			NewLA.myOutputFile << "<Real> ->" << NewLA.getLexeme() << endl;
		}
		NewLA.lexer();
	}
	else if (NewLA.getLexeme() == "true") {
		//SEMANTIC ACTION: Push boolean into the stack. True = 1. Since 1 is an integer, PUSHI is used
		gen_instr("PUSHI", 1);
		//check if the boolean is negative via isNegative switch. If so, generate generate an error
		if (isNegative) {
			NewLA.myOutputFile << "ERROR at primary(): Booleans not allowed in arithmetic expressions" << endl;
			NewLA.closeReadFile();
			NewLA.closeOutputFile();
			exit(1);
		}
		typeStack.push("boolean");
		NewLA.lexer();
	}
	else if (NewLA.getLexeme() == "false") {
		//SEMANTIC ACTION: Push boolean into the stack. False = 0. Since 0 is an integer, PUSHI is used
		gen_instr("PUSHI", 0);
		//check if the boolean is negative via isNegative switch. If so, generate generate an error
		if (isNegative) {
			NewLA.myOutputFile << "ERROR at primary(): Booleans not allowed in arithmetic expressions" << endl;
			NewLA.closeReadFile();
			NewLA.closeOutputFile();
			exit(1);
		}
		typeStack.push("boolean");
		NewLA.lexer();
	}
	else {
		NewLA.myOutputFile << "ERROR at primary(): Identifier, Integer, (, Real, true, or false expected at primary(). Received Token: " << NewLA.getToken() << " and Lexeme: " << NewLA.getLexeme() << endl;
		NewLA.closeReadFile();
		NewLA.closeOutputFile();
		exit(1);
	}
}

void primaryPrime() {
	if (printSwitch) {
		NewLA.myOutputFile << "<Primary'> -> <Empty> | ( <IDs> )" << endl;
	}
	if (NewLA.getLexeme() == "(") {
		NewLA.lexer();
		IDs();
		if (NewLA.getLexeme() == ")") {
			NewLA.lexer();
		}
		else {
			NewLA.myOutputFile << "ERROR at primaryPrime(): ) expected after <IDs> at primaryPrime(). Received Token: " << NewLA.getToken() << " and Lexeme: " << NewLA.getLexeme() << endl;
			NewLA.closeReadFile();
			NewLA.closeOutputFile();
			exit(1);
		}
	}
	else {
		empty();
	}
}

void empty() {
	if (printSwitch) {
		NewLA.myOutputFile << "<Empty> -> <Empty>" << endl;
	}
}

//checks symbol table if given symbol (lexeme) exists
bool checkSym_Table(string lexeme) {
	bool found = false;
	for (int i = 0; i < s; i++) {
		if (symTable[i][1] == lexeme) {
			found = true;
		}
	}
	return found;
}

//checks for a type match of the last two items in the typeStack. "type" is saved for a results push into the typeStack after gen_instr() of a arithmetic operation
void checkType(string op) {
	if (!typeStack.empty()) {
		//save for future use
		type = typeStack.top();
		typeStack.pop();
	}
	else {
		NewLA.myOutputFile << "ERROR at checkType(): typeStack is empty" << endl;
		NewLA.closeReadFile();
		NewLA.closeOutputFile();
		exit(1);
	}
	if (!typeStack.empty()) {
		if (type == typeStack.top()) {
			typeStack.pop();
		}
		else {
			NewLA.myOutputFile << "ERROR at checkType(): Type mismatch error" << endl;
			NewLA.closeReadFile();
			NewLA.closeOutputFile();
			exit(1);
		}
	}
	else {
		NewLA.myOutputFile << "ERROR at checkType(): typeStack is empty" << endl;
		NewLA.closeReadFile();
		NewLA.closeOutputFile();
		exit(1);
	}
	//if operation (op) this function is invoked from is not assign(), then also check for booleans (for add/sub, mul/div or condition)
	if (op != "assignment") {
		if (type == "boolean") {
			NewLA.myOutputFile << "ERROR at checkType(): Booleans not allowed in arithmetic/conditional expressions" << endl;
			NewLA.closeReadFile();
			NewLA.closeOutputFile();
			exit(1);
		}
	}
}

void insertSym(string type, string lexeme) {
	//type is defined in qualifier()
	if (type != "integer" & type != "boolean" & type != "real") {
		NewLA.myOutputFile << "ERROR at insertSym(): integer/boolean/real type expected" << endl;
		NewLA.closeReadFile();
		NewLA.closeOutputFile();
		exit(1);
	}
	symTable[s][1] = lexeme;
	symTable[s][2] = type;
	s++;
}

void printSym_Table() {
	NewLA.myOutputFile << left << setw(15) << "Symbol" << left << setw(15) << "Type" << "Memory Address" << endl;
	for (int i = 0; i < s; i++) {
		NewLA.myOutputFile << left << setw(15) << symTable[i][1] << left << setw(15) << symTable[i][2] << i + memory_address << endl;
	}
	NewLA.myOutputFile << endl;
}

int get_address(string lexeme) {
	int addr = 0;
	for (int i = 0; i < s; i++) {
		if (symTable[i][1] == lexeme) {
			addr = i + memory_address;
		}
	}
	if (addr == 0) {
		NewLA.myOutputFile << "ERROR at get_address(): symbol not found in symTable" << endl;
		NewLA.closeReadFile();
		NewLA.closeOutputFile();
		exit(1);
	}
	return addr;
}

string get_type(string lexeme) {
	string type = "Not found";
	for (int i = 0; i < s; i++) {
		if (symTable[i][1] == lexeme) {
			type = symTable[i][2];
		}
	}
	if (type == "Not found") {
		NewLA.myOutputFile << "ERROR at get_type(): symbol not found in symTable" << endl;
		NewLA.closeReadFile();
		NewLA.closeOutputFile();
		exit(1);
	}
	return type;
}

void gen_instr(string instr, int address) {
	//cols 1 and 2 contains the instruction and address, respectively
	instrTable[d][1] = instr;
	//addresses of -999 are considered null
	if (address != -999) {
		//must convert int to string
		instrTable[d][2] = to_string(address);
	}
	d++;
}

//updates a prev jump instruction with a new jump address, jump_addr
void back_patch(int jump_addr) {
	if (!jumpStack.empty()) {
		//grab the address on top
		addr = jumpStack.top();
		//then remove it
		jumpStack.pop();
		//update a JMP's Oprd to the current d (Instruction Address)
		//Must convert int to string
		instrTable[addr][2] = to_string(jump_addr);
	}
	else {
		NewLA.myOutputFile << "ERROR at back_patch(): stack is empty" << endl;
		NewLA.closeReadFile();
		NewLA.closeOutputFile();
		exit(1);
	}
}

void printInstr() {
	NewLA.myOutputFile << left << setw(15) << "Address" << left << setw(15) << "OP" << "Oprnd " << endl;
	for (int i = 1; i < d; i++) {
		NewLA.myOutputFile << left << setw(15) << i << left << setw(15) << instrTable[i][1] << instrTable[i][2] << endl;
	}
}