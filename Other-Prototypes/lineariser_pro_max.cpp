#include <iostream>
#include <string>
#include <algorithm>
#include <cmath>     
#include <limits>    
#include <cctype>    
#include <vector>
#include <sstream>

using namespace std;

// Struct to pass output data cleanly back to a GUI[cite: 1]
struct LinearisationResult {
    bool success;
    string message;
    string formulaY;
    string formulaX;
};

// Helper function to remove all spaces from user inputs[cite: 1]
string removeSpaces(string str) {
    str.erase(remove(str.begin(), str.end(), ' '), str.end());
    return str;
}

// ==========================================
// PART 2: CORE DYNAMIC ALGORITHMIC LINEARISER[cite: 1]
// ==========================================

LinearisationResult runAlgorithmicLineariser(string lhs, string rhs, string indepVar, string depVar) {
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

    string fullEquation = lhs + "=" + rhs;
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

        if (currentLHS == depVar) {
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
        else if (currentLHS.length() > depVar.length() && currentLHS.find_first_of("+/-*") == string::npos) {
            size_t targetPos = currentLHS.find(depVar);
            if (targetPos != string::npos) {
                string otherVar = (targetPos == 0) ? currentLHS.substr(depVar.length()) : currentLHS.substr(0, targetPos);
                currentLHS = depVar;
                currentRHS = "(" + currentRHS + ") / " + otherVar;
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
// PART 3: DISPATCHER / ROUTER (GUI BRIDGE)[cite: 1]
// ==========================================

vector<string> getFormulaTokens(string str) {
    vector<string> tokens;
    string cleaned = removeSpaces(str);
    string currentToken = "";

    for (size_t i = 0; i < cleaned.length(); ++i) {
        char c = cleaned[i];
        
        if (isalnum(c) || c == '_' || c == '.') {
            currentToken += c;
        } else {
            if (!currentToken.empty()) {
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

    for (string &t : tokens) {
        t.erase(remove(t.begin(), t.end(), '_'), t.end());
    }
    sort(tokens.begin(), tokens.end());
    return tokens;
}

LinearisationResult processFormulaInput(string fullInputEquation, string lhs, string rhs, string indepVar, string depVar) {
    vector<string> userTokens = getFormulaTokens(fullInputEquation);

    // Preset library including the newly added logarithmic/square root law (Preset #18)
    vector<pair<string, int>> library = {
        {"y=a*e^(b*x)", 1}, {"y=ax^b", 2}, {"T=2*pi*sqrt(L/g)", 3},
        {"I=k/d^2", 4},     {"v_0=(V_max*[S])/(K_m+[S])", 5}, {"k=A*e^(-E_a/(R*T))", 6},
        {"y=x/(ax+b)", 7},  {"V=V_0*e^(-t/(R*C))", 8}, {"d=0.5*a*t^2", 9},
        {"F=k*x", 10},      {"E_k=0.5*m*v^2", 11}, {"F_c=(m*v^2)/r", 12},
        {"F=(G*m_1*m_2)/r^2", 13}, {"V=I*R", 14}, {"P=I^2*R", 15},
        {"X_L=2*pi*f*L", 16}, {"X_C=1/(2*pi*f*C)", 17},
        {"T=2*pi*sqrt(m/k)", 18} // Logarithmic equation preset added here
    };

    int discoveredLibraryID = -1;

    for (auto const& item : library) {
        vector<string> templateTokens = getFormulaTokens(item.first);
        if (userTokens == templateTokens) {
            discoveredLibraryID = item.second;
            break;
        }
    }

    LinearisationResult res;
    res.success = true;

    if (discoveredLibraryID != -1) {
        res.message = "[System Router] Match found in preset library! Running Library Formula #" + to_string(discoveredLibraryID) + "\n";
        switch (discoveredLibraryID) {
            case 1: res.formulaY = "ln(y)"; res.formulaX = "x"; res.message += "   Y = b * X + ln(a)\n   (Where Y = ln(y) and X = x)"; break;
            case 2: res.formulaY = "log10(y)"; res.formulaX = "log10(x)"; res.message += "   Y = b * X + log10(a)\n   (Where Y = log10(y) and X = log10(x))"; break;
            case 3: res.formulaY = "T^2"; res.formulaX = "L"; res.message += "   Y = (4*pi^2/g) * X + 0\n   (Where Y = T^2 and X = L)"; break;
            case 4: res.formulaY = "I"; res.formulaX = "1 / d^2"; res.message += "   Y = k * X + 0\n   (Where Y = I and X = 1 / d^2)"; break;
            case 5: res.formulaY = "1 / v_0"; res.formulaX = "1 / [S]"; res.message += "   Y = (K_m/V_max) * X + (1/V_max)\n   (Where Y = 1 / v_0 and X = 1 / [S])"; break;
            case 6: res.formulaY = "ln(k)"; res.formulaX = "1 / T"; res.message += "   Y = (-E_a/R) * X + ln(A)\n   (Where Y = ln(k) and X = 1 / T)"; break;
            case 7: res.formulaY = "x / y"; res.formulaX = "x"; res.message += "   Y = a * X + b\n   (Where Y = x / y and X = x)"; break;
            case 8: res.formulaY = "ln(V)"; res.formulaX = "t"; res.message += "   Y = (-1/(R*C)) * X + ln(V_0)\n   (Where Y = ln(V) and X = t)"; break;
            case 9: res.formulaY = "d"; res.formulaX = "t^2"; res.message += "   Y = (0.5*a) * X + 0\n   (Where Y = d and X = t^2)"; break;
            case 10: res.formulaY = "F"; res.formulaX = "x"; res.message += "   Y = k * X + 0\n   (Where Y = F and X = x)"; break;
            case 11: res.formulaY = "E_k"; res.formulaX = "v^2"; res.message += "   Y = (0.5*m) * X + 0\n   (Where Y = E_k and X = v^2)"; break;
            case 12: res.formulaY = "F_c"; res.formulaX = "v^2"; res.message += "   Y = (m/r) * X + 0\n   (Where Y = F_c and X = v^2)"; break;
            case 13: res.formulaY = "F"; res.formulaX = "1 / r^2"; res.message += "   Y = (G*m_1*m_2) * X + 0\n   (Where Y = F and X = 1 / r^2)"; break;
            case 14: res.formulaY = "V"; res.formulaX = "I"; res.message += "   Y = R * X + 0\n   (Where Y = V and X = I)"; break;
            case 15: res.formulaY = "P"; res.formulaX = "I^2"; res.message += "   Y = R * X + 0\n   (Where Y = P and X = I^2)"; break;
            case 16: res.formulaY = "X_L"; res.formulaX = "f"; res.message += "   Y = (2*pi*L) * X + 0\n   (Where Y = X_L and X = f)"; break;
            case 17: res.formulaY = "X_C"; res.formulaX = "1 / f"; res.message += "   Y = (1/(2*pi*C)) * X + 0\n   (Where Y = X_C and X = 1 / f)"; break;
            case 18: 
                res.formulaY = "log(T)"; 
                res.formulaX = "log(m)"; 
                res.message += "   Y = 0.5 * X + log(2*pi) - 0.5*log(k)\n   (Where Y = log(T), X = log(m), slope m = 1/2, and intercept c = log(2*pi) - 0.5*log(k))"; 
                break;
            default: res.message += "   Formula configurations matching error."; break;
        }
    } else {
        cout << "[System Router] No matching character composition found. Falling back to Dynamic Algorithmic Lineariser...\n";
        res = runAlgorithmicLineariser(lhs, rhs, indepVar, depVar);
    }

    return res;
}

int main() {
    string lhs, rhs, fullEquation;
    string indepVar, depVar;

    cout << "=== MATH LINEARISER TEST SUITE ===\n";
    cout << "Must enter * for multiplication and / for division\n\n";
    
    cout << "Enter Left-Hand Side (LHS): ";
    getline(cin, lhs);
    
    cout << "Enter Right-Hand Side (RHS): ";
    getline(cin, rhs);

    fullEquation = lhs + "=" + rhs;

    cout << "Enter Independent Variable (e.g., x, t, m): ";
    cin >> indepVar;
    
    cout << "Enter Dependent Variable to isolate (e.g., y, T): ";
    cin >> depVar;

    cout << "\n--------------------------------------------------\n";
    cout << "Processing: " << fullEquation << "\n";
    cout << "--------------------------------------------------\n";

    LinearisationResult result = processFormulaInput(fullEquation, lhs, rhs, indepVar, depVar);
    cout << result.message << "\n";

    return 0;
}