#include <iostream>
#include <string>
#include <algorithm>
#include <cmath>     
#include <limits>    
#include <cctype>    
#include <vector>
#include <sstream>

using namespace std;

// Struct to pass output data cleanly back to a GUI
struct LinearisationResult {
    bool success;
    string message;
    string formulaY;
    string formulaX;
};

// Helper function to remove all spaces from user inputs
string removeSpaces(string str) {
    str.erase(remove(str.begin(), str.end(), ' '), str.end());
    return str;
}

// ==========================================
// PART 1: CORE PRESET LIBRARY ALGORITHMS
// ==========================================

string fl1_GetResult(double a, double b) {
    if (a <= 0) return "Error: Value 'a' must be greater than 0.";
    double m = b;
    double c = log(a); 
    return "Y = " + to_string(m) + " * X + " + to_string(c) + "  (Where Y = ln(y) and X = x)";
}

string fl2_GetResult(double a, double b) {
    if (a <= 0) return "Error: Value 'a' must be greater than 0.";
    double m = b;
    double c = log10(a);
    return "Y = " + to_string(m) + " * X + " + to_string(c) + "  (Where Y = log10(y) and X = log10(x))";
}

string fl3_GetResult(double g) {
    if (g == 0) return "Error: g cannot be 0.";
    const double PI = 3.14159265358979323846;
    double m = (4 * PI * PI) / g;
    return "Y = " + to_string(m) + " * X + 0.0  (Where Y = T^2 and X = L)";
}

string fl4_GetResult(double k) {
    return "Y = " + to_string(k) + " * X + 0.0  (Where Y = I and X = 1 / d^2)";
}

string fl5_GetResult(double Vmax, double Km) {
    if (Vmax == 0) return "Error: V_max cannot be 0.";
    double m = Km / Vmax;
    double c = 1.0 / Vmax;
    return "Y = " + to_string(m) + " * X + " + to_string(c) + "  (Where Y = 1 / v_0 and X = 1 / [S])";
}

string fl6_GetResult(double A, double Ea) {
    if (A <= 0) return "Error: A must be greater than 0.";
    const double R = 8.314;
    double m = -Ea / R;
    double c = log(A);
    return "Y = " + to_string(m) + " * X + " + to_string(c) + "  (Where Y = ln(k) and X = 1 / T)";
}

string fl7_GetResult(double a, double b) {
    return "Y = " + to_string(a) + " * X + " + to_string(b) + "  (Where Y = x / y and X = x)";
}

string fl8_GetResult(double V0, double R, double C) {
    if (V0 <= 0) return "Error: V_0 must be greater than 0.";
    if (R * C == 0) return "Error: R * C cannot multiply to 0.";
    double m = -1.0 / (R * C);
    double c = log(V0);
    return "Y = " + to_string(m) + " * X + " + to_string(c) + "  (Where Y = ln(V) and X = t)";
}

string fl9_GetResult(double a) {
    return "Y = " + to_string(0.5 * a) + " * X + 0.0  (Where Y = d and X = t^2)";
}

string fl10_GetResult(double k) {
    return "Y = " + to_string(k) + " * X + 0.0  (Where Y = F and X = x)";
}

string fl11_GetResult(double mass) {
    return "Y = " + to_string(0.5 * mass) + " * X + 0.0  (Where Y = E_k and X = v^2)";
}

string fl12_GetResult(double mass, double radius) {
    if (radius == 0) return "Error: Radius cannot be 0.";
    return "Y = " + to_string(mass / radius) + " * X + 0.0  (Where Y = F_c and X = v^2)";
}

string fl13_GetResult(double m1, double m2) {
    const double G = 6.67430e-11;
    return "Y = " + to_string(G * m1 * m2) + " * X + 0.0  (Where Y = F and X = 1 / r^2)";
}

string fl14_GetResult(double R) {
    return "Y = " + to_string(R) + " * X + 0.0  (Where Y = V and X = I)";
}

string fl15_GetResult(double R) {
    return "Y = " + to_string(R) + " * X + 0.0  (Where Y = P and X = I^2)";
}

string fl16_GetResult(double L) {
    const double PI = 3.14159265358979323846;
    return "Y = " + to_string(2 * PI * L) + " * X + 0.0  (Where Y = X_L and X = f)";
}

string fl17_GetResult(double C) {
    if (C == 0) return "Error: C cannot be 0.";
    const double PI = 3.14159265358979323846;
    return "Y = " + to_string(1.0 / (2 * PI * C)) + " * X + 0.0  (Where Y = X_C and X = 1 / f)";
}


// ==========================================
// PART 2: CORE DYNAMIC ALGORITHMIC LINEARISER
// ==========================================

LinearisationResult runAlgorithmicLineariser(string lhs, string rhs, char indepVar, char depVar) {
    LinearisationResult res;
    res.success = false;

    lhs = removeSpaces(lhs);
    rhs = removeSpaces(rhs);

    if (lhs.empty() || rhs.empty()) {
        res.message = "Error: LHS and RHS cannot be empty.";
        return res;
    }
    if (lhs.find('=') != string::npos || rhs.find('=') != string::npos) {
        res.message = "Error: The '=' symbol is not allowed inside either side.";
        return res;
    }

    string fullEquation = lhs + rhs;
    if (fullEquation.find(indepVar) == string::npos || fullEquation.find(depVar) == string::npos) {
        res.message = "Error: Variable keys must explicitly exist inside the string inputs.";
        return res;
    }

    string currentLHS = lhs;
    string currentRHS = rhs;
    bool isolationComplete = false;

    while (!isolationComplete) {
        if (currentRHS.find(depVar) != string::npos && currentLHS.find(depVar) == string::npos) {
            swap(currentLHS, currentRHS);
        }

        if (currentLHS.length() == 1 && currentLHS[0] == depVar) {
            isolationComplete = true;
            break;
        } 

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
                res.message = "Error: Target variable missing from expression.";
                return res;
            }
        }
        else if (currentLHS.find('/') != string::npos) {
            size_t slashPos = currentLHS.find('/');
            string numerator = currentLHS.substr(0, slashPos);
            string denominator = currentLHS.substr(slashPos + 1);

            if (numerator.find(depVar) != string::npos) {
                currentLHS = numerator;
                currentRHS = "(" + currentRHS + ") * " + denominator; 
            } else {
                res.message = "Error: Target trapped in denominator.";
                return res;
            }
        } 
        else if (currentLHS.length() > 1 && currentLHS.find_first_of("+/-*") == string::npos) {
            size_t targetPos = currentLHS.find(depVar);
            if (targetPos != string::npos) {
                char otherVar = (targetPos == 0) ? currentLHS[1] : currentLHS[0];
                currentLHS = string(1, depVar);
                currentRHS = "(" + currentRHS + ") / " + string(1, otherVar);
            } else {
                res.message = "Error: Target variable mismatch.";
                return res;
            }
        } 
        else {
            res.message = "Error: Formula manipulation stalled or expression too complex.";
            return res;
        }
    }

    res.success = true;
    res.message = "Success: " + currentLHS + " = " + currentRHS;
    return res;
}


// ==========================================
// PART 3: DISPATCHER / ROUTER (GUI BRIDGE)
// ==========================================

// Helper to tokenize an equation string into clean individual variables/words and numeric blocks
vector<string> getFormulaTokens(string str) {
    vector<string> tokens;
    string cleaned = removeSpaces(str);
    string currentToken = "";

    for (size_t i = 0; i < cleaned.length(); ++i) {
        char c = cleaned[i];
        
        // Treat alphanumeric characters, underscores, and decimal points as parts of tokens
        if (isalnum(c) || c == '_' || c == '.') {
            currentToken += c;
        } else {
            // Cut off the token when hitting a math symbol like =, +, *, /, ^, (, )
            if (!currentToken.empty()) {
                // Filter out "pi" explicitly so geometric/circuit constants don't pollute variables
                if (currentToken != "pi") {
                    tokens.push_back(currentToken);
                }
                currentToken = "";
            }
        }
    }
    if (!currentToken.empty() && currentToken != "pi") {
        tokens.push_back(currentToken);
    }

    // Sort tokens alphabetically so ["Ek", "m", "v", "0.5"] matches ["0.5", "E_k", "m", "v"] 
    // note: we normalize "E_k" and "Ek" variations by removing underscores during token comparison
    for (string &t : tokens) {
        t.erase(remove(t.begin(), t.end(), '_'), t.end());
    }
    sort(tokens.begin(), tokens.end());
    return tokens;
}

void processFormulaInput(string fullInputEquation, string lhs, string rhs, char indepVar, char depVar) {
    // 1. Tokenize and sort the user's input composition
    vector<string> userTokens = getFormulaTokens(fullInputEquation);

    // 2. Define the exact library templates
    vector<pair<string, int>> library = {
        {"y=a*e^(b*x)", 1}, {"y=ax^b", 2}, {"T=2*pi*sqrt(L/g)", 3},
        {"I=k/d^2", 4},     {"v_0=(V_max*[S])/(K_m+[S])", 5}, {"k=A*e^(-E_a/(R*T))", 6},
        {"y=x/(ax+b)", 7},  {"V=V_0*e^(-t/(R*C))", 8}, {"d=0.5*a*t^2", 9},
        {"F=k*x", 10},      {"E_k=0.5*m*v^2", 11}, {"F_c=(m*v^2)/r", 12},
        {"F=(G*m_1*m_2)/r^2", 13}, {"V=I*R", 14}, {"P=I^2*R", 15},
        {"X_L=2*pi*f*L", 16}, {"X_C=1/(2*pi*f*C)", 17}
    };

    int discoveredLibraryID = -1;

    // 3. Scan the library using matching token blocks
    for (auto const& item : library) {
        vector<string> templateTokens = getFormulaTokens(item.first);
        if (userTokens == templateTokens) {
            discoveredLibraryID = item.second;
            break;
        }
    }

    // 4. ROUTER PROCESSOR
    if (discoveredLibraryID != -1) {
        cout << "[System Router] Match found in preset library! Running Library Formula #" << discoveredLibraryID << "...\n";
        cout << "Linearised Form (Y = mX + C):\n";

        switch (discoveredLibraryID) {
            case 1:
                cout << "   Y = b * X + ln(a)\n   (Where Y = ln(y) and X = x)\n";
                break;
            case 2:
                cout << "   Y = b * X + log10(a)\n   (Where Y = log10(y) and X = log10(x))\n";
                break;
            case 3:
                cout << "   Y = (4*pi^2/g) * X + 0\n   (Where Y = T^2 and X = L)\n";
                break;
            case 4:
                cout << "   Y = k * X + 0\n   (Where Y = I and X = 1 / d^2)\n";
                break;
            case 5:
                cout << "   Y = (K_m/V_max) * X + (1/V_max)\n   (Where Y = 1 / v_0 and X = 1 / [S])\n";
                break;
            case 6:
                cout << "   Y = (-E_a/R) * X + ln(A)\n   (Where Y = ln(k) and X = 1 / T)\n";
                break;
            case 7:
                cout << "   Y = a * X + b\n   (Where Y = x / y and X = x)\n";
                break;
            case 8:
                cout << "   Y = (-1/(R*C)) * X + ln(V_0)\n   (Where Y = ln(V) and X = t)\n";
                break;
            case 9:
                cout << "   Y = (0.5*a) * X + 0\n   (Where Y = d and X = t^2)\n";
                break;
            case 10:
                cout << "   Y = k * X + 0\n   (Where Y = F and X = x)\n";
                break;
            case 11:
                cout << "   Y = (0.5*m) * X + 0\n   (Where Y = E_k and X = v^2)\n";
                break;
            case 12:
                cout << "   Y = (m/r) * X + 0\n   (Where Y = F_c and X = v^2)\n";
                break;
            case 13:
                cout << "   Y = (G*m_1*m_2) * X + 0\n   (Where Y = F and X = 1 / r^2)\n";
                break;
            case 14:
                cout << "   Y = R * X + 0\n   (Where Y = V and X = I)\n";
                break;
            case 15:
                cout << "   Y = R * X + 0\n   (Where Y = P and X = I^2)\n";
                break;
            case 16:
                cout << "   Y = (2*pi*L) * X + 0\n   (Where Y = X_L and X = f)\n";
                break;
            case 17:
                cout << "   Y = (1/(2*pi*C)) * X + 0\n   (Where Y = X_C and X = 1 / f)\n";
                break;
            default:
                cout << "   Formula configurations matching error.\n";
                break;
        }
    } 
    // IF NO MATCH FOUND: Divert natively straight to the Algorithmic Lineariser
    else {
        cout << "[System Router] No matching character composition found. Falling back to Dynamic Algorithmic Lineariser...\n";
        LinearisationResult res = runAlgorithmicLineariser(lhs, rhs, indepVar, depVar);
        cout << res.message << "\n";
    }
}

// Simple console implementation showing how cleanly the engine bridges inputs
int main() {
    string lhs, rhs, fullEquation;
    char indepVar, depVar;

    cout << "=== MATH LINEARISER TEST SUITE ===\n";
    cout << "must enter * for multiplication and / for division\n\n";
    
    // 1. Collect equations inputs
    cout << "Enter Left-Hand Side (LHS): ";
    getline(cin, lhs);
    
    cout << "Enter Right-Hand Side (RHS): ";
    getline(cin, rhs);

    // Build the full equation string to check against the preset library templates
    fullEquation = lhs + "=" + rhs;

    // 2. Collect variable targeting rules for the algorithmic system
    cout << "Enter Independent Variable character: ";
    cin >> indepVar;
    
    cout << "Enter Dependent Variable character (to isolate): ";
    cin >> depVar;

    cout << "\n--------------------------------------------------\n";
    cout << "Processing: " << fullEquation << "\n";
    cout << "--------------------------------------------------\n";

    // 3. Run the automated router system
    processFormulaInput(fullEquation, lhs, rhs, indepVar, depVar);

    return 0;
}