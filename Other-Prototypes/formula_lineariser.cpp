#include <iostream>
#include <string>
#include <cmath>     // Required for log(), log10(), and sqrt()
#include <limits>    // Required for numeric_limits
#include <cctype>    // Required for isspace()
#include <thread>    // Required for sleeping/delays (NEW)
#include <chrono>    // Required for time units (NEW)

using namespace std;

// Helper function to print a line of text and pause for a specific duration
void print_line(const string &text, int delay_ms) {
    cout << text << "\n";
    this_thread::sleep_for(chrono::milliseconds(delay_ms));
}

// Fixes double inputs (handles trailing letters like '2b' immediately)
void corrector(double &input) {
    while (cin.fail() || (cin.peek() != '\n' && cin.peek() != EOF && !isspace(cin.peek()))) {
        cin.clear(); 
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
        cout << "Invalid input. Please enter a valid number: ";
        cin >> input;
    }
}

// Fixes integer inputs for the main menu selection
void menu_corrector(int &input) {
    while (cin.fail() || (cin.peek() != '\n' && cin.peek() != EOF && !isspace(cin.peek()))) {
        cin.clear(); 
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
        cout << "Invalid selection. Please enter a valid menu number: ";
        cin >> input;
    }
}

// 1. Exponential Growth (y = a * e^(b * x))
void fl1() {
    print_line("\n--- Exponential Growth Lineariser ---", 500);
    print_line("Original Equation: y = a * e^(b * x)", 500);
    
    double a, b;
    cout << "Enter coefficient 'a' (must be > 0): ";
    cin >> a; corrector(a);
    while (a <= 0) { cout << "Value must be greater than 0: "; cin >> a; corrector(a); }
    
    cout << "Enter growth rate 'b': ";
    cin >> b; corrector(b);

    double m = b;
    double c = log(a); 
    
    print_line("\nLinearised Form (Y = mX + C):", 500);
    print_line("   Y = " + to_string(m) + " * X + " + to_string(c), 500);
    print_line("   (Where Y = ln(y) and X = x)\n\n\n", 500);
}

// 2. Power Law Relationship (y = a * x^b)
void fl2() {
    print_line("\n--- Power Law Lineariser ---", 500);
    print_line("Original Equation: y = a * x^b", 500);
    
    double a, b;
    cout << "Enter coefficient 'a' (must be > 0): ";
    cin >> a; corrector(a);
    while (a <= 0) { cout << "Value must be greater than 0: "; cin >> a; corrector(a); }
    
    cout << "Enter exponent 'b': ";
    cin >> b; corrector(b);

    double m = b;
    double c = log10(a);
    
    print_line("\nLinearised Form (Y = mX + C):", 500);
    print_line("   Y = " + to_string(m) + " * X + " + to_string(c), 500);
    print_line("   (Where Y = log10(y) and X = log10(x))\n\n\n", 500);
}

// 3. Simple Pendulum Period (T = 2*pi * sqrt(L/g))
void fl3() {
    print_line("\n--- Simple Pendulum Lineariser ---", 500);
    print_line("Original Equation: T = 2 * pi * sqrt(L / g)", 500);
    
    double g;
    cout << "Enter gravitational acceleration 'g' (e.g., 9.81): ";
    cin >> g; corrector(g);
    while (g == 0) { cout << "g cannot be 0. Enter a valid g: "; cin >> g; corrector(g); }

    const double PI = 3.14159265358979323846;
    double m = (4 * PI * PI) / g;
    double c = 0.0;
    
    print_line("\nLinearised Form (Y = mX + C):", 500);
    print_line("   Y = " + to_string(m) + " * X + " + to_string(c), 500);
    print_line("   (Where Y = T^2 and X = L)\n\n\n", 500);
}

// 4. Inverse Square Law (I = k / d^2)
void fl4() {
    print_line("\n--- Inverse Square Law Lineariser ---", 500);
    print_line("Original Equation: I = k / d^2", 500);
    
    double k;
    cout << "Enter proportionality constant 'k': ";
    cin >> k; corrector(k);

    double m = k;
    double c = 0.0;
    
    print_line("\nLinearised Form (Y = mX + C):", 500);
    print_line("   Y = " + to_string(m) + " * X + " + to_string(c), 500);
    print_line("   (Where Y = I and X = 1 / d^2)\n\n\n", 500);
}

// 5. Michaelis-Menten Kinetics (v0 = (Vmax * [S]) / (Km + [S]))
void fl5() {
    print_line("\n--- Lineweaver-Burk Enzyme Lineariser ---", 500);
    print_line("Original Equation: v_0 = (V_max * [S]) / (K_m + [S])", 500);
    
    double Vmax, Km;
    cout << "Enter V_max (maximum velocity, must not be 0): ";
    cin >> Vmax; corrector(Vmax);
    while (Vmax == 0) { cout << "V_max cannot be 0: "; cin >> Vmax; corrector(Vmax); }
    
    cout << "Enter K_m (Michaelis constant): ";
    cin >> Km; corrector(Km);

    double m = Km / Vmax;
    double c = 1.0 / Vmax;
    
    print_line("\nLinearised Form (Y = mX + C):", 500);
    print_line("   Y = " + to_string(m) + " * X + " + to_string(c), 500);
    print_line("   (Where Y = 1 / v_0 and X = 1 / [S])\n\n\n", 500);
}

// 6. Arrhenius Equation (k = A * e^(-Ea / (R * T)))
void fl6() {
    print_line("\n--- Arrhenius Reaction Lineariser ---", 500);
    print_line("Original Equation: k = A * e^(-E_a / (R * T))", 500);
    
    double A, Ea;
    const double R = 8.314; // Ideal gas constant J/(mol*K)
    cout << "Enter frequency factor 'A' (must be > 0): ";
    cin >> A; corrector(A);
    while (A <= 0) { cout << "A must be greater than 0: "; cin >> A; corrector(A); }
    
    cout << "Enter activation energy 'E_a' (in J/mol): ";
    cin >> Ea; corrector(Ea);

    double m = -Ea / R;
    double c = log(A);
    
    print_line("\nLinearised Form (Y = mX + C):", 500);
    print_line("   Y = " + to_string(m) + " * X + " + to_string(c), 500);
    print_line("   (Where Y = ln(k) and X = 1 / T)\n\n\n", 500);
}

// 7. Hyperbolic Rational Form (y = x / (ax + b))
void fl7() {
    print_line("\n--- Hyperbolic Rational Lineariser ---", 500);
    print_line("Original Equation: y = x / (ax + b)", 500);
    
    double a, b;
    cout << "Enter parameter 'a': ";
    cin >> a; corrector(a);
    cout << "Enter parameter 'b': ";
    cin >> b; corrector(b);

    double m = a;
    double c = b;
    
    print_line("\nLinearised Form (Y = mX + C):", 500);
    print_line("   Y = " + to_string(m) + " * X + " + to_string(c), 500);
    print_line("   (Where Y = x / y and X = x)\n\n\n", 500);
}

// 8. Capacitive Voltage Decay (V = V0 * e^(-t / (R * C)))
void fl8() {
    print_line("\n--- RC Circuit Voltage Decay Lineariser ---", 500);
    print_line("Original Equation: V = V_0 * e^(-t / (R * C))", 500);
    
    double V0, R, C;
    cout << "Enter initial voltage 'V_0' (must be > 0): ";
    cin >> V0; corrector(V0);
    while (V0 <= 0) { cout << "V_0 must be greater than 0: "; cin >> V0; corrector(V0); }
    
    cout << "Enter resistance 'R' (Ohms, must not be 0): ";
    cin >> R; corrector(R);
    
    cout << "Enter capacitance 'C' (Farads, must not be 0): ";
    cin >> C; corrector(C);
    while (R * C == 0) { cout << "R * C cannot multiply to 0. Re-enter C: "; cin >> C; corrector(C); }

    double m = -1.0 / (R * C);
    double c = log(V0);
    
    print_line("\nLinearised Form (Y = mX + C):", 500);
    print_line("   Y = " + to_string(m) + " * X + " + to_string(c), 500);
    print_line("   (Where Y = ln(V) and X = t)\n\n\n", 500);
}

// 9. Kinematic Distance (d = 0.5 * a * t^2)
void fl9() {
    print_line("\n--- Kinematic Distance Lineariser ---", 500);
    print_line("Original Equation: d = 0.5 * a * t^2", 500);
    
    double a;
    cout << "Enter constant acceleration 'a': ";
    cin >> a; corrector(a);

    double m = 0.5 * a;
    double c = 0.0;
    
    print_line("\nLinearised Form (Y = mX + C):", 500);
    print_line("   Y = " + to_string(m) + " * X + " + to_string(c), 500);
    print_line("   (Where Y = d and X = t^2)\n\n\n", 500);
}

void formula_lineariser() {
    
    int choice;

    while (true) {
        // Main menu displays quickly with a 100ms (0.1s) delay per line
        print_line("========================================", 100);
        print_line("       SELECT A FORMULA TO LINEARISE    ", 100);
        print_line("========================================", 100);
        print_line("1. Exponential Growth/Decay [y = a * e^(b*x)]", 100);
        print_line("2. Power Law Relationship   [y = a * x^b]", 100);
        print_line("3. Simple Pendulum Period   [T = 2 * pi * sqrt(L/g)]", 100);
        print_line("4. Inverse Square Law       [I = k / d^2]", 100);
        print_line("5. Lineweaver-Burk          [v_0 = (V_max * [S]) / (K_m + [S])]", 100);
        print_line("6. Arrhenius Equation       [k = A * e^(-E_a / (R*T))]", 100);
        print_line("7. Hyperbolic Rational Form [y = x / (ax + b)]", 100);
        print_line("8. RC Circuit Voltage Decay [V = V_0 * e^(-t / (R*C))]", 100);
        print_line("9. Kinematic Distance       [d = 0.5 * a * t^2]", 100);
        print_line("10. Goodbye (Exit)", 100);
        print_line("----------------------------------------", 100);
        cout << "Enter your selection (1-10): ";

        cin >> choice;
        menu_corrector(choice);

        switch (choice) {
            case 1: fl1(); break;
            case 2: fl2(); break;
            case 3: fl3(); break;
            case 4: fl4(); break;
            case 5: fl5(); break;
            case 6: fl6(); break;
            case 7: fl7(); break;
            case 8: fl8(); break;
            case 9: fl9(); break;
            case 10: 
                print_line("Goodbye!", 500);
                return;
                
            default:
                print_line("Invalid choice. Please select between 1 and 10.\n", 500);
                break;
        }
    }

}


int main() {
    formula_lineariser();
    return 0;
}