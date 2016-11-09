#include "defs.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>

using namespace std;

int symbol;
char ch;

void error()
{
    cout << "uknown error\n";
}

void getch()
{
    if (cin >> ch) {
        cout << "program incomplete!\n";
        exit(0);
    }
}

void getSymbol()
{
    while (ch == ' ' || ch == '\n' || ch == '\t')
        getch();
    if (isalpha(ch) || ch == '_') { // alpha

    } else if (isdigit(ch)) { // number

    } else if (ch == '\'') { // char

    } else if (ch == '\"') { // string

    } else if (ch == '<') { // < || <=
        getch();
        if (ch == '=') {
            symbol = LESY;
            getch();
        } else
            symbol = LTSY;
    } else if (ch == '>') { // > || >=
        getch();
        if (ch == '=') {
            symbol = GESY;
            getch();
        } else
            symbol = GTSY;
    } else if (ch == '!') { // !=

    } else if (ch == '=') { // ==

    } else if (strchr("+-*/()[]{},;", ch)) { // other operators

    } else
        error();
}
int main()
{
    getch();

    return 0;
}
