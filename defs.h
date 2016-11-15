#ifndef _DEFS_H_
#define _DEFS_H_

#define NOTP 0
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

#define CONSTOBJ 37
#define VAROBJ 38
#define ARRAYOBJ 37
#define FUNCOBJ 37

#define INTTP 37
#define CHARTP 38
#define FUNCTP 39
#define ARRAYTP 40
// for code generate
#define TABLE_NUM 32768
#define CODE_NUM 32768
// error code
#define E_NOFILE 1
#define E_INCOMPLETE 2
#define E_INVALIDCHAR 3
#define E_INVALIDEXP 4
#define E_DEFINEERROR 5
#define E_UNDEFINE 6
#define E_REDEFINE 7
#define E_CONVERTION 8
#define E_NOTWRITABLE 9
#define E_MISSSEMI 10
#define E_MISSCOMMA 11
#define E_MISSLPAR 12
#define E_MISSRPAR 13
#define E_MISSLBRK 14
#define E_MISSRBRK 15
#define E_MISSLCUR 16
#define E_MISSRCUR 17
#define E_NORETURN 18
#define E_INVALIDTYPE 19
#endif