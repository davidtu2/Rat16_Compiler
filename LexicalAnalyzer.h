/*
LexicalAnalyzer.h

CPSC 323, Spring 2016, Prof. Song Choi, Assignment 3

NAME:			David Tu, Section 2 (david.tu2@csu.fullerton.edu)
DESCRIPTION:	Rat16 Compiler - Lexical Analyzer implementation (Assignment 1)
REVISIONS:		Version 3
MODIFIED BY:	2016/05/09
CHANGE LOG:		Line 56: Added a printSwitch
*/

#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <iomanip>
using namespace std;

class LexicalAnalyzer
{
public:
	//constructor
	LexicalAnalyzer();

	//destructor
	~LexicalAnalyzer();

	//getters
	bool readFileOpen();
	string getToken();
	string getLexeme();

	//setters
	void closeReadFile();
	void closeOutputFile();

	//other functions
	//output a token and corresponding lexeme into the output file
	void lexer();
	//accumulates more alpha and _ characters into a variable called lexeme and correspondingly updates the lexState variable
	void runIDDFSM();
	//accumulates more digit and decimal characters into a variable called lexeme and correspondingly updates the lexState variable
	void runIntRealDFSM();
	//takes a lexeme and checks against the "vectorToCheck" for any keywords, separators or operators - depending on what I pass to "vectorToCheck"
	//If there's a match, the function returns true
	bool vectorChecker(vector<string>& vectorToCheck);
	//recursive algoritm which skips the comment block. The algoritm terminates once the string, twoCharString matches with "*]"
	void commentSkip(const string twoCharString);
	//converts lexemes to lowercase
	void convertLower();
	//output file - output of Test Case are placed here
	ofstream myOutputFile;
	bool printSwitch = false;

private:
	//holds the lexeme after lexer() is completed
	string lexeme;
	//container for get/unget functions
	char ch;
	//container for column assignment based on the value of ch
	int column;
	//holds the token after lexer() is completed
	string token;
	//container for lexical state assignment based on the value of ch and column
	int lexState;
	//Input file - Test Cases are read from here
	ifstream myReadFile;
	//intRealDFSM (Integer/Real DFSM)
	vector< vector<int> > intRealDFSM;
	//IDDFSM (Identifier DFSM)
	vector< vector<int> > IDDFSM;
	//Keywords:
	vector <string> keywords;
	//Separators:
	vector <string> separators;
	//Operators:
	vector <string> operators;
};