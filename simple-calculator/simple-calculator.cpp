#include "../../std_lib_facilities.h"

class Token {                          // a very simple user-defined type
public:
    char kind;                         // what kind of token
    double value;                      // for numbers: a value
    Token(char k) :kind{ k }, value{ 0.0 } {}            // construct from one value
    Token(char k, double v) :kind{ k }, value{ v } {}    // construct from two values
};

double expression()
{
    double left = term();                   // read and evaluate a Term
    Token t = get_token();                  // get the next token
    while (true) {
        switch (t.kind) {
        case '+':
            left += term();                // read and evaluate a Term,
            t = get_token();
            break;
        case '-':
            left -= term();                // read and evaluate a Term,
            t = get_token();
            break;
        default:
            return left;                   // finally: no more + or -; return the answer
        }   
    }
}

double term()
{
    double left = primary();
    Token t = get_token();
    while (true) {
        switch (t.kind) {
        case '*':
            left *= primary();
            t = get_token();
            break;
        case '/':
        {
            double d = primary();
            if (d == 0) error("divide by zero");
            left /= d;
            t = get_token();
            break;
        }
        default:
            return left;
        }
    }
}

double primary()
{
    Token t = get_token();
    switch (t.kind) {
    case '(':   // handle '(' expression ')'
    {
        double d = expression();
        t = get_token();
        if (t.kind != ')') error("')' expected");
        return d;
    }
    case '8':                  // we use '8' to represent a number
        return t.value;        // return the number's value
    defualt:
        error("primary expected");
    }
}

int main()
{
    try {
        while (cin)
            cout << expression() << '\n';
        keep_window_open();
    }
    catch (exception& e) {
        cerr << e.what() << '\n';
        keep_window_open();
        return 1;
    }
    catch (...) {
        cerr << "exception \n";
        keep_window_open();
        return 2;
    }
}

