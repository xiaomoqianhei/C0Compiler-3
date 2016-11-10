#ifndef _DEFS_H_
#define _DEFS_H_

#define CHARSY 1
#define CONSTSY 2
#define ELSESY 3
#define DOSY 4
#define FORSY 5
#define IFSY 6
#define INTSY 7
#define PRTSY 8
#define SCANSY 9
#define VOIDSY 10
#define WHILESY 11
#define IDSY 12
#define INTCON 13
#define CHARCON 14
#define STRCON 15
#define PLUSSY 16
#define MINUSSY 17
#define STARSY 18
#define DIVSY 19
#define LCURSY 20
#define RCURSY 21
#define LBRKSY 22
#define RBRKSY 23
#define LPARSY 24
#define RPARSY 25
#define SQSY 26
#define DQSY 27
#define COMMASY 28
#define SEMISY 29
#define ASSIGNSY 30
#define LTSY 31
#define LESY 32
#define GTSY 33
#define GESY 34
#define EQSY 35
#define NESY 36
#define INTTP 37
#define CHARTP 38
#define FUNCTP 39

#include <string>
#include <sstream>
using namespace std;

// for return value
int symbol;
string id;
int inum;
// for deal srcfile.
char ch;
string curLine;
int cl, cc, ll;
// for global words
int syms[256]; // for character 
const int symbolNumber = 40; // number of the symbolWords
string symbolWords[symbolNumber];
const int rwordNumber = 12; // number of the rwords.
string rwords[rwordNumber];

void setup()
{
    // for operator symbols
    ch = ' ';
    syms['+'] = PLUSSY;
    syms['-'] = MINUSSY;
    syms['*'] = STARSY;
    syms['/'] = DIVSY;
    syms['{'] = LCURSY;
    syms['}'] = RCURSY;
    syms['['] = LBRKSY;
    syms[']'] = RBRKSY;
    syms['('] = LPARSY;
    syms[')'] = RPARSY;
    syms[','] = COMMASY;
    syms[';'] = SEMISY;
    // for symbol type output
    symbolWords[CHARSY] = "CHARSY";
    symbolWords[CONSTSY] = "CONSTSY";
    symbolWords[ELSESY] = "ELSESY";
    symbolWords[DOSY] = "DOSY";
    symbolWords[FORSY] = "FORSY";
    symbolWords[IFSY] = "IFSY";
    symbolWords[INTSY] = "INTSY";
    symbolWords[PRTSY] = "PRTSY";
    symbolWords[SCANSY] = "SCANSY";
    symbolWords[VOIDSY] = "VOIDSY";
    symbolWords[WHILESY] = "WHILESY";
    symbolWords[IDSY] = "IDSY";
    symbolWords[INTCON] = "INTCON";
    symbolWords[CHARCON] = "CHARCON";
    symbolWords[STRCON] = "STRCON";
    symbolWords[PLUSSY] = "PLUSSY";
    symbolWords[MINUSSY] = "MINUSSY";
    symbolWords[STARSY] = "STARSY";
    symbolWords[DIVSY] = "DIVSY";
    symbolWords[LCURSY] = "LCURSY";
    symbolWords[RCURSY] = "RCURSY";
    symbolWords[LBRKSY] = "LBRKSY";
    symbolWords[RBRKSY] = "RBRKSY";
    symbolWords[LPARSY] = "LPARSY";
    symbolWords[RPARSY] = "RPARSY";
    symbolWords[COMMASY] = "COMMASY";
    symbolWords[SEMISY] = "SEMISY";
    symbolWords[ASSIGNSY] = "ASSIGNSY";
    symbolWords[LTSY] = "LTSY";
    symbolWords[LESY] = "LESY";
    symbolWords[GTSY] = "GTSY";
    symbolWords[GESY] = "GESY";
    symbolWords[EQSY] = "EQSY";
    symbolWords[NESY] = "NESY";
    symbolWords[INTTP] = "INTTP";
    symbolWords[CHARTP] = "CHARTP";
    symbolWords[FUNCTP] = "FUNCTP";
    // for reserved words
    rwords[1] = "char";
    rwords[2] = "const";
    rwords[3] = "else";
    rwords[4] = "do";
    rwords[5] = "for";
    rwords[6] = "if";
    rwords[7] = "int";
    rwords[8] = "printf";
    rwords[9] = "scanf";
    rwords[10] = "void";
    rwords[11] = "while";
}

#endif