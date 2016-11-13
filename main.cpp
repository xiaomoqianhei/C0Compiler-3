#include "defs.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <set>
#include <sstream>
#include <string>

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
// code generate
int errors;
int level;
struct Table {
    string id; // name
    int objtp; // type of the entry
    int val; // value of the entry
    int adr; // address in a block
    int params; // the number of the array length or paramList length
} tab[TABLE_NUM];
int funcNumber; // the number of the functions
int functable[TABLE_NUM]; // the index of the functions
set<int> fsyms; // for skipTo
// for generate code
struct Code {
    string op;
    string id1;
    string id2;
    string res;
} codes[CODE_NUM];
int pc;

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
    // init the code generate
    errors = 0;

    fsyms.insert("int");
    fsyms.insert("char");
    fsyms.insert("void");
    fsyms.insert("const");
}

void error()
{
    cout << "Format Error(" << cl << ',' << cc << "): Unknown Error\n";
}

void generateError()
{
}

void generateCode()
{
}

void getNextLine()
{
    if (!getline(cin, curLine)) {
        cout << "Reach the End of the File!" << endl;
        exit(0);
    }
    curLine += '\n';
}

void getch()
{
    if (cc == ll) {
        getNextLine();
        cl++;
        cc = 0;
        ll = curLine.length();
    }
    ch = curLine[cc++];
}

void getSymbol()
{
    while (ch == ' ' || ch == '\n' || ch == '\t')
        getch();
    if (isalpha(ch) || ch == '_') { // alpha
        id = ch;
        for (getch(); isalpha(ch) || ch == '_' || isdigit(ch); getch())
            id += ch;
        int i;
        for (i = 1; i <= rwordNumber; ++i)
            if (id == rwords[i])
                break;
        symbol = i;
    } else if (isdigit(ch)) { // number
        symbol = INTCON;
        id = ch;
        for (getch(); isdigit(ch); getch())
            id += ch;
        if (id != "0" && id[0] == '0')
            error();
        stringstream ss;
        ss << id;
        ss >> inum;
    } else if (ch == '<') { // < || <=
        id = ch;
        getch();
        if (ch == '=') {
            id += ch;
            symbol = LESY;
            getch();
        } else
            symbol = LTSY;
    } else if (ch == '>') { // > || >=
        id = ch;
        getch();
        if (ch == '=') {
            id += ch;
            symbol = GESY;
            getch();
        } else
            symbol = GTSY;
    } else if (ch == '!') { // !=
        id = ch;
        symbol = NESY;
        getch();
        if (ch == '=') {
            id += ch;
            getch();
        } else
            error();
    } else if (ch == '=') { // ==
        id = ch;
        getch();
        if (ch == '=') {
            id += ch;
            symbol = ASSIGNSY;
            getch();
        } else
            symbol = EQSY;
    } else if (ch == '\'') { // char const
        symbol = CHARCON;
        getch();
        if (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '_' || isalnum(ch)) {
            id = ch;
            getch();
            if (ch != '\'')
                error();
            else
                getch();
        } else
            error();
    } else if (ch == '\"') { // string const
        symbol = STRCON;
        id = "";
        for (getch(); ch != '\"'; getch())
            if (32 <= ch && ch <= 126)
                id += ch;
            else {
                error();
                if (ch == '\n')
                    break;
            }
        if (ch == '\"')
            getch();
    } else if (strchr("+-*/()[]{},;", ch)) { // other operators
        id = ch;
        symbol = syms[(int)ch];
        getch();
    } else {
        error();
        getch();
    }
}
void skipTo(set<int> valid)
{
    set<int>::iterator iter;
    for (getSymbol(); iter.find(symbol) == iter.end(); getSymbol())
        ;
}
int generateLabel() {}
int enter() {}
int findSym() {}
int paramList() {}
int expression() {}
int term() {}
int factor() {}
int statement() {}
int ifStatement() {}
int loopStatement() {}
int call() {}
int paramReal() {}
int scanfStatement() {}
int printfStatement() {}
int returnStatement() {}
int defConst() {}
int defVar() {}
int defFunc() {}
int defMain() {}
int program()
{
    getSymbol();
    test(fsyms, fsyms, 0);
    while (symbol == CONSTSY) {
        getSymbol();
        if (defConst(level))
            skipTo(fsyms);
    }
    int isFunc = 0;
    while (symbol == INTSY || symbol == CHARSY) {
        int ts = symbol;
        getSymbol();
        int is = symbol;
        string iid = id;
        getSymbol();
        if (symbol == LPARSY) {
            isFunc = 1;
            enter(iid, ts, ) break;
        }
        if (symbol == SEMISY || symbol == COMMASY)
    }
}
int main()
{
    setup();
    program(0);
    if (errors == 0) {
        generateCode();
        cout << "\nnice!\n";
    } else {
        generateError();
        cout << "\nOoops\n";
    }
    return 0;
}
