#include "defs.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>

using namespace std;

int symbol;
string id;
stringstream ss;
int inum;
char ch;
string curLine;
int cl, cc, ll;

int syms[256];
string symbolWords[100];
int rword; // number of the rwords.
string rwords[12];

void setup()
{
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

    rword = 11;
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

void error()
{
    cout << "Format Error(" << cl << ',' << cc << "): Unknown Error\n";
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
        for (i = 1; i <= rword; ++i)
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
    } else
        error();
}

int main()
{
    setup();
    int word = 0;
    while (1) {
        getSymbol();
        cout << ++word << " : " << symbolWords[symbol] << ' ' << id << "\n";
    }
    return 0;
}
