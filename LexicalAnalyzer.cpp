/*
LexicalAnalyzer.cpp

CPSC 323, Spring 2016, Prof. Song Choi, Assignment 3

NAME:			David Tu, Section 2 (david.tu2@csu.fullerton.edu)
DESCRIPTION:	Rat16 Compiler - Lexical Analyzer implementation (Assignment 1)
REVISIONS:		Version 3
MODIFIED BY:	2016/05/09
CHANGE LOG:		Line 206: Starting from this line, added printSwitch functionality
*/

#include "LexicalAnalyzer.h"

//constructor
LexicalAnalyzer::LexicalAnalyzer()
{
	lexState = 1;

	myOutputFile.open("Result.txt");
	myReadFile.open("TestCase.txt");

	//IntRealDFSM Construction
	//Row 0 will be disregarded
	vector<int> intRealState0;

	vector<int> intRealState1;
	intRealState1.push_back(2);
	intRealState1.push_back(3);

	//accepting state:
	vector<int> intRealState2;
	intRealState2.push_back(2);
	intRealState2.push_back(3);

	vector<int> intRealState3;
	intRealState3.push_back(4);
	intRealState3.push_back(5);

	//accepting state:
	vector<int> intRealState4;
	intRealState4.push_back(4);
	intRealState4.push_back(5);

	vector<int> intRealState5;
	intRealState5.push_back(5);
	intRealState5.push_back(5);

	intRealDFSM.push_back(intRealState0);
	intRealDFSM.push_back(intRealState1);
	intRealDFSM.push_back(intRealState2);
	intRealDFSM.push_back(intRealState3);
	intRealDFSM.push_back(intRealState4);
	intRealDFSM.push_back(intRealState5);

	//IDDFSM Construction
	//row 0 will be disregarded
	vector<int> IDState0;

	vector<int> IDState1;
	IDState1.push_back(2);
	IDState1.push_back(5);
	IDState1.push_back(5);

	//accepting state:
	vector<int> IDState2;
	IDState2.push_back(3);
	IDState2.push_back(4);
	IDState2.push_back(5);

	//accepting state:
	vector<int> IDState3;
	IDState3.push_back(3);
	IDState3.push_back(4);
	IDState3.push_back(5);

	vector<int> IDState4;
	IDState4.push_back(3);
	IDState4.push_back(4);
	IDState4.push_back(5);

	vector<int> IDState5;
	IDState5.push_back(5);
	IDState5.push_back(5);
	IDState5.push_back(5);

	IDDFSM.push_back(IDState0);
	IDDFSM.push_back(IDState1);
	IDDFSM.push_back(IDState2);
	IDDFSM.push_back(IDState3);
	IDDFSM.push_back(IDState4);
	IDDFSM.push_back(IDState5);

	//keywords vector construction
	keywords.push_back("function");
	keywords.push_back("integer");
	keywords.push_back("boolean");
	keywords.push_back("real");
	keywords.push_back("if");
	keywords.push_back("endif");
	keywords.push_back("else");
	keywords.push_back("return");
	keywords.push_back("printf");
	keywords.push_back("scanf");
	keywords.push_back("while");
	keywords.push_back("true");
	keywords.push_back("false");

	//separators vector construction
	separators.push_back("$$");
	separators.push_back("(");
	separators.push_back(")");
	separators.push_back(",");
	separators.push_back(":");
	separators.push_back("{");
	separators.push_back("}");
	separators.push_back(";");

	//operators vector construction
	operators.push_back(":=");
	operators.push_back("=");
	operators.push_back("/=");
	operators.push_back(">");
	operators.push_back("<");
	operators.push_back("=>");
	operators.push_back("<=");
	operators.push_back("+");
	operators.push_back("-");
	operators.push_back("*");
	operators.push_back("/");
}

//destructor
LexicalAnalyzer::~LexicalAnalyzer()
{
}

//getters
bool LexicalAnalyzer::readFileOpen() {
	return myReadFile.is_open();
}

string LexicalAnalyzer::getToken() {
	return token;
}

string LexicalAnalyzer::getLexeme() {
	return lexeme;
}

//setters
void LexicalAnalyzer::closeReadFile() {
	myReadFile.close();
}

void LexicalAnalyzer::closeOutputFile() {
	myOutputFile.close();
}

//other functions
//output a token and corresponding lexeme into the output file
void LexicalAnalyzer::lexer() {
	//if condition ensures that I'm still in the file
	if (myReadFile.get(ch)) {

		//initialize the lexeme/state for every iteration
		lexeme = "";
		lexState = 1;

		//setup to get the first character:
		while (isspace(ch) && myReadFile) {
			myReadFile.get(ch);
		}

		//check if the 1st character in the stream is an alpha
		if (isalpha(ch)) {
			runIDDFSM();
		}
		//check if the first character is a digit or a decimal
		else if (isdigit(ch) | ch == '.') {
			runIntRealDFSM();
		}
		//lookups for $$, :=, /=, <=, => and [*
		else if (ch == '$') {
			lexeme = lexeme + ch;
			myReadFile.get(ch);

			//Case: Separator, $$
			if (ch == '$') {
				lexeme = lexeme + ch;
				token = "Separator";
			}
			//Case: Illegal lexeme, $
			else {
				token = "Unknown";
				//move pointer back to $
				myReadFile.unget();
			}
			if (printSwitch) {
				myOutputFile << "Lexeme: " << setw(20) << left << lexeme << "\tToken: " << token << endl;
			}
		}
		else if (ch == ':' | ch == '/' | ch == '<') {
			lexeme = lexeme + ch;
			myReadFile.get(ch);

			//Case: Operators, :=, /= or <=
			if (ch == '=') {
				lexeme = lexeme + ch;
				token = "Operator";
			}
			//Case: Operators, / or <
			else {
				if (lexeme == "/" | lexeme == "<") {
					token = "Operator";
				}
				else {
					//Case: Separator, :
					token = "Separator";
				}
				//move pointer back to '/', '<' or ':'
				myReadFile.unget();
			}
			if (printSwitch) {
				myOutputFile << "Lexeme: " << setw(20) << left << lexeme << "\tToken: " << token << endl;
			}
		}
		else if (ch == '=') {
			lexeme = lexeme + ch;
			myReadFile.get(ch);

			//Case: Operator, =>
			if (ch == '>') {
				lexeme = lexeme + ch;
				token = "Operator";
			}
			else {
				//Case: Operator, =
				token = "Operator";
				//move pointer back to '='
				myReadFile.unget();
			}
			if (printSwitch) {
				myOutputFile << "Lexeme: " << setw(20) << left << lexeme << "\tToken: " << token << endl;
			}
		}
		else if (ch == '[') {
			lexeme = lexeme + ch;
			myReadFile.get(ch);

			//Case: start of comment block, [*
			if (ch == '*') {
				//the lexeme is passed and it is '[', but it will be overwritten by the algoritm
				commentSkip(lexeme);
				lexer();
			}
			else {
				//Case: Illegal lexeme, [
				token = "Unknown";
				if (printSwitch) {
					myOutputFile << "Lexeme: " << setw(20) << left << lexeme << "\tToken: " << token << endl;
				}
				//move pointer back to '['
				myReadFile.unget();
			}
		}
		//look for the rest of the separators/operators
		else {
			//ensures that no whitespace char are marked as "Unknown", located at End of File
			if (!isspace(ch)) {
				lexeme = lexeme + ch;
				if (vectorChecker(separators)) {
					token = "Separator";
				}
				else if (vectorChecker(operators)) {
					token = "Operator";
				}
				else {
					token = "Unknown";
				}
				if (printSwitch) {
					myOutputFile << "Lexeme: " << setw(20) << left << lexeme << "\tToken: " << token << endl;
				}
			}
		}
	}
}

//accumulates more digit and decimal characters into a variable called lexeme and correspondingly updates the lexState variable
void LexicalAnalyzer::runIntRealDFSM() {
	if (isdigit(ch)) {
		//eqivalent to column 1 in the documentation
		column = 0;
		//initial value should be 2
		lexState = intRealDFSM[lexState][column];
		lexeme = lexeme + ch;
		myReadFile.get(ch);
	}
	else if (ch = '.') {
		//eqivalent to column 2 in the documentation
		column = 1;
		//initial value should be 3
		lexState = intRealDFSM[lexState][column];
		lexeme = lexeme + ch;
		myReadFile.get(ch);
	}

	//keep iterating until pointer finds a illegal char. The myReadFile condition prevents infinite loop when the pointer passes End of File
	while ((isdigit(ch) | ch == '.') && myReadFile) {
		if (isdigit(ch)) {
			column = 0;
			//lexState = 2 if it's an integer, 4 if it's a real
			lexState = intRealDFSM[lexState][column];
			lexeme = lexeme + ch;
			myReadFile.get(ch);
		}
		else if (ch == '.') {
			column = 1;
			//lexState = 3 if the prior state was 2, otherwise 5
			lexState = intRealDFSM[lexState][column];
			lexeme = lexeme + ch;
			myReadFile.get(ch);
		}
	}

	//Case: Integer
	if (lexState == 2) {
		token = "Integer";
	}
	//Case: Real
	else if (lexState == 4) {
		token = "Real";
	}
	//Case: decimal is the last digit
	else {
		token = "Unknown";
	}
	//char pointer pointing 1 char after the integer. We must move the pointer back one char in order to not miss any tokens
	myReadFile.unget();
	if (printSwitch) {
		myOutputFile << "Lexeme: " << setw(20) << left << lexeme << "\tToken: " << token << endl;
	}
}

//accumulates more alpha and _ characters into a variable called lexeme and correspondingly updates the lexState variable 
void LexicalAnalyzer::runIDDFSM() {
	//eqivalent to column 1 in the documentation
	column = 0;
	//initial value is 2
	lexState = IDDFSM[lexState][column];
	lexeme = lexeme + ch;
	myReadFile.get(ch);

	//keep iterating until pointer finds a illegal char. The myReadFile condition prevents infinite loop when the pointer passes End of File
	while ((isalpha(ch) | ch == '_' | isdigit(ch)) && myReadFile) {
		if (isalpha(ch)) {
			column = 0;
			//lexState = 3
			lexState = IDDFSM[lexState][column];
			lexeme = lexeme + ch;
			myReadFile.get(ch);
		}
		else if (ch == '_') {
			column = 1;
			//lexState = 4
			lexState = IDDFSM[lexState][column];
			lexeme = lexeme + ch;
			myReadFile.get(ch);
		}
		else if (isdigit(ch)) {
			column = 2;
			//lexState = 5
			lexState = IDDFSM[lexState][column];
			lexeme = lexeme + ch;
			myReadFile.get(ch);
		}
	}

	if (lexState == 2 | lexState == 3) {
		convertLower();
		if (vectorChecker(keywords)) {
			token = "Keyword";
		}
		else {
			token = "Identifier";
		}
	}
	else {
		//if lexeme ends with an underscore, it is lexState = 4 scenario, but if lexeme contains digit(s), it is lexState = 5 scenario
		token = "Unknown";
	}
	//char pointer pointing 1 char after the illegal lexeme. We must move the pointer back one char in order to not miss any tokens
	myReadFile.unget();
	if (printSwitch) {
		myOutputFile << "Lexeme: " << setw(20) << left << lexeme << "\tToken: " << token << endl;
	}
}

//takes a lexeme and checks against the "vectorToCheck" for any keywords, separators or operators - depending on what I pass to "vectorToCheck"
//If there's a match, the function returns true
bool LexicalAnalyzer::vectorChecker(vector<string>& vectorToCheck) {
	bool found = false;
	for (int i = 0; i < vectorToCheck.size(); i++) {
		if (lexeme == vectorToCheck.at(i)) {
			found = true;
		}
	}
	return found;
}

//recursive algoritm which skips the comment block. The algoritm terminates once the string, twoCharString matches with "*]"
void LexicalAnalyzer::commentSkip(const string twoCharString) {
	if (twoCharString == "*]") {
		myReadFile.get(ch);
	}
	else {
		//ensure that I'm still in the file. This condition is for the event of an unending comment block that have reached End of File
		if (myReadFile.get(ch)) {
			string twoChar;
			twoChar = ch;
			myReadFile.get(ch);
			twoChar = twoChar + ch;
			myReadFile.unget();
			commentSkip(twoChar);
		}
	}
}

//converts lexemes to lowercase
void LexicalAnalyzer::convertLower() {
	for (int i = 0; i < lexeme.length(); i++) {
		lexeme[i] = tolower(lexeme[i]);
	}
}