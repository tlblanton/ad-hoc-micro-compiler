//
//  main.cpp
//  homework_3_micro_compiler
//
//  Created by Tyler Blanton on 8/31/15.
//  Copyright (c) 2015 tlblanton. All rights reserved.
//

#include <iostream>
#include "functions.h"
using namespace std;

int main()
{
    systemGoal();
    return 0;
}

/*
 --------------------------------------------------------------------------------
program 1(testProg.txt):
 
    begin A:=BB+314+A; end EofSym
 
output:
 
 
 **|function call|** 		 **|Code generated|**
 
 call systemGoal
 call program
 call start
 call match(BeginSym)
 call statementList
 call statement
 call ident
 call match(Id)
 call processId
                                 Declare A, Integer
 call match(AssignOp)
 call expression
 call primary
 call ident
 call match(Id)
 call processId
                                 Declare BB, Integer
 call addOp
 call match(PlusOp)
 call expression
 call primary
 call match(IntLiteral)
 call addOp
 call match(PlusOp)
 call expression
 call primary
 call ident
 call match(Id)
 call processId
                                 Declare Temp&1, Integer
                                 ADD  314, A, Temp&1
                                 Declare Temp&2, Integer
                                 ADD  BB, Temp&1, Temp&2
 call assign
                                 Store Temp&2, A
 call match(SemiColon)
 call match(EndSym)
 call match(EofSym)
 call finish
                                 Halt

----------------------------------------------------------------------------

 program 2 (myProg.txt):
 
 begin
 x :=(r+35+(x+2));
 write(x);
 y:= 5; --this is a lovely comment that will be ignored
 end EofSym


output:
 
 

 
 **|function call|** 		 **|Code generated|**
 
 call systemGoal
 call program
 call start
 call match(BeginSym)
 call statementList
 call statement
 call ident
 call match(Id)
 call processId
                                Declare x, Integer
 call match(AssignOp)
 call expression
 call primary
 call match(LParen)
 call expression
 call primary
 call ident
 call match(Id)
 call processId
                                Declare r, Integer
 call addOp
 call match(PlusOp)
 call expression
 call primary
 call match(IntLiteral)
 call addOp
 call match(PlusOp)
 call expression
 call primary
 call match(LParen)
 call expression
 call primary
 call ident
 call match(Id)
 call processId
 call addOp
 call match(PlusOp)
 call expression
 call primary
 call match(IntLiteral)
                                Declare Temp&1, Integer
                                ADD  x, 2, Temp&1
 call match(RParen)
                                Declare Temp&2, Integer
                                ADD  35, Temp&1, Temp&2
                                Declare Temp&3, Integer
                                ADD  r, Temp&2, Temp&3
 call match(RParen)
 call assign
                                Store Temp&3, x
 call match(SemiColon)
 call statementList
 call statement
 call match(WriteSym)
 call match(LParen)
 call exprList
 call expression
 call primary
 call ident
 call match(Id)
 call processId
                                Write x, Integer
 call match(RParen)
 call match(SemiColon)
 call statementList
 call statement
 call ident
 call match(Id)
 call processId
                                Declare y, Integer
 call match(AssignOp)
 call expression
 call primary
 call match(IntLiteral)
 call assign
                                Store 5, y
 call match(SemiColon)
 call match(EndSym)
 call match(EofSym)
 call finish
                                Halt



------------------------------------------------------------------------------------------------





*/