#include "defs.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>

using namespace std;

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
