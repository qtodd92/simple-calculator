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

int main()
{
    cout << "Please enter expression (we can handle +, -, * and /)\n";
    cout << "add an x to end expression (e.g., 1+2*3x): ";
    int lval = 0;
    int rval;
    cin >> lval;                       // read leftmost operand
    if (!cin) error("no first operand");
    for (char op; cin >> op; ) {       // read operator and right-hand operand
                                       // repeatedly
        if (op != 'x') cin >> rval;
        if (!cin) error("no second operand");
        switch (op) {
        case '+':
            lval += rval;              // add: lval = lval + rval
            break;
        case '-':
            lval -= rval;              // subtract: lval = lval - rval
            break;
        case '*':
            lval *= rval;              // multiply: lval = lval * rval
            break;
        case '/':
            lval /= rval;              // divide: lval = lval / rval
            break;
        default:                       // not another operator: print result
            cout << "Result: " << lval << '\n';
            keep_window_open();
            return 0;
        }   
    }    
    error("bad expression");
}

