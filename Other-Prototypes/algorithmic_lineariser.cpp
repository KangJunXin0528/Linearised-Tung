#include <iostream>
#include <string>
#include <algorithm>
#include <limits>    // Required for clearing the input buffer capacity limits

using namespace std;

// Helper function to remove all spaces from user inputs
string removeSpaces(string str) {
    str.erase(remove(str.begin(), str.end(), ' '), str.end());
    return str;
}

void lineariseFormula() {
    string lhs, rhs;
    char indepVar, depVar;

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
    lineariseFormula();
    return 0;
}
