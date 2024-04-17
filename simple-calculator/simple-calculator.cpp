/*
    Simple calculator

    Revision history:

        Originally written by Quentin Todd Spring 2024.

    This program implements a basic expression calculator.
    Input from cin; output to cout.
    The grammar for input is:

    Statement:
        Expression
        Print
        Quit

    Print:
        ;

    Quit:
        q

    Expression:
        Term
        Expression + Term
        Expression - Term
    Term:
        Primary
        Term * Primary
        Term / Primary
        Term % Primary
    Primary:
        Number
        ( Expression )
        - Primary
        + Primary
    Number:
        floating-point-literal


    Input comes from cin through the Token_stream called ts.
*/


#include "../../std_lib_facilities.h"

const char number = '8';
const char quit = 'q';
const char print = ';';
const string prompt = "> ";
const string result = "= ";            // used to indicate that what follows is a result

class Token {                          // a very simple user-defined type
public:
    char kind;                         // what kind of token
    double value;                      // for numbers: a value
    Token(char k) :kind{ k }, value{ 0.0 } {}            // construct from one value
    Token(char k, double v) :kind{ k }, value{ v } {}    // construct from two values
};

class Token_stream {
public:
    Token get();                           // get a Token
    void putback(Token t);                 // put a Token back
private:
    bool full{ false };                    // is there a Token in the buffer?
    Token buffer{ 0 };                     // putback() saves its Token here
};

void Token_stream::putback(Token t)
{
    if (full) error("putback() int a full buffer");
    buffer = t;                            // copy t to buffer
    full = true;                           // buffer is now full
}

Token Token_stream::get()
      // read characters from cin and compose a Token
{
    if (full) {                            // check if we already have a Token ready
        full = false;                      
        return buffer;
    }

    char ch;
    cin >> ch;                             // note that >> skips whitespace (space, newlines, tab, etc.)

    switch (ch) {
    case quit:                              
    case print:                              
    case '(': 
    case ')': 
    case '+': 
    case '-': 
    case '*': 
    case '/': 
    case '%':
        return Token{ ch };                // let each character represent itself
    case '.':                              // a floating-point-literal can start with a dot
    case '0': case '1': case '2': case '3': case '4': 
    case '5': case '6': case '7': case '8': case '9':          // numeric literal
    {
        cin.putback(ch);                   // put digit back into the input stream
        double val;
        cin >> val;                        // read a floating-point number
        return Token{ number, val };          
    }
    default:
        error("Bad token");
    }
}

Token_stream ts;

double expression();

double primary()
{
    Token t = ts.get();
    switch (t.kind) {
    case '(':   // handle '(' expression ')'
    {
        double d = expression();
        t = ts.get();
        if (t.kind != ')') error("')' expected");
        return d;
    }
    case number:                  
        return t.value;        // return the number's value
    case '-':
        return -primary();
    case '+':
        return primary();
    defualt:
        error("primary expected");
    }
}

double term()
{
    double left = primary();
    Token t = ts.get();                    // get the next Token from the Token stream
    while (true) {
        switch (t.kind) {
        case '*':
            left *= primary();
            t = ts.get();
            break;
        case '/':
        {
            double d = primary();
            if (d == 0) error("divide by zero");
            left /= d;
            t = ts.get();
            break;
        }
        case '%':
        {
            double d = primary();
            if (d == 0) error("%:divide by zero");
            left = fmod(left, d);
            t = ts.get();
            break;
        }
        default:
            ts.putback(t);                   // put t back into the Token stream
            return left;
        }
    }
}

double expression()
{
    double left = term();                   // read and evaluate a Term
    Token t = ts.get();                    // get the next token
    while (true) {
        switch (t.kind) {
        case '+':
            left += term();                // read and evaluate a Term,
            t = ts.get();
            break;
        case '-':
            left -= term();                // read and evaluate a Term,
            t = ts.get();
            break;
        default:
            ts.putback(t);                 // put t back into the token stream
            return left;                   // finally: no more + or -; return the answer
        }
    }
}

void calculate()                           // expression evaluation loop
{
    while (cin) {
        cout << prompt;
        Token t = ts.get();
        while (t.kind == print) t = ts.get();
        if (t.kind == quit) return;
        ts.putback(t);
        cout << result << expression() << '\n';
    }
}


int main()
{
    try {
        calculate();
        keep_window_open();
        return 0;
    }
    catch (exception& e) {
        cerr << e.what() << '\n';
        keep_window_open("~~");
        return 1;
    }
    catch (...) {
        cerr << "exception \n";
        keep_window_open();
        return 2;
    }
}

