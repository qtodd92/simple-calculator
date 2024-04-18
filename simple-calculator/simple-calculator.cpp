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
const char name = 'a';                 // name token
const char let = 'L';                  // declaration token
const string declkey = "let";          // declaration keyword

class Variable {
public:
    string name;
    double value;
};

vector<Variable> var_table;

bool is_declared(string var) 
      // is var already in var_table?
{
    for (const Variable& v : var_table)
        if (v.name == var) return true;
    return false;
}

double define_name(string var, double val)
// add {var,val} to var_table
{
    if (is_declared(var)) error(var, " declared twice");
    var_table.push_back(Variable{ var,val });
    return val;
}

double get_value(string s)
     // return the value of the Variable named s
{
    for (const Variable& v : var_table)
        if (v.name == s) return v.value;
    error("get: undefined variable", s);
}

void set_value(string s, double d)
      // set the Variable named s to d
{
    for (Variable& v : var_table)
        if (v.name == s) {
            v.value = d;
            return;
        }
    error("set: undefined variable", s);
}

class Token {                          // a very simple user-defined type
public:
    char kind;                         // what kind of token
    double value;                      // for numbers: a value
    string name;
    Token() : kind{ 0 } {}                               // default constructor
    Token(char k) :kind{ k }, value{ 0.0 } {}            // initialize kind with ch
    Token(char k, double v) :kind{ k }, value{ v } {}    // initialize kind and value
    Token(char k, string n) :kind{ k }, name{ n } {}     // initialize kind and name
};

class Token_stream {
public:
    Token get();                           // get a Token
    void putback(Token t);                 // put a Token back
    void ignore(char c);                   // discard characters up to and including a c
private:
    bool full{ false };                    // is there a Token in the buffer?
    Token buffer{ 0 };                     // putback() saves its Token here
};

void Token_stream::ignore(char c) 
      // c represents the kind of Token
{
    // first look in buffer
    if (full && c == buffer.kind) {
        full = false;
        return;
    }
    full = false;

    // now search input:
    char ch = 0;
    while (cin >> ch)
        if (ch == c) return;
}

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
    case '=':
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
        if (isalpha(ch)) {            
            string s;
            s += ch;
            while (cin.get(ch) && (isalpha(ch) || isdigit(ch))) s += ch;
            cin.putback(ch);
            if (s == declkey) return Token{ let };    // declaration key
            return Token{ name,s };
        }
        error("Bad token");
    }
}

Token_stream ts;

double expression();

double declaration()
     // assume we have seen "let"
     // handle: name = expression
     // declare a variable called "name" with the initial value "expression"
{
    Token t = ts.get();
    if (t.kind != name) error("name expected in declaration");
    string var_name = t.name;

    Token t2 = ts.get();
    if (t2.kind != '=') error("= missing in declaration of ", var_name);

    double d = expression();
    define_name(var_name, d);
    return d;
}

double statement()
{
    Token t = ts.get();
    switch (t.kind) {
    case let:
        return declaration();
    default:
        ts.putback(t);
        return expression();
    }
}

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
        return t.value;        // return the number's 
    case name:
        return get_value(t.name);
    case '-':
        return -primary();
    case '+':
        return primary();
    default:
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

void clean_up_mess() 
{
    ts.ignore(print);
}

void calculate()                           // expression evaluation loop
{
    while (cin) {
        try {
            cout << prompt;
            Token t = ts.get();
            while (t.kind == print) t = ts.get();           // first discard all "prints"
            if (t.kind == quit) return;
            ts.putback(t);
            cout << result << statement() << '\n';
        }        
        catch (exception& e) {
            cerr << e.what() << '\n';                       // write error message
            clean_up_mess();
        }
    }
}


int main()
{
    try {
        // predefine names:
        define_name("pi", 3.1415926535);
        define_name("e", 2.7182818284);

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

