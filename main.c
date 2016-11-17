#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int debug; // print the executed instructions
int assembly; // print out the assembly and source

int token; // current token

// instructions
enum {
    LEA,
    IMM,
    JMP,
    CALL,
    JZ,
    JNZ,
    ENT,
    ADJ,
    LEV,
    LI,
    LC,
    SI,
    SC,
    PUSH,
    OR,
    XOR,
    AND,
    EQ,
    NE,
    LT,
    GT,
    LE,
    GE,
    SHL,
    SHR,
    ADD,
    SUB,
    MUL,
    DIV,
    SCANF,
    PRTF
};

// tokens and classes (operators last and in precedence order)
// copied from c4
enum {
    Num = 128,
    Asc,
    Cvar,
    Fun,
    Sys,
    Glo,
    Loc,
    Id,
    Char,
    Const,
    Do,
    Else,
    For,
    If,
    Int,
    Return,
    Void,
    While,
    Assign,
    Eq,
    Ne,
    Lt,
    Gt,
    Le,
    Ge,
    Add,
    Mul,
    Sub,
    Div,
    Main
};
enum {
    EVarDecl,
    EReDecl,
    EParamDecl
};

// fields of identifier
enum {
    Token,
    Hash,
    Name,
    Type,
    Class,
    Value,
    BType,
    BClass,
    BValue,
    IdSize
};

/** value:
* For global variable: index into data section
* For local variable: index from base pointer into data section
* For function: offset into code section of the bytecode
* For system function: opcode for the system function
*/

// types of variable/function
enum {
    CHAR,
    INT,
    CCHAR,
    CINT,
    ARRAY
};

int *text, // text segment
    *stack; // stack
int* old_text; // for dump text segment
char* data; // data segment
int* idmain;

char *src, *old_src; // pointer to source code string;

int poolsize; // default size of text/data/stack
int *pc, *bp, *sp, ax, cycle; // virtual machine registers

int *current_id, // current parsed ID
    *symbols, // symbol table
    line, // line number of source code
    token_val; // value of current token (mainly for number)

int basetype; // the type of a declaration, make it global for convenience
int expr_type; // the type of an expression

// function frame
//
// 0: arg 1
// 1: arg 2
// 2: arg 3
// 3: return address
// 4: old bp pointer  <- index_of_bp
// 5: local var 1
// 6: local var 2
int index_of_bp; // index of bp pointer on stack
// for special use
int declaration_index;

void error(int errorNum)
{
    printf("%d: No.%d Error Happend\n", line, errorNum);
}

void next()
{
    char* last_pos;
    int hash;

    while ((token = *src)) {
        ++src;

        if (token == '\n')
            ++line;
        else if ((token >= 'a' && token <= 'z') || (token >= 'A' && token <= 'Z') || (token == '_')) {
            // parse identifier
            last_pos = src - 1;
            hash = token;

            while ((*src >= 'a' && *src <= 'z') || (*src >= 'A' && *src <= 'Z') || (*src >= '0' && *src <= '9') || (*src == '_')) {
                hash = hash * 147 + *src;
                src++;
            }

            // look for existing identifier, linear search
            current_id = symbols;
            while (current_id[Token]) {
                if (current_id[Hash] == hash && !memcmp((char*)current_id[Name], last_pos, src - last_pos)) {
                    //found one, return
                    token = current_id[Token];
                    return;
                }
                current_id = current_id + IdSize;
            }

            // store new ID
            current_id[Name] = (int)last_pos;
            current_id[Hash] = hash;
            token = current_id[Token] = Id;
            return;
        } else if (token >= '0' && token <= '9') {
            // parse number, three kinds: dec(123) hex(0x123) oct(017)
            token_val = token - '0';
            if (token_val > 0) {
                // dec, starts with [1-9]
                while (*src >= '0' && *src <= '9') {
                    token_val = token_val * 10 + *src++ - '0';
                }
            } else {
                // starts with number 0
                while (*src >= '0' && *src <= '9')
                    token_val = token_val * 10 + *src++ - '0';
                if (token_val != 0)
                    error(100);
            }
            token = Num;
            return;
        } else if (token == '/') {
            if (*src == '/') {
                // skip comments
                while (*src != 0 && *src != '\n') {
                    ++src;
                }
            } else {
                // divide operator
                token = Div;
                return;
            }
        } else if (token == '"' || token == '\'') {
            // parse string literal, currently, the only supported escape
            // character is '\n', store the string literal into data.
            last_pos = data;
            while (*src != 0 && *src != token) {
                token_val = *src++;

                if (token == '"') {
                    *data++ = token_val;
                }
            }

            src++;
            // if it is a single character, return Asc token
            if (token == '"') {
                token_val = (int)last_pos;
            } else {
                token = Asc;
            }

            return;
        } else if (token == '=') {
            // parse '==' and '='
            if (*src == '=') {
                src++;
                token = Eq;
            } else {
                token = Assign;
            }
            return;
        } else if (token == '!') {
            // parse '!='
            if (*src == '=') {
                src++;
                token = Ne;
            }
            return;
        } else if (token == '<') {
            // parse '<=', '<<' or '<'
            if (*src == '=') {
                src++;
                token = Le;
            } else {
                token = Lt;
            }
            return;
        } else if (token == '>') {
            // parse '>=', '>>' or '>'
            if (*src == '=') {
                src++;
                token = Ge;
            } else {
                token = Gt;
            }
            return;
        } else if (token == '+') {
            token = Add;
            return;
        } else if (token == '-') {
            token = Sub;
            return;
        } else if (token == '*') {
            token = Mul;
            return;
        } else if (token == '(' || token == ')' || token == '[' || token == ']' || token == '{' || token == '}' || token == ',' || token == ';') {
            // directly return the character as token;
            return;
        }
    }
}

void match(int tk)
{
    if (token == tk) {
        next();
    } else {
        printf("%d: expected token: %d(%c)\n", line, tk, tk);
        exit(-1);
    }
}

void showMsg(char* str)
{
    printf("this is a %s statement\n", str);
}

void expression();
// <因子> ::= <标识符>|<标识符>'['<表达式>']'|<整数>|<字符>|<有返回值函数调用语句>|'('<表达式>')'
void factor()
{
    int tmp;
    if (token == Id) {
        match(Id);
        if (token == '[') {
            match('[');
            expression();
            match(']');
        } else if (token == '(') {
            match('(');
            // pass in arguments
            tmp = 0; // number of arguments
            while (token != ')') {
                expression();
                *++text = PUSH;
                tmp++;
                if (token == ',')
                    match(',');
            }
            match(')');
        }
    } else if (token == Num) {
        match(Num);
    } else if (token == Asc) {
        match(Asc);
    } else if (token == '(') {
        expression();
        match(')');
    } else {
        printf(" unexpected token \n");
        exit(-1);
    }
    showMsg("factor");
}

void term()
{
    factor();
    while (token == Mul || token == Div) {
        match(token);
        factor();
    }
    showMsg("term");
}

void expression()
{
    if (token == '"') {
        match('"');
        showMsg("string");
        return;
    } else if (token == '+')
        match('+');
    else if (token == '-')
        match('-');
    term();
    while (token == Add || token == Sub) {
        match(token);
        term();
    }
    showMsg("expression");
}
// <条件> ::=  <表达式><关系运算符><表达式>|<表达式> //表达式为0条件为假，否则为真
void condition()
{
    expression();
    if (token == Lt || token == Le || token == Gt || token == Ge || token == Ne || token == Eq) {
        match(token);
        expression();
    }
    showMsg("condition");
}

void statement()
{
    // there are 8 kinds of statements here:
    // 1. if (...) <statement> [else <statement>]
    // 2. do <statement> while(...)
    // 3. { <statement> }
    // 4. return xxx;
    // 5. <empty statement>;
    // 6. expression; (expression end with semicolon including functions.)

    int *a, *b; // bess for branch control
    int tmp;
    if (token == If) {
        // if (...) <statement> [else <statement>]
        //
        //   if (...)           <cond>
        //                      JZ a
        //     <statement>      <statement>
        //   else:              JMP b
        // a:
        //     <statement>      <statement>
        // b:                   b:
        //
        //
        match(If);
        match('(');
        condition(); // parse condition
        match(')');

        // emit code for if
        *++text = JZ;
        b = ++text;

        statement(); // parse statement
        showMsg("if");
        if (token == Else) { // parse else
            match(Else);

            // emit code for JMP B
            *b = (int)(text + 3);
            *++text = JMP;
            b = ++text;

            statement();
            showMsg("else");
        }

        *b = (int)(text + 1);
    } else if (token == Do) {
        //
        // a:                     a:
        //    do                 nop
        //      <statement>      <statement>
        //    while (<cond>)     <cond>
        //                          JZ a
        // b:                     b:
        match(Do);

        a = text + 1;
        while (token != While)
            statement();
        match(While);
        match('(');
        condition();
        match(')');
        showMsg("do-while");
        *++text = JZ;
        b = ++text;

        *++text = JMP;
        *++text = (int)a;
        *b = (int)(text + 1);
    } else if (token == For) {
        match(For);

        match('(');
        if (token == Id) {
            match(Id);
            match(Assign);
            expression();
        }
        match(';');
        condition();
        match(';');
        if (token == Id) {
            match(Id);
            match(Assign);
            match(Id);
            if (token == Add || token == Sub) {
                match(token);
                match(Num);
            }
        }
        match(')');
        statement();
        showMsg("for");
    } else if (token == '{') {
        // { <statement> ... }
        match('{');
        while (token != '}')
            statement();
        match('}');
        showMsg("statements");
    } else if (token == Return) {
        // return [expression];
        match(Return);
        if (token != ';') {
            match('(');
            expression();
            match(')');
        }
        match(';');
        showMsg("return");
        // emit code for return
        *++text = LEV;
    } else if (token == ';') {
        // empty statement
        match(';');
        showMsg("blank");
    } else if (token == Id) {
        // a = b; a[1]=b; or function_call();
        match(Id);
        if (token == Assign) {
            match(Assign);
            expression();
            showMsg("assign");
        } else if (token == '[') {
            match('[');
            expression();
            match(']');
            match(Assign);
            expression();
            showMsg("assign");
        } else if (token == '(') {
            match('(');
            // pass in arguments
            tmp = 0; // number of arguments
            while (token != ')') {
                expression();
                *++text = PUSH;
                tmp++;
                if (token == ',')
                    match(',');
            }
            match(')');
            showMsg("call function");
        }
        match(';');
    }
    showMsg("statement");
}

void function_parameter()
{
    int type;
    int params;
    params = 0;
    while (token != ')') {
        // int name, ...
        type = INT;
        if (token == Int) {
            match(Int);
        } else if (token == Char) {
            type = CHAR;
            match(Char);
        }

        // parameter name
        if (token != Id)
            error(EParamDecl);
        if (current_id[Class] == Loc)
            error(EReDecl);

        match(Id);
        // store the local variable
        current_id[BClass] = current_id[Class];
        current_id[Class] = Loc;
        current_id[BType] = current_id[Type];
        current_id[Type] = type;
        current_id[BValue] = current_id[Value];
        current_id[Value] = params++; // index of current parameter

        if (token == ',') {
            match(',');
        }
    }
    index_of_bp = params + 1;
    showMsg("function_parameter");
}

void function_body()
{
    // type func_name (...) {...}
    //                   -->|   |<--

    // ... {
    // 1. local declarations
    // 2. statements
    // }

    int pos_local; // position of local variables on the stack.
    int type;
    pos_local = index_of_bp;

    while (token == Const) {
        match(Const);
        if (token != Int && token != Char)
            error(EVarDecl);
        basetype = (token == Int) ? CINT : CCHAR;
        match(token);
        type = basetype;
        while (token != ';') {
            if (token != Id)
                error(EVarDecl);
            if (current_id[Class] == Loc)
                error(EReDecl);
            match(Id);
            if (token != Assign)
                error(EVarDecl);
            match(Assign);
            if ((token == Num && type != CINT) || (token == Asc && type != CCHAR))
                error(EVarDecl);
            match(token);

            // store the local variable
            current_id[BClass] = current_id[Class];
            current_id[Class] = Loc;
            current_id[BType] = current_id[Type];
            current_id[Type] = type;
            current_id[BValue] = current_id[Value];
            current_id[Value] = ++pos_local; // index of current parameter
            if (token == ',')
                match(',');
        }
        match(';');
        showMsg("const");
    }
    while (token == Int || token == Char) {
        // local variable declaration, just like global ones.
        basetype = (token == Int) ? INT : CHAR;
        match(token);

        while (token != ';') {
            type = basetype;
            if (token != Id)
                error(EVarDecl);
            if (current_id[Class] == Loc)
                error(EReDecl);
            match(Id);

            // store the local variable
            current_id[BClass] = current_id[Class];
            current_id[Class] = Loc;
            current_id[BType] = current_id[Type];
            current_id[Type] = type;
            current_id[BValue] = current_id[Value];
            current_id[Value] = ++pos_local; // index of current parameter
            if (token == ',')
                match(',');
        }
        match(';');
        showMsg("int or char");
    }

    // save the stack size for local variables
    *++text = ENT;
    *++text = pos_local - index_of_bp;

    // statements
    while (token != '}') {
        statement();
    }

    // emit code for leaving the sub function
    *++text = LEV;
    showMsg("function_body");
}

void function_declaration()
{
    // type func_name (...) {...}
    //               | this part

    match('(');
    function_parameter();
    match(')');
    match('{');
    function_body();
    //match('}');

    // unwind local variable declarations for all local variables.
    current_id = symbols;
    while (current_id[Token]) {
        if (current_id[Class] == Loc) {
            current_id[Class] = current_id[BClass];
            current_id[Type] = current_id[BType];
            current_id[Value] = current_id[BValue];
        }
        current_id = current_id + IdSize;
    }
    showMsg("function_declaration");
}

void global_declaration(int* declaration_index)
{
    // const int id=1{,id2=1};
    // int id([1]){,id([1])};

    int type; // tmp, actual type for variable

    basetype = INT;

    if (token == Const) {
        if (*declaration_index != 0)
            error(12);
        match(Const);
        basetype = CINT;
        if (token == Int)
            match(Int);
        else if (token == Char) {
            match(Char);
            basetype = CCHAR;
        }
        type = basetype;
        current_id[Type] = type;
        while (token != ';') {
            // invalid declaration
            if (token != Id)
                error(EVarDecl);
            // identifier exists
            if (current_id[Class])
                error(EReDecl);
            match(Id);

            if (token != Assign)
                error(EVarDecl);
            match(Assign);
            if ((token == Asc && type != CCHAR) || (token == Num && type != CINT))
                error(EVarDecl);
            current_id[Class] = Glo;
            current_id[Value] = token_val;
            match(token);
            if (token == ',')
                match(',');
        }
        match(';');
        return;
    }

    // parse type information
    *declaration_index = *declaration_index > 1 ? *declaration_index : 1;
    if (token == Int)
        match(Int);
    else if (token == Char) {
        match(Char);
        basetype = CHAR;
    } else if (token == Void) {
        match(Void);
    }
    // parse the comma seperated variable declaration.
    while (token != ';' && token != '}') { //for variables and functions
        type = basetype;
        // invalid declaration
        if (token != Id)
            error(EVarDecl);
        // identifier exists
        if (current_id[Class])
            error(EReDecl);
        match(Id);
        current_id[Type] = type;
        if (token == '[') {
            match('[');
            if (token != Num)
                error(EVarDecl);
            current_id[Class] = Glo;
            current_id[Value] = token_val;
            current_id[Type] += ARRAY;
            match(Num);
            if (token != ']')
                error(EVarDecl);
            match(']');
            if (*declaration_index != 1)
                error(0);
        } else if (token == '(') {
            *declaration_index = 2;
            current_id[Class] = Fun;
            current_id[Value] = (int)(text + 1); // the memory address of function
            function_declaration();
        } else {
            // variable declaration
            if (*declaration_index != 1)
                error(0);
            current_id[Class] = Glo; // global variable
            current_id[Value] = (int)data; // assign memory address
            data = data + sizeof(int);
        }

        if (token == ',')
            match(',');
    }
    next();
}

void program()
{
    // get next token
    next();
    declaration_index = 0;
    while (token > 0)
        global_declaration(&declaration_index);
    showMsg("program");
}

int main(int argc, char** argv)
{
    int i, fd;

    argc--;
    argv++;

    // parse arguments
    if (argc > 0 && **argv == '-' && (*argv)[1] == 'd') {
        debug = 1;
        --argc;
        ++argv;
    }
    if (argc < 1) {
        printf("no input file\n");
        return -1;
    }

    if ((fd = open(*argv, 0)) < 0) {
        printf("could not open(%s)\n", *argv);
        return -1;
    }

    poolsize = 256 * 1024; // arbitrary size
    line = 1;

    // allocate memory
    if (!(text = malloc(poolsize))) {
        printf("could not malloc(%d) for text area\n", poolsize);
        return -1;
    }
    if (!(data = malloc(poolsize))) {
        printf("could not malloc(%d) for data area\n", poolsize);
        return -1;
    }
    if (!(stack = malloc(poolsize))) {
        printf("could not malloc(%d) for stack area\n", poolsize);
        return -1;
    }
    if (!(symbols = malloc(poolsize))) {
        printf("could not malloc(%d) for symbol table\n", poolsize);
        return -1;
    }

    memset(text, 0, poolsize);
    memset(data, 0, poolsize);
    memset(stack, 0, poolsize);
    memset(symbols, 0, poolsize);

    old_text = text;

    src = "char const do else for if int return void while "
          "scanf printf main";

    // add keywords to symbol table
    i = Char;
    while (i <= While) {
        next();
        current_id[Token] = i++;
    }

    // add library to symbol table
    i = SCANF;
    while (i <= PRTF) {
        next();
        current_id[Class] = Sys;
        current_id[Type] = INT;
        current_id[Value] = i++;
    }

    next();
    idmain = current_id; // keep track of main

    if (!(src = old_src = malloc(poolsize))) {
        printf("could not malloc(%d) for source area\n", poolsize);
        return -1;
    }
    // read the source file
    if ((i = read(fd, src, poolsize - 1)) <= 0) {
        printf("read() returned %d\n", i);
        return -1;
    }
    src[i] = 0; // add EOF character
    close(fd);

    program();

    if (!(pc = (int*)idmain[Value])) {
        printf("main() not defined\n");
        return -1;
    }

    // only for compile
    return 0;
}
