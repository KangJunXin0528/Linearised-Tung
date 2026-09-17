#include <iostream>
#include <string>
#include <cmath>     
#include <limits>    
#include <cctype>    
#include <thread>    
#include <chrono>    

using namespace std;

void print_line(const string &text, int delay_ms) {
    cout << text << "\n";
    this_thread::sleep_for(chrono::milliseconds(delay_ms));
}

void corrector(double &input) {
    while (cin.fail() || (cin.peek() != '\n' && cin.peek() != EOF && !isspace(cin.peek()))) {
        cin.clear(); 
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
        cout << "Invalid input. Please enter a valid number: ";
        cin >> input;
    }
}

void menu_corrector(int &input) {
    while (cin.fail() || (cin.peek() != '\n' && cin.peek() != EOF && !isspace(cin.peek()))) {
        cin.clear(); 
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
        cout << "Invalid selection. Please enter a valid menu number: ";
        cin >> input;
    }
}

void fl1() {
    print_line("\n--- Exponential Growth Lineariser ---", 500);
    double a, b;
    cout << "Enter coefficient 'a': "; cin >> a; corrector(a);
    cout << "Enter growth rate 'b': "; cin >> b; corrector(b);
    double m = b; double c = log(a); 
    print_line("Y = " + to_string(m) + " * X + " + to_string(c), 500);
}

void fl2() {
    print_line("\n--- Power Law Lineariser ---", 500);
    double a, b;
    cout << "Enter coefficient 'a': "; cin >> a; corrector(a);
    cout << "Enter exponent 'b': "; cin >> b; corrector(b);
    double m = b; double c = log10(a);
    print_line("Y = " + to_string(m) + " * X + " + to_string(c), 500);
}

void fl3() {
    print_line("\n--- Simple Pendulum Lineariser ---", 500);
    double g;
    cout << "Enter 'g': "; cin >> g; corrector(g);
    const double PI = 3.14159265358979323846;
    double m = (4 * PI * PI) / g;
    print_line("Y = " + to_string(m) + " * X + 0", 500);
}

int main() {
    fl1();
    return 0;
}
