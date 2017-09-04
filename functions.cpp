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
    static ifstream inFile;
    
    long long pos = 0;
    bool goBack = false;
    if(optional != 0) //if optional value is passed, the next token is retrieved without advancing the stream by rewinding to its previous position
    {
        goBack = true;
        pos = inFile.tellg();
    }

    if(!inFile.is_open())
    {
        inFile.open("myProg.txt"); //opens file if it is not open already. file must be reachable by program (in working directory ideally)
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
        //If we are not at the end of the file, then the next symbol needs to be taken in and interpreted as something that the compiler will understand
        while(inspect(inFile) != EOF)
        {
            currentChar = read(inFile); //read current char
            if (currentChar == ' ' || currentChar == '\t' || currentChar == '\n')   //if we find space, we advance past it
            {
                advance(inFile);
            }
            //if we find a character then we keep taking in characters to form a whole name
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
            // If we have found a symbol, then we return the name of that symbol, such that the compiler will understand it.
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
            else if(currentChar == '-') //possibe beginning of comment
            {
                if(inspect(inFile) == '-')//comments are initiated by "--"
                {
                    currentChar = read(inFile);
                    while(inspect(inFile) != '\n')      //keep reading until end of line to process comment
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
    for(int i = 0; i < id.size(); ++i)
    {
        id[i] = toupper(id[i]);
    }

    if(id == "BEGIN")
    {
        return "BeginSym";
    }
    else if(id == "END")
    {
        return "EndSym";
    }
    else if(id == "READ")
    {
        return "ReadSym";
    }
    else if(id == "WRITE")
    {
        return "WriteSym";
    }
    else
    {
        return "Id";
    }
    
}

// Moves our file reader ahead on char
void advance(ifstream& inFile)
{
    inFile.ignore(0);
}

// Reads next char in file
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
    if(!legal)
    {
        cerr << "syntaxError(" << currentToken << ")" << endl;
        //maybe quit here
    }
}

/**********************
 * SYSTEMGOAL
 * this is the only function called by main. It initiaes the compilation process.
 *********************/
void systemGoal()
{
    cout << endl << "**|function call|** \t\t **|Code generated|**" << endl << endl;
    cout << "call systemGoal" << endl;
    program();
    match("EofSym");
    finish();
}


/**************
 * PROGRAM
 * calls Start() which initializes compiler, checks for presence of proper beginning symbol, and then proceeds toread in entire list of statements in 
 * user-created program.
 *************/
void program()
{
    cout << "call program" << endl;
    start();
    match("BeginSym");
    statementList();
    match("EndSym");
}

/**************
 * STATMENTLIST
 * Processes a single statement, and if a look-ahead sees another statement, then the function calls itseld recursively
 *************/
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

/**************
 * STATEMENT
 * Interprets a statement as a series of tokens.
 * In the case that there are nestings (meaning a "(" has been found), this function calls others to recognize the list of
 * IDs or expressions that are inside of it.
 *************/
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
        idList();       //if we have found a "(" after a READ symbol then what follows has to be a list of identifiers
        match("RParen");
        match("Semicolon");
    }
    else if(nextToken == "WriteSym")
    {
        match("WriteSym");
        match("LParen");
        exprList();         //if we have found a "(" after a WRITE symbol then what follows has to be a list of expressions
        match("RParen");
        match("SemiColon");
    }
    else
    {
        cout << "SyntaxError(" << nextToken << ")" << endl; //displaying where a syntax error was found in the file
    }
}

/***********
 * IDLIST
 * processes a list of comma-separated identifiers, calling to itself recursively as is necessary
 **********/
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


/***************
 * EXPRLIST
 * processes a list of expressions
 **************/

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


/****************
 * EXPRESSION
 * interprets a single expressions (addition, subtraction)
 ***************/
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

/****************
 * PRIMARY
 * interprets primary expressions
 ***************/
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

/****************
 * IDENT
 * looks ahead for appropriate next operator given a current operator
 ***************/
void ident(ExprRec& result)
{
    cout << "call ident" << endl;
    match("Id");
    processId(result); //processId changed to pass-by-reference so that the value of result could be changed
}


/****************
 * ADDOP
 * handles the case in which the progrma comes across a "+" or "-"
 ***************/
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

/****************
 * GETNEXTTOKEN
 * Retrieve written programs next token
 ***************/
string getNextToken()
{
    return scanner(1); //scanner called with any non-zero argument returns next token but does not advance the stream
}


/*************************************************************************************************************************
 AUXILARY ROUTINES
 ************************************************************************************************************************/
/****************
 * GENERATE
 * Writes out the compiled text for the user to see.
 ***************/
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

/****************
 * EXTRACT
 * returns type of expression that is being handled
 ***************/
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


/****************
 * EXTRACTOP
 * returns type of operator that is being handled
 ***************/
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

/****************
 * LOOKUP
 * checks if argument string is present in the symbol table
 ***************/
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

/****************
 * ENTER
 * puts new string onto symbol table
 ***************/
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

/****************
 * CHECKID
 * if we are not at the end of the file, checka string's Id and generate a statement that declares it
 ***************/
void checkId(string s)
{
    if(!lookUp(s) && s != "end")
    {
        enter(s);
        generate("Declare", s, "Integer");
    }
}

/****************
 * GETTEMP
 * Generates a temporary variable as a placeholder for when we have intermediary values as a result of computation (add, sub, mul, div)
 ***************/
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


/****************
 * START
 * initializes compiler
 ***************/
void start()
{
    cout << "call start" << endl;
    maxSymbol = 50;
    maxTemp = 0;
}

/****************
 * ASSIGN
 * Stores target's value in source and displays that
 ***************/
void assign(ExprRec target, ExprRec source) //IN
{
    cout << "call assign" << endl;
    generate("Store", extract(source), target.getName());
}

/****************
 * READID
 * prints what is being read
 ***************/
void readId(ExprRec inVar)
{
    generate("Read", inVar.getName(), "Integer");
}

/****************
 * WRITEID
 * prints what is being written
 ***************/
void writeExpr(ExprRec outExpr)
{
    generate("Write", extract(outExpr), "Integer");
}

/****************
 * EXPRREC
 * prints intermediary calculation for user to see what is being done
 ***************/
ExprRec genInfix(ExprRec e1, OpRec oper, ExprRec e2)
{
    ExprRec ERec(TempExpr);
    ERec.setName(getTemp());
    generate(extractOp(oper), extract(e1), extract(e2), ERec.getName());
    return ERec;
}

/****************
 * PROCESSID
 * checks a token's ID
 ***************/
void processId(ExprRec& e)
{
    cout << "call processId" << endl;
    checkId(tokenBuffer);
    e.setKind(IdExpr);
    e.setName(tokenBuffer);
}

/****************
 * PROCESSLITERAL
 * if there is a literal in the code, make sure it's proper value is associated with it.
 ***************/
void processLiteral(ExprRec& e)
{
    e.setKind(LiteralExpr);
    e.setVal(stoi(tokenBuffer));
}

/****************
 * PROCESSOP
 * Generate "+" or "_" based on "PlusOp" or "MinusOp"
 ***************/
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

/****************
 * FINISH
 * complete compilation process, print HALT message
 ***************/
void finish()
{
    cout << "call finish" << endl;
    generate("Halt");
    cout << endl;
}

/****************
 * WRITE
 * function that prints its arguments
 ***************/
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



