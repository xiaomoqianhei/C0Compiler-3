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
int Symbol;
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
const int rwordNumber = 11; // number of the rwords.
string rwords[rwordNumber + 1];
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

    fsyms.insert(INTSY);
    fsyms.insert(CHARSY);
    fsyms.insert(VOIDSY);
    fsyms.insert(CONSTSY);
}

void error()
{
    cout << "Format Error(" << cl << ',' << cc << "): Unknown Error\n";
}

void showMsg(string kind, string type, string name, int value)
{
    cout << kind << " " << type << " " << name << " " << value << endl;
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
        Symbol = i;
    } else if (isdigit(ch)) { // number
        Symbol = INTCON;
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
            Symbol = LESY;
            getch();
        } else
            Symbol = LTSY;
    } else if (ch == '>') { // > || >=
        id = ch;
        getch();
        if (ch == '=') {
            id += ch;
            Symbol = GESY;
            getch();
        } else
            Symbol = GTSY;
    } else if (ch == '!') { // !=
        id = ch;
        Symbol = NESY;
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
            Symbol = EQSY;
            getch();
        } else
            Symbol = ASSIGNSY;
    } else if (ch == '\'') { // char const
        Symbol = CHARCON;
        getch();
        if (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '_' || isalnum(ch)) {
            id = ch;
            inum = (int)ch;
            getch();
            if (ch != '\'')
                error();
            else
                getch();
        } else
            error();
    } else if (ch == '\"') { // string const
        Symbol = STRCON;
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
        Symbol = syms[(int)ch];
        getch();
    } else {
        error();
        getch();
    }
}
void skipTo(set<int> valid)
{
    for (getSymbol(); valid.find(Symbol) == valid.end(); getSymbol())
        ;
}
void test(set<int> valid, int errornum)
{
    if (valid.find(Symbol) == valid.end()) {
        error();
        cout << "from test" << endl;
        getSymbol();
        while (valid.find(Symbol) == valid.end())
            getSymbol();
    }
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
int defConst(int level)
{
    getSymbol();
    int tp = Symbol == INTSY ? INTTP : Symbol == CHARSY ? CHARTP : NOTP;
    if (tp == NOTP) {
        error(E_DEFINEERROR);
        return 1;
    }
    getSymbol();
    if (Symbol != IDSY) {
        error(E_DEFINEERROR);
        return 1;
    }
    for (; Symbol == IDSY; getSymbol()) {
        string name = id;
        getSymbol();
        if (Symbol == ASSIGNSY) {
            getSymbol();
            if ((tp == INTTP && Symbol == INTCON) || (tp == CHARTP && Symbol == CHARCON)) {
                int value = inum;
                showMsg("define const", symbolWords[tp], name, value);
                getSymbol();
                if (Symbol == SEMISY) {
                    getSymbol();
                    return 0;
                } else if (Symbol == COMMASY)
                    continue;
                else {
                    error(E_MISSSEMI);
                    return 1;
                }
            } else {
                error(E_INVALIDTYPE);
                return 1;
            }
        } else {
            error(E_DEFINEERROR);
            return 1;
        }
    }
    return 0;
}
int defVar(int level, int back)
{
}
int defFunc(int level, int back) {}
int defMain(int level) {}
int program(int level)
{
    getSymbol();
    test(fsyms, 0);
    while (Symbol == CONSTSY) {
        if (defConst(level))
            skipTo(fsyms);
    }
    test(fsyms, 0);
    while (Symbol == INTSY || Symbol == CHARSY) {
        if (defVar(level, 1))
            break;
    }
    test(fsyms, 0);
    while (Symbol == INTSY || Symbol == CHARSY || Symbol == VOIDSY) {
        if (defFunc(level, 1))
            break;
    }
    test(fsyms, 0);
    if (Symbol == VOIDSY) {
        if (defMain(level) == 0)
            cout << "pofei!\n";
    } else
        error();
    return 0;
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
