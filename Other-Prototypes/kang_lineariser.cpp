#include <iostream>
#include <string>
#include <algorithm>
#include <limits>    // Required for clearing the input buffer capacity limits

// Helper function to remove all spaces from user inputs
std::string removeSpaces(std::string str) {
    str.erase(std::remove(str.begin(), str.end(), ' '), str.end());
    return str;
}

void lineariseFormula() {
    std::string lhs, rhs;
    char indepVar, depVar;

    std::cout << "--- Formula Lineariser ---\n";

    // 1. Input Left-Hand Side (Loop until valid)
    while (true) {
        std::cout << "1. Enter Left-Hand Side (LHS): ";
        std::getline(std::cin, lhs);
        lhs = removeSpaces(lhs);

        if (lhs.empty()) {
            std::cout << "Error: LHS cannot be empty. Please try again.\n\n";
        } else if (lhs.find('=') != std::string::npos) {
            std::cout << "Error: The '=' symbol is not allowed inside the LHS. Please try again.\n\n";
        } else {
            break; 
        }
    }

    // 2. Input Right-Hand Side (Loop until valid)
    while (true) {
        std::cout << "2. Enter Right-Hand Side (RHS): ";
        std::getline(std::cin, rhs);
        rhs = removeSpaces(rhs);

        if (rhs.empty()) {
            std::cout << "Error: RHS cannot be empty. Please try again.\n\n";
        } else if (rhs.find('=') != std::string::npos) {
            std::cout << "Error: The '=' symbol is not allowed inside the RHS. Please try again.\n\n";
        } else {
            break; 
        }
    }

    // Combine both sides to verify variable existence
    std::string fullEquation = lhs + rhs;

    // 3. Input Independent Variable (Loop until valid)
    while (true) {
        std::cout << "3. Enter Independent Variable: ";
        std::cin >> indepVar;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 

        if (fullEquation.find(indepVar) == std::string::npos) {
            std::cout << "Error: Independent variable must exist in the formula (" << lhs << " = " << rhs << "). Try again.\n\n";
        } else {
            break; 
        }
    }

    // 4. Input Dependent Variable (Loop until valid)
    while (true) {
        std::cout << "4. Enter Dependent Variable (to solve for): ";
        std::cin >> depVar;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 

        if (fullEquation.find(depVar) == std::string::npos) {
            std::cout << "Error: Dependent variable must exist in the formula (" << lhs << " = " << rhs << "). Try again.\n\n";
        } else {
            break; 
        }
    }

        // 5. Perform mathematical manipulations 
    std::cout << "\nRearranging formula...\n";
    
    std::string currentLHS = lhs;
    std::string currentRHS = rhs;

    bool isolationComplete = false;
    while (!isolationComplete) {
        
        // Always make sure the side containing our target dependent variable is on the LHS
        if (currentRHS.find(depVar) != std::string::npos && currentLHS.find(depVar) == std::string::npos) {
            std::string temp = currentLHS;
            currentLHS = currentRHS;
            currentRHS = temp;
        }

        // CASE 0: Fully Solved!
        if (currentLHS.length() == 1 && currentLHS[0] == depVar) {
            isolationComplete = true;
            break;
        } 

        // CASE 1: Addition and Subtraction (e.g., y/3 + x/2 = 1)
        if (currentLHS.find('+') != std::string::npos || currentLHS.find('-') != std::string::npos) {
            size_t opPos = currentLHS.find_first_of("+-");
            char op = currentLHS[opPos];
            
            std::string term1 = currentLHS.substr(0, opPos);
            std::string term2 = currentLHS.substr(opPos + 1);

            if (term1.find(depVar) != std::string::npos) {
                currentLHS = term1;
                currentRHS = (op == '+') ? "(" + currentRHS + ") - (" + term2 + ")" : "(" + currentRHS + ") + (" + term2 + ")";
            }
            else if (term2.find(depVar) != std::string::npos) {
                currentLHS = term2;
                currentRHS = (op == '+') ? "(" + currentRHS + ") - (" + term1 + ")" : "(" + term1 + ") - (" + currentRHS + ")";
            } else {
                std::cout << "Error: Target variable missing from expression.\n";
                return;
            }
        }
        // CASE 2: Division/Fraction formats (e.g., y/3 = 1 - x/2)
        else if (currentLHS.find('/') != std::string::npos) {
            size_t slashPos = currentLHS.find('/');
            std::string numerator = currentLHS.substr(0, slashPos);
            std::string denominator = currentLHS.substr(slashPos + 1);

            if (numerator.find(depVar) != std::string::npos) {
                currentLHS = numerator;
                currentRHS = "(" + currentRHS + ") * " + denominator; 
            } else {
                std::cout << "Error: Target trapped in denominator.\n";
                return;
            }
        } 
        // CASE 3: Multiplication formats (e.g., ma = F -> solving for m)
        else if (currentLHS.length() > 1 && currentLHS.find_first_of("+/-*") == std::string::npos) {
            size_t targetPos = currentLHS.find(depVar);
            if (targetPos != std::string::npos) {
                char otherVar = (targetPos == 0) ? currentLHS[1] : currentLHS[0];
                currentLHS = std::string(1, depVar);
                currentRHS = "(" + currentRHS + ") / " + otherVar;
            } else {
                std::cout << "Error: Target variable mismatch.\n";
                return;
            }
        } 
        else {
            std::cout << "Error: Formula manipulation stalled or expression too complex.\n";
            return;
        }
    }

    // Output Final Result
    std::cout << "Output: " << currentLHS << " = " << currentRHS << "\n";
}

int main() {
    lineariseFormula();
    return 0;
}
