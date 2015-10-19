//
//  functions.cpp
//  assignment_2_micro_parser
//
//  Created by Tyler Blanton on 8/24/15.
//  Copyright (c) 2015 tlblanton. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <ctype.h>
#include <sstream>
#include "functions.h"

using namespace std;


std::vector<string> symbolTable;
int maxTemp;
int maxSymbol; //I am using symbolTable.size() to keep track of how many symbols are in the table currently and maxsymbol to say how many
//symbols are allowed
int lastSymbol = 0;
string tokenBuffer;

/**********************************
 Data Structures
 **********************************/
OpRec::OpRec(Operator newOp)
{
    op = newOp;
}

OpRec::OpRec()
{}

ExprRec::ExprRec()
{
    kind = TempExpr;
    isName = true;
    name = "nochange";
}

ExprRec::ExprRec(ExprKind EKind)
{
    kind = EKind;
    if(kind == IdExpr || kind == TempExpr)
    {
        isName = true;
        name = string();
    }
    else if(kind == LiteralExpr)
    {
        isName = false;
        val = int();
    }
}




/*********************
 SCANNER
 ********************/
string scanner(int optional)//returns token every time it runs. when non-zero argument is passed, it returns the next token but does
//not advance the stream
{
    
    static ifstream inFile; //static so it retains its open state throughout function calls until EofSym is returned.
    
    long long pos = 0; //long long because tellg() returns long long
    bool goBack = false;
    if(optional != 0)
    {
        goBack = true;
        pos = inFile.tellg();
    }
    
    
    if(!inFile.is_open())
    {
        inFile.open("testProg.txt"); //opens file if it is not open already. file must be reachable by program (in same directory ideally)
    }
    
    clearBuffer(tokenBuffer);
    char currentChar;
    if(inspect(inFile) == EOF)
    {
        if(goBack)
        {
            inFile.seekg(pos);
        }
        return "EofSym";
    }
    else
    {
        while(inspect(inFile) != EOF)
        {
            currentChar = read(inFile); //read current char
            if (currentChar == ' ' || currentChar == '\t' || currentChar == '\n')
            {
                advance(inFile);
            }
            else if((currentChar >= 'A' && currentChar <= 'Z') || (currentChar  >= 'a' && currentChar <= 'z'))
            {
                bufferChar(currentChar, tokenBuffer); //put character into tokenBuffer
                while((inspect(inFile) >= 'A' && inspect(inFile) <= 'Z') || (inspect(inFile) >='a' && inspect(inFile) <= 'z') || inspect(inFile) == '_' || (inspect(inFile) >= '0' && inspect(inFile) <= '9'))//checking the next char
                {
                    currentChar = read(inFile);
                    bufferChar(currentChar, tokenBuffer);
                }
                if (!(inspect(inFile) >= 'A' && inspect(inFile) <= 'Z') && !(inspect(inFile) >= 'a' && inspect(inFile) <= 'z') && !(inspect(inFile) >= '0' && inspect(inFile) <= '9'))
                {
                    if(goBack)
                    {
                        inFile.seekg(pos);
                    }
                    return checkReserved(tokenBuffer); //function that takes identifiers and converts them to tokens
                }
            }
            else if(currentChar >= '0' && currentChar <= '9')
            {
                bufferChar(currentChar, tokenBuffer);
                while(inspect(inFile) >= '0' && inspect(inFile) <= '9')
                {
                    currentChar = read(inFile);
                    bufferChar(currentChar, tokenBuffer);
                }
                if(goBack)
                {
                    inFile.seekg(pos);
                }
                return "IntLiteral";
                
            }
            else if(currentChar == '(')
            {
                if(goBack)
                {
                    inFile.seekg(pos);
                }
                return "LParen";
            }
            else if(currentChar == ')')
            {
                if(goBack)
                {
                    inFile.seekg(pos);
                }
                return "RParen";
            }
            else if(currentChar == ';')
            {
                if(goBack)
                {
                    inFile.seekg(pos);
                }
                return "SemiColon";
            }
            else if(currentChar == ',')
            {
                if(goBack)
                {
                    inFile.seekg(pos);
                }
                return "Comma";
            }
            else if(currentChar == '+')
            {
                if(goBack)
                {
                    inFile.seekg(pos);
                }
                return "PlusOp";
            }
            else if(currentChar == ':')
            {
                if(inspect(inFile) == '=')
                {
                    currentChar = read(inFile);
                    if(goBack)
                    {
                        inFile.seekg(pos);
                    }
                    return "AssignOp";
                }
                else
                {
                    if(goBack)
                    {
                        inFile.seekg(pos);
                    }
                    return "LexicalError(Inspect)"; //returns LexicalError Token if ':' is not followed by '='
                }
            }
            else if(currentChar == '-')
            {
                if(inspect(inFile) == '-')//comments --
                {
                    currentChar = read(inFile);
                    while(inspect(inFile) != '\n')
                    {
                        read(inFile);
                    }
                }
                else
                {
                    if(goBack)
                    {
                        inFile.seekg(pos);
                    }
                    return "MinusOp";
                }
            }
            else
            {
                if(goBack)
                {
                    inFile.seekg(pos);
                }
                return "LexicalError(CurrentChar)"; //returning LexicalError here, as the parser will handle throwing the actual error
            }
        }
        
        if(goBack)
        {
            inFile.seekg(pos);
        }
        return "EofSym";
    }
    
}

void clearBuffer(string& tokenBuffer)
{
    tokenBuffer = "";
}

char inspect(ifstream& inFile)
{
    return inFile.peek();
}

void bufferChar(char currentChar, string& singleTokenBuffer)
{
    singleTokenBuffer += currentChar;
}


string checkReserved(string id)
{
    
    if(id == "BEGIN" || id == "begin")
    {
        return "BeginSym";
    }
    else if(id == "END" || id == "end")
    {
        return "EndSym";
    }
    else if(id == "READ" || id == "read")
    {
        return "ReadSym";
    }
    else if(id == "WRITE" || id == "write")
    {
        return "WriteSym";
    }
    else
    {
        return "Id";
    }
    
}

void advance(ifstream& inFile)
{
    inFile.ignore(0);
}

char read(ifstream& inFile)
{
    return inFile.get();
}



/*************************************************************************************************************************
 PARSER
 ************************************************************************************************************************/

void match(string tokenLookingFor)
{
    cout << "call match(" << tokenLookingFor << ")" << endl;
    string currentToken = scanner(); //get next token
    bool legal = false;
    for(int i = 0; i < numOfLegalTokens; ++i)
    {
        if(currentToken == legalTokens[i])
        {
            legal = true;
        }
    }
    if(legal == false)
    {
        cout << "syntaxError(" << currentToken << ")" << endl;
    }
}

void systemGoal()
{
    cout << endl << "**|function call|** \t\t **|Code generated|**" << endl << endl;
    cout << "call systemGoal" << endl;
    program();
    match("EofSym");
    finish();
}

void program()
{
    cout << "call program" << endl;
    start();
    match("BeginSym");
    statementList();
    match("EndSym");
}

void statementList()
{
    cout << "call statementList" << endl;
    statement();
    string nextToken = getNextToken();
    if(nextToken == "Id" || nextToken == "ReadSym" || nextToken == "WriteSym")
    {
        statementList();
    }
    else
    {
        return;
    }
}

void statement()
{
    cout << "call statement" << endl;
    ExprRec identifier;
    ExprRec expr;
    
    string nextToken = getNextToken();
    if(nextToken == "Id")
    {
        ident(identifier);
        match("AssignOp");
        expression(expr);
        assign(identifier, expr);
        match("SemiColon");
    }
    else if(nextToken == "ReadSym")
    {
        match("ReadSym");
        match("LParen");
        idList();
        match("RParen");
        match("Semicolon");
    }
    else if(nextToken == "WriteSym")
    {
        match("WriteSym");
        match("LParen");
        exprList();
        match("RParen");
        match("SemiColon");
    }
    else
    {
        cout << "SyntaxError(" << nextToken << ")" << endl;
    }
}

void idList()
{
    cout << "call idList" << endl;
    ExprRec identifier;
    ident(identifier);
    readId(identifier);
    string nextToken = getNextToken();
    if (nextToken == "Comma")
    {
        match("Comma");
        idList();
    }
    else
    {
        return;
    }
}

void exprList()
{
    cout << "call exprList" << endl;
    ExprRec expr;
    expression(expr);
    writeExpr(expr);
    string nextToken = getNextToken();
    if(nextToken == "Comma")
    {
        match("Comma");
        exprList();
    }
    else
    {
        return;
    }
}

void expression(ExprRec& result)
{
    cout << "call expression" << endl;
    ExprRec leftOperand;
    ExprRec rightOperand;
    OpRec op;
    primary(leftOperand);
    string nextToken = getNextToken();
    if(nextToken == "PlusOp" || nextToken == "MinusOp")
    {
        addOp(op);
        expression(rightOperand);
        result = genInfix(leftOperand, op, rightOperand);
    }
    else
    {
        result = leftOperand;
    }
}

void primary(ExprRec& result)
{
    cout << "call primary" << endl;
    string nextToken = getNextToken();
    if(nextToken == "LParen")
    {
        match("LParen");
        expression(result);
        match("RParen");
    }
    else if(nextToken == "Id")
    {
        ident(result);
    }
    else if(nextToken == "IntLiteral")
    {
        match("IntLiteral");
        processLiteral(result);
    }
    else
    {
        cout << "SyntaxError(" << nextToken << ")" << endl;
    }
}

void ident(ExprRec& result)
{
    cout << "call ident" << endl;
    match("Id");
    processId(result); //processId changed to pass-by-reference so that the value of result could be changed
}

void addOp(OpRec op)
{
    cout << "call addOp" << endl;
    string nextToken = getNextToken();
    if(nextToken == "PlusOp")
    {
        match("PlusOp");
        processOp(op);
    }
    else if(nextToken == "MinusOp")
    {
        match("MinusOp");
        processOp(op);
    }
    else
    {
        cout << "SyntaxError(" << nextToken << ")" << endl;
    }
}


string getNextToken()
{
    return scanner(1); //scanner called with any non-zero argument returns next token but does not advance the stream
}


/*************************************************************************************************************************
 AUXILARY ROUTINES
 ************************************************************************************************************************/
void generate(string s1, string s2, string s3, string s4)
{
    
    if(s2 == "" && s3 == "" && s4 == "")
    {
        cout << "\t\t\t\t" << s1 << endl;
        write(s1);
    }
    else if(s3 == "" && s4 == "")
    {
        cout << "\t\t\t\t" << s1 << " " << s2 <<endl;
        write(s1, s2);
    }
    else if(s4 == "")
    {
        cout << "\t\t\t\t" << s1 << " " << s2  << ", " << s3 << endl;
        write(s1, s2, s3);
    }
    else
    {
        cout << "\t\t\t\t" << s1 << " " << s2 << ", " << s3 << ", " << s4 << endl;
        write(s1, s2, s3, s4);
    }
}

string extract(ExprRec E)
{
    if(E.getKind() == IdExpr || E.getKind() ==  TempExpr)
    {
        return E.getName();
    }
    else
    {
        stringstream ss;
        ss << E.getVal();
        return ss.str();
    }
}


string extractOp(OpRec o)
{
    if(o.getOp() == PlusOp)
    {
        return "ADD ";
    }
    else if(o.getOp() == MinusOp)
    {
        return "SUB ";
    }
    return "ADD ";
}


bool lookUp(string s)
{
    bool up = false;
    for(int i = 0; i < symbolTable.size(); ++i)
    {
        if(symbolTable[i] == s)
        {
            up = true;
        }
    }
    return up;
}

void enter(string s)
{
    if(lastSymbol < maxSymbol)
    {
        lastSymbol++;
        symbolTable.push_back(s);
    }
    else
    {
        cout << "Exception: Symbol Table Overflow" << endl;
    }
}

void checkId(string s)
{
    if(!lookUp(s) && s != "end")
    {
        enter(s);
        generate("Declare", s, "Integer");
    }
}

string getTemp()
{
    maxTemp++;
    stringstream ss;
    ss << maxTemp;
    string tempName = ("Temp&" + ss.str());
    checkId(tempName);
    return tempName;
}




/*************************************************************************************************************************
 SEMANTIC ROUTINES
 ************************************************************************************************************************/


void start()
{
    cout << "call start" << endl;
    maxSymbol = 50;
    maxTemp = 0;
}


void assign(ExprRec target, ExprRec source) //IN
{
    cout << "call assign" << endl;
    generate("Store", extract(source), target.getName());
}

void readId(ExprRec inVar)
{
    generate("Read", inVar.getName(), "Integer");
}

void writeExpr(ExprRec outExpr)
{
    generate("Write", extract(outExpr), "Integer");
}

ExprRec genInfix(ExprRec e1, OpRec oper, ExprRec e2)
{
    ExprRec ERec(TempExpr);
    ERec.setName(getTemp());
    generate(extractOp(oper), extract(e1), extract(e2), ERec.getName());
    return ERec;
}


void processId(ExprRec& e)
{
    cout << "call processId" << endl;
    checkId(tokenBuffer);
    e.setKind(IdExpr);
    e.setName(tokenBuffer);
}

void processLiteral(ExprRec& e)
{
    e.setKind(LiteralExpr);
    e.setVal(stoi(tokenBuffer));
}

void processOp(OpRec& oper)
{
    if(tokenBuffer == "PlusOp")
    {
        oper.setOp(PlusOp);
    }
    else
    {
        oper.setOp(MinusOp);
    }
}

void finish()
{
    cout << "call finish" << endl;
    generate("Halt");
    cout << endl;
}


void write(string s1, string s2, string s3, string s4)
{
    static ofstream outFile("output.txt");
    if(s2 == "" && s3 == "" && s4 == "")
    {
        outFile << s1 << endl;
    }
    else if(s3 == "" && s4 == "")
    {
        outFile << s1 << " " << s2 << endl;
    }
    else if(s4 == "")
    {
        outFile << s1 <<  " " << s2 << ", " << s3 << endl;
    }
    else
    {
        outFile << s1  << " " << s2 << ", " << s3 << ", " << s4 << endl;
    }
}



