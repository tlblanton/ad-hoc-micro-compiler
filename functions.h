//
//  functions.h
//  assignment_2_micro_parser
//
//  Created by Tyler Blanton on 8/24/15.
//  Copyright (c) 2015 tlblanton. All rights reserved.
//
#ifndef _FUNCTIONS_H
#define _FUNCTIONS_H
#pragma once
#include <iostream>
#include <string>
#include <vector>
using namespace std;

//--#globals#--//
const int numOfLegalTokens = 14;
const string legalTokens[numOfLegalTokens] = {"BeginSym" ,"EndSym", "ReadSym", "WriteSym", "Id", "IntLiteral", "LParen", "RParen", "SemiColon", "Comma", "AssignOp", "PlusOp", "MinusOp", "EofSym"};

//****************************************|data structures|****************************************

enum Operator
{
    PlusOp, MinusOp
};

class OpRec
{
public:
    OpRec();
    OpRec(Operator);
    Operator getOp(){return op;}
    void setOp(Operator newOp){op = newOp;}
private:
    Operator op;
};

enum ExprKind
{
    IdExpr, LiteralExpr, TempExpr
};



class ExprRec
{
public:
    ExprRec();
    ExprRec(ExprKind);
    ExprKind getKind(){return kind;}
    string getName(){return name;}
    int getVal(){return val;}
    void setName(string newName){kind = IdExpr;name = newName;}
    void setKind(ExprKind newKind){kind = newKind;}
    void setVal(int newVal){val = newVal;}
private:
    ExprKind kind;
    bool isName; //this bool is to keep track of whether the "name" or "val" variables should be active. If "true": name is active
    string name;
    int val;
};


//****************************************|functions for scanner|****************************************
string scanner(int optional = 0); //returns Token. called in loop. when any non-zero number is passed to it, the next token is
//returned but the stream is not advanced. works like a peek
void bufferChar(char currentChar, string& singleTokenBuffer); //adds character to tokenBuffer
void clearBuffer(string& tokenBuffer); //clears tokenBuffer
char inspect(ifstream& inFile); //peeks at next char
string checkReserved(string input); //takes identifiers, returns strings that are tokens.
void advance(ifstream& inFile);//moves forward one character
char read(ifstream& inFile);//reads next character


//****************************************|functions for parser|****************************************
void match(string tokenLookingFor);
void systemGoal();
void program();
void statementList();
void statement();
string getNextToken();
void ident(ExprRec& result);
void expression(ExprRec& result);
void exprList();
void idList();
void primary(ExprRec& result);
void addOp(OpRec op);


//****************************************|Auxilary Routines|****************************************
void generate(string opCode, string op1 = "", string op2 = "", string resultField = "");
string extract(ExprRec E);
string extractOp(OpRec o);



//****************************************|Symbol Table Routines|****************************************
bool lookUp(string s);
void enter(string s);
void checkId(string s);
string getTemp();


//****************************************|Semantic Routines|****************************************
void start(); //DONE
void assign(ExprRec target, ExprRec source); 
void readId(ExprRec inVar);
void writeExpr(ExprRec outExpr);
ExprRec genInfix(ExprRec e1, OpRec op, ExprRec e2);
void processId(ExprRec& e);
void processLiteral(ExprRec& e);
void processOp(OpRec& o);
void finish();


void write(string s1, string s2 = "", string s3 = "", string s4 = "");


#endif
