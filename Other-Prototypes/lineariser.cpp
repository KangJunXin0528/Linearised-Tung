#include <iostream>
#include <string>
#include <algorithm>
#include <cmath>     // Required for log(), log10(), and sqrt()
#include <limits>    // Required for clearing the input buffer capacity limits
#include <cctype>    // Required for isspace()
#include <thread>    // Required for sleeping/delays
#include <chrono>    // Required for time units

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

// --- NEW MECHANICS PHYSICS FORMULAS ---

// 10. Hooke's Law (F = k * x)
void fl10() {
    print_line("\n--- Hooke's Law Lineariser ---", 500);
    print_line("Original Equation: F = k * x", 500);
    
    double k;
    cout << "Enter spring constant 'k': ";
    cin >> k; corrector(k);

    double m = k;
    double c = 0.0;
    
    print_line("\nLinearised Form (Y = mX + C):", 500);
    print_line("   Y = " + to_string(m) + " * X + " + to_string(c), 500);
    print_line("   (Where Y = F and X = x)\n\n\n", 500);
}

// 11. Kinetic Energy (E_k = 0.5 * m * v^2)
void fl11() {
    print_line("\n--- Kinetic Energy Lineariser ---", 500);
    print_line("Original Equation: E_k = 0.5 * m * v^2", 500);
    
    double mass;
    cout << "Enter mass 'm': ";
    cin >> mass; corrector(mass);

    double m = 0.5 * mass;
    double c = 0.0;
    
    print_line("\nLinearised Form (Y = mX + C):", 500);
    print_line("   Y = " + to_string(m) + " * X + " + to_string(c), 500);
    print_line("   (Where Y = E_k and X = v^2)\n\n\n", 500);
}

// 12. Centripetal Force (F_c = m * v^2 / r)
void fl12() {
    print_line("\n--- Centripetal Force Lineariser ---", 500);
    print_line("Original Equation: F_c = (m * v^2) / r", 500);
    
    double mass, radius;
    cout << "Enter mass 'm': ";
    cin >> mass; corrector(mass);
    cout << "Enter radius 'r' (must not be 0): ";
    cin >> radius; corrector(radius);
    while (radius == 0) { cout << "Radius cannot be 0: "; cin >> radius; corrector(radius); }

    double m = mass / radius;
    double c = 0.0;
    
    print_line("\nLinearised Form (Y = mX + C):", 500);
    print_line("   Y = " + to_string(m) + " * X + " + to_string(c), 500);
    print_line("   (Where Y = F_c and X = v^2)\n\n\n", 500);
}

// 13. Universal Gravitation (F = G * m1 * m2 / r^2)
void fl13() {
    print_line("\n--- Universal Gravitation Lineariser ---", 500);
    print_line("Original Equation: F = (G * m_1 * m_2) / r^2", 500);
    
    double m1, m2;
    const double G = 6.67430e-11; // Gravitational constant
    cout << "Enter mass 1 'm_1': ";
    cin >> m1; corrector(m1);
    cout << "Enter mass 2 'm_2': ";
    cin >> m2; corrector(m2);

    double m = G * m1 * m2;
    double c = 0.0;
    
    print_line("\nLinearised Form (Y = mX + C):", 500);
    print_line("   Y = " + to_string(m) + " * X + " + to_string(c), 500);
    print_line("   (Where Y = F and X = 1 / r^2)\n\n\n", 500);
}

// --- NEW DC & AC CIRCUIT FORMULAS ---

// 14. Ohm's Law (V = I * R)
void fl14() {
    print_line("\n--- Ohm's Law Lineariser ---", 500);
    print_line("Original Equation: V = I * R", 500);
    
    double R;
    cout << "Enter resistance 'R' (Ohms): ";
    cin >> R; corrector(R);

    double m = R;
    double c = 0.0;
    
    print_line("\nLinearised Form (Y = mX + C):", 500);
    print_line("   Y = " + to_string(m) + " * X + " + to_string(c), 500);
    print_line("   (Where Y = V and X = I)\n\n\n", 500);
}

// 15. Electric Power (P = I^2 * R)
void fl15() {
    print_line("\n--- Electric Power Lineariser ---", 500);
    print_line("Original Equation: P = I^2 * R", 500);
    
    double R;
    cout << "Enter resistance 'R' (Ohms): ";
    cin >> R; corrector(R);

    double m = R;
    double c = 0.0;
    
    print_line("\nLinearised Form (Y = mX + C):", 500);
    print_line("   Y = " + to_string(m) + " * X + " + to_string(c), 500);
    print_line("   (Where Y = P and X = I^2)\n\n\n", 500);
}

// 16. Inductive Reactance (X_L = 2 * pi * f * L)
void fl16() {
    print_line("\n--- Inductive Reactance Lineariser ---", 500);
    print_line("Original Equation: X_L = 2 * pi * f * L", 500);
    
    double L;
    cout << "Enter inductance 'L' (Henries): ";
    cin >> L; corrector(L);

    const double PI = 3.14159265358979323846;
    double m = 2 * PI * L;
    double c = 0.0;
    
    print_line("\nLinearised Form (Y = mX + C):", 500);
    print_line("   Y = " + to_string(m) + " * X + " + to_string(c), 500);
    print_line("   (Where Y = X_L and X = f)\n\n\n", 500);
}

// 17. Capacitive Reactance (X_C = 1 / (2 * pi * f * C))
void fl17() {
    print_line("\n--- Capacitive Reactance Lineariser ---", 500);
    print_line("Original Equation: X_C = 1 / (2 * pi * f * C)", 500);
    
    double C;
    cout << "Enter capacitance 'C' (Farads, must not be 0): ";
    cin >> C; corrector(C);
    while (C == 0) { cout << "C cannot be 0: "; cin >> C; corrector(C); }

    const double PI = 3.14159265358979323846;
    double m = 1.0 / (2 * PI * C);
    double c = 0.0;
    
    print_line("\nLinearised Form (Y = mX + C):", 500);
    print_line("   Y = " + to_string(m) + " * X + " + to_string(c), 500);
    print_line("   (Where Y = X_C and X = 1 / f)\n\n\n", 500);
}

void formula_lineariser() {
    int choice;

    while (true) {
        print_line("==================================================", 50);
        print_line("          SELECT A FORMULA TO LINEARISE           ", 50);
        print_line("==================================================", 50);
        print_line("--- General / Chemical & RC ---", 50);
        print_line("1. Exponential Growth/Decay [y = a * e^(b*x)]", 50);
        print_line("2. Power Law Relationship   [y = a * x^b]", 50);
        print_line("3. Simple Pendulum Period   [T = 2 * pi * sqrt(L/g)]", 50);
        print_line("4. Inverse Square Law       [I = k / d^2]", 50);
        print_line("5. Lineweaver-Burk          [v_0 = (V_max * [S]) / (K_m + [S])]", 50);
        print_line("6. Arrhenius Equation       [k = A * e^(-E_a / (R*T))]", 50);
        print_line("7. Hyperbolic Rational Form [y = x / (ax + b)]", 50);
        print_line("8. RC Circuit Voltage Decay [V = V_0 * e^(-t / (R*C))]", 50);
        print_line("9. Kinematic Distance       [d = 0.5 * a * t^2]", 50);
        print_line("--- Mechanics Physics ---", 50);
        print_line("10. Hooke's Law             [F = k * x]", 50);
        print_line("11. Kinetic Energy          [E_k = 0.5 * m * v^2]", 50);
        print_line("12. Centripetal Force       [F_c = (m * v^2) / r]", 50);
        print_line("13. Universal Gravitation   [F = (G * m_1 * m_2) / r^2]", 50);
        print_line("--- DC & AC Circuits ---", 50);
        print_line("14. Ohm's Law               [V = I * R]", 50);
        print_line("15. Electric Power          [P = I^2 * R]", 50);
        print_line("16. Inductive Reactance     [X_L = 2 * pi * f * L]", 50);
        print_line("17. Capacitive Reactance    [X_C = 1 / (2 * pi * f * C)]", 50);
        print_line("18. Exit", 50);
        print_line("--------------------------------------------------", 50);
        cout << "Enter your selection (1-18): ";

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
            case 10: fl10(); break;
            case 11: fl11(); break;
            case 12: fl12(); break;
            case 13: fl13(); break;
            case 14: fl14(); break;
            case 15: fl15(); break;
            case 16: fl16(); break;
            case 17: fl17(); break;
            case 18: 
                return;
            default:
                print_line("Invalid choice. Please select between 1 and 18.\n", 500);
                break;
        }
    }
}

// Helper function to remove all spaces from user inputs
string removeSpaces(string str) {
    str.erase(remove(str.begin(), str.end(), ' '), str.end());
    return str;
}




void lineariseFormula() {
    string lhs, rhs;
    char indepVar, depVar;

    // ADD THIS LINE HERE to clear the menu's leftover Enter key press
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    cout << "--- Formula Lineariser ---\n";

    // 1. Input Left-Hand Side (Loop until valid)
    while (true) {
        cout << "1. Enter Left-Hand Side (LHS): ";
        getline(cin, lhs);
        lhs = removeSpaces(lhs);

        if (lhs.empty()) {
            cout << "Error: LHS cannot be empty. Please try again.\n\n";
        } else if (lhs.find('=') != string::npos) {
            cout << "Error: The '=' symbol is not allowed inside the LHS. Please try again.\n\n";
        } else {
            break; 
        }
    }

    // 2. Input Right-Hand Side (Loop until valid)
    while (true) {
        cout << "2. Enter Right-Hand Side (RHS): ";
        getline(cin, rhs);
        rhs = removeSpaces(rhs);

        if (rhs.empty()) {
            cout << "Error: RHS cannot be empty. Please try again.\n\n";
        } else if (rhs.find('=') != string::npos) {
            cout << "Error: The '=' symbol is not allowed inside the RHS. Please try again.\n\n";
        } else {
            break; 
        }
    }

    // Combine both sides to verify variable existence
    string fullEquation = lhs + rhs;

    // 3. Input Independent Variable (Loop until valid)
    while (true) {
        cout << "3. Enter Independent Variable: ";
        cin >> indepVar;
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); 

        if (fullEquation.find(indepVar) == string::npos) {
            cout << "Error: Independent variable must exist in the formula (" << lhs << " = " << rhs << "). Try again.\n\n";
        } else {
            break; 
        }
    }

    // 4. Input Dependent Variable (Loop until valid)
    while (true) {
        cout << "4. Enter Dependent Variable (to solve for): ";
        cin >> depVar;
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); 

        if (fullEquation.find(depVar) == string::npos) {
            cout << "Error: Dependent variable must exist in the formula (" << lhs << " = " << rhs << "). Try again.\n\n";
        } else {
            break; 
        }
    }

    // 5. Perform mathematical manipulations 
    cout << "\nRearranging formula...\n";
    
    string currentLHS = lhs;
    string currentRHS = rhs;

    bool isolationComplete = false;
    while (!isolationComplete) {
        
        // Always make sure the side containing our target dependent variable is on the LHS
        if (currentRHS.find(depVar) != string::npos && currentLHS.find(depVar) == string::npos) {
            string temp = currentLHS;
            currentLHS = currentRHS;
            currentRHS = temp;
        }

        // CASE 0: Fully Solved!
        if (currentLHS.length() == 1 && currentLHS[0] == depVar) {
            isolationComplete = true;
            break;
        } 

        // CASE 1: Addition and Subtraction (e.g., y/3 + x/2 = 1)
        if (currentLHS.find('+') != string::npos || currentLHS.find('-') != string::npos) {
            size_t opPos = currentLHS.find_first_of("+-");
            char op = currentLHS[opPos];
            
            string term1 = currentLHS.substr(0, opPos);
            string term2 = currentLHS.substr(opPos + 1);

            if (term1.find(depVar) != string::npos) {
                currentLHS = term1;
                currentRHS = (op == '+') ? "(" + currentRHS + ") - (" + term2 + ")" : "(" + currentRHS + ") + (" + term2 + ")";
            }
            else if (term2.find(depVar) != string::npos) {
                currentLHS = term2;
                currentRHS = (op == '+') ? "(" + currentRHS + ") - (" + term1 + ")" : "(" + term1 + ") - (" + currentRHS + ")";
            } else {
                cout << "Error: Target variable missing from expression.\n";
                return;
            }
        }
        // CASE 2: Division/Fraction formats (e.g., y/3 = 1 - x/2)
        else if (currentLHS.find('/') != string::npos) {
            size_t slashPos = currentLHS.find('/');
            string numerator = currentLHS.substr(0, slashPos);
            string denominator = currentLHS.substr(slashPos + 1);

            if (numerator.find(depVar) != string::npos) {
                currentLHS = numerator;
                currentRHS = "(" + currentRHS + ") * " + denominator; 
            } else {
                cout << "Error: Target trapped in denominator.\n";
                return;
            }
        } 
        // CASE 3: Multiplication formats (e.g., ma = F -> solving for m)
        else if (currentLHS.length() > 1 && currentLHS.find_first_of("+/-*") == string::npos) {
            size_t targetPos = currentLHS.find(depVar);
            if (targetPos != string::npos) {
                // Fixed ternary statement to extract chars instead of full string references
                char otherVar = (targetPos == 0) ? currentLHS[1] : currentLHS[0];
                currentLHS = string(1, depVar);
                // Wrapped otherVar safely in a string to avoid string literal pointer errors
                currentRHS = "(" + currentRHS + ") / " + string(1, otherVar);
            } else {
                cout << "Error: Target variable mismatch.\n";
                return;
            }
        } 
        else {
            cout << "Error: Formula manipulation stalled or expression too complex.\n";
            return;
        }
    }

    // Output Final Result
    cout << "Output: " << currentLHS << " = " << currentRHS << "\n";
}

int main() {
    int choices;

    while (true) {
        // Minimalist Menu Display with a 0.1-second (100ms) delay per line
        print_line("\n=== MATH LINEARISER SOFTWARE SUITE ===", 100);
        print_line("  1. Dynamic Algorithmic Lineariser", 100);
        print_line("  2. Preset Formula Lineariser", 100);
        print_line("  3. Exit Program", 100);
        print_line("======================================", 100);
        cout << "Selection (1-3): ";

        cin >> choices;
        menu_corrector(choices); 

        if (choices == 1) {
            // Displays notices cleanly with distinct reading paces
            print_line("\n[Notice] Please DO NOT use decimals (e.g. 0.5) in this engine.", 500);
            print_line("         Use whole numbers or variables only.\n", 500);
            
            lineariseFormula(); 
        } 
        else if (choices == 2) {
            print_line("\n[Notice] Please DO NOT use text fractions (e.g. 1/2) for constant values.", 500);
            print_line("         Enter them as decimals (e.g. 0.5) instead.\n", 500);
            
            formula_lineariser(); 
        } 
        else if (choices == 3) {
            print_line("\nGoodbye!", 500);
            break;
        } 
        else {
            print_line("\nInvalid choice. Try again.", 500);
        }
    }

    return 0;
}

