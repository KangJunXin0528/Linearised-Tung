#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "implot.h"
#include <GLFW/glfw3.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <vector>
#include <algorithm>
#include <filesystem>
#include <cstdio>
#include <cstring>
#include <ctime>

// ---------------------------------------------------------------------------
// Shared types / state
// ---------------------------------------------------------------------------

enum class AppScreen {
    MainLobby,
    GraphPlotter,
    FormulaLineariser,
    Credits
};

struct DataPoint {
    double x;
    double y;
};

// Set by the "Save Graph as Image" button; handled once per frame in main()
// right after rendering (so the captured pixels include everything drawn).
static bool g_saveGraphRequested = false;

// ---------------------------------------------------------------------------
// Screenshot helper: grabs the current framebuffer and writes it as a PNG.
// ---------------------------------------------------------------------------

static void SaveWindowScreenshotAsPng(GLFWwindow* window) {
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    if (width <= 0 || height <= 0) return;

    const int channels = 3; // RGB
    std::vector<unsigned char> pixels((size_t)width * height * channels);
    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());

    // OpenGL's origin is bottom-left; images are stored top-left first, so flip rows.
    std::vector<unsigned char> flipped((size_t)width * height * channels);
    int rowSize = width * channels;
    for (int y = 0; y < height; y++) {
        memcpy(&flipped[(size_t)y * rowSize], &pixels[(size_t)(height - 1 - y) * rowSize], rowSize);
    }

    std::filesystem::path dir = "C:\\Personal\\Tertier\\S1 Digital\\Project\\calculator\\graphs";
    std::error_code ec;
    std::filesystem::create_directories(dir, ec); // ok if it already exists

    std::time_t t = std::time(nullptr);
    char nameBuf[64];
    std::strftime(nameBuf, sizeof(nameBuf), "graph_%Y%m%d_%H%M%S.png", std::localtime(&t));

    std::filesystem::path filepath = dir / nameBuf;
    stbi_write_png(filepath.string().c_str(), width, height, channels, flipped.data(), rowSize);
}

// ---------------------------------------------------------------------------
// Texture loading helper (used for the Main Lobby background image)
// ---------------------------------------------------------------------------

static bool LoadTextureFromFile(const char* filename, GLuint* out_texture, int* out_width, int* out_height) {
    int width, height, channels;
    unsigned char* data = stbi_load(filename, &width, &height, &channels, 4); // force RGBA
    if (data == nullptr) return false;

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    stbi_image_free(data);

    *out_texture = texture;
    *out_width = width;
    *out_height = height;
    return true;
}

// Loaded once in main(); 0 means "not loaded" / "failed to load".
static GLuint g_LobbyBackgroundTexture = 0;
static int g_LobbyBgWidth = 0;
static int g_LobbyBgHeight = 0;

// ---------------------------------------------------------------------------
// Main Lobby
// ---------------------------------------------------------------------------
// Font Declarations
static ImFont* g_LargeFont = nullptr;
static ImFont* g_MathFont = nullptr;

static void ShowMainLobby(AppScreen& screen) {
    // --- 0. DRAW BACKGROUND IMAGE (drawn first = appears behind everything else) ---
    if (g_LobbyBackgroundTexture != 0) {
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        ImVec2 p0 = ImGui::GetWindowPos();
        ImVec2 p1 = ImVec2(p0.x + ImGui::GetWindowSize().x, p0.y + ImGui::GetWindowSize().y);
        draw_list->AddImage((ImTextureID)(intptr_t)g_LobbyBackgroundTexture, p0, p1);
    }

    // --- 1. PUSH THE CUSTOM COLORS ---
    // Change these ImVec4(R, G, B, A) values to whatever theme colors you prefer
    ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.44f, 0.22f, 0.26f, 1.0f)); // Normal state
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.44f, 0.78f, 0.22f, 0.8f)); // Hover state
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(0.44f, 0.25f, 0.45f, 1.0f)); // Clicked state

    // 2. Keep your existing layout metrics
    float buttonWidth = 500.0f;
    float buttonHeight = 60.0f;
    float spacing = ImGui::GetStyle().ItemSpacing.y;

    float totalContentWidth = buttonWidth;
    float totalContentHeight = 100.0f + (spacing * 2) + buttonHeight + spacing + buttonHeight + spacing + buttonHeight;

    ImVec2 windowSize = ImGui::GetWindowSize();
    float startX = (windowSize.x - totalContentWidth) * 0.5f;
    float startY = (windowSize.y - totalContentHeight) * 0.4f;

    ImGui::SetCursorPos(ImVec2(startX, startY));

    // --- DRAW CONTENT ---
    ImGui::PushFont(g_LargeFont);
    float titleWidth = ImGui::CalcTextSize("Main Lobby").x;
    ImGui::SetCursorPosX(startX + (buttonWidth - titleWidth) * 2.0f);
    ImGui::Text("Linear Calculators");
    ImGui::PopFont();

    // Spacing blocks...
    ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();
    ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();
    ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();
    ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();

    // Draw Button 1
    ImGui::SetCursorPosX(startX);
    if (ImGui::Button("Graph Plotter", ImVec2(buttonWidth, buttonHeight))) {
        screen = AppScreen::GraphPlotter;
    }

    ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();

    // Draw Button 2
    ImGui::SetCursorPosX(startX);
    if (ImGui::Button("Formula Lineariser", ImVec2(buttonWidth, buttonHeight))) {
        screen = AppScreen::FormulaLineariser;
    }

    ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();

    // Draw Button 3
    ImGui::SetCursorPosX(startX);
    if (ImGui::Button("Credits", ImVec2(buttonWidth, buttonHeight))) {
        screen = AppScreen::Credits;
    }

    // --- 3. POP THE CUSTOM COLORS AT THE END ---
    ImGui::PopStyleColor(3); 
}

// --- Linearisers ---
struct LinearisationResult {
    bool success;
    std::string message;
    std::string formulaY;
    std::string formulaX;
};

static std::string removeSpaces(std::string str) {
    str.erase(std::remove(str.begin(), str.end(), ' '), str.end());
    return str;
}

static LinearisationResult runAlgorithmicLineariser(std::string lhs, std::string rhs, std::string indepVar, std::string depVar) {
    LinearisationResult res;
    res.success = false;

    lhs = removeSpaces(lhs);
    rhs = removeSpaces(rhs);

    if (lhs.empty() || rhs.empty()) {
        res.message = "Error: LHS and RHS cannot be empty.";
        return res;
    }
    if (lhs.find('=') != std::string::npos || rhs.find('=') != std::string::npos) {
        res.message = "Error: The '=' symbol is not allowed inside either side.";
        return res;
    }

    std::string fullEquation = lhs + "=" + rhs;
    if (fullEquation.find(indepVar) == std::string::npos || fullEquation.find(depVar) == std::string::npos) {
        res.message = "Error: Variable keys must explicitly exist inside the string inputs.";
        return res;
    }

    std::string currentLHS = lhs;
    std::string currentRHS = rhs;
    bool isolationComplete = false;

    for (int iter = 0; iter < 10 && !isolationComplete; ++iter) {
        if (currentRHS.find(depVar) != std::string::npos && currentLHS.find(depVar) == std::string::npos) {
            std::swap(currentLHS, currentRHS);
        }

        if (currentLHS == depVar) {
            isolationComplete = true;
            break;
        } 

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
                res.message = "Error: Target variable missing from expression.";
                return res;
            }
        }
        else if (currentLHS.find('/') != std::string::npos) {
            size_t slashPos = currentLHS.find('/');
            std::string numerator = currentLHS.substr(0, slashPos);
            std::string denominator = currentLHS.substr(slashPos + 1);

            if (numerator.find(depVar) != std::string::npos) {
                currentLHS = numerator;
                currentRHS = "(" + currentRHS + ") * " + denominator; 
            } else {
                res.message = "Error: Target trapped in denominator.";
                return res;
            }
        } 
        else if (currentLHS.length() > depVar.length() && currentLHS.find_first_of("+/-*") == std::string::npos) {
            size_t targetPos = currentLHS.find(depVar);
            if (targetPos != std::string::npos) {
                std::string otherVar = (targetPos == 0) ? currentLHS.substr(depVar.length()) : currentLHS.substr(0, targetPos);
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

    if (isolationComplete) {
        res.success = true;
        res.message = "Success: " + currentLHS + " = " + currentRHS;
    } else {
        res.message = "Error: Expression structure limits computational step limits.";
    }
    return res;
}

static std::vector<std::string> getFormulaTokens(std::string str) {
    std::vector<std::string> tokens;
    std::string cleaned = removeSpaces(str);
    std::string currentToken = "";

    for (size_t i = 0; i < cleaned.length(); ++i) {
        char c = cleaned[i];
        if (isalnum(c) || c == '_' || c == '.') {
            currentToken += c;
        } else {
            if (!currentToken.empty()) {
                if (currentToken != "pi") tokens.push_back(currentToken);
                currentToken = "";
            }
        }
    }
    if (!currentToken.empty() && currentToken != "pi") {
        tokens.push_back(currentToken);
    }

    for (std::string &t : tokens) {
        t.erase(std::remove(t.begin(), t.end(), '_'), t.end());
    }
    std::sort(tokens.begin(), tokens.end());
    return tokens;
}

static LinearisationResult processFormulaInput(std::string fullInputEquation, std::string lhs, std::string rhs, std::string indepVar, std::string depVar) {
    std::vector<std::string> userTokens = getFormulaTokens(fullInputEquation);

    std::vector<std::pair<std::string, int>> library = {
        {"y=a*e^(b*x)", 1}, {"y=ax^b", 2}, {"T=2*pi*sqrt(L/g)", 3},
        {"I=k/d^2", 4},     {"v_0=(V_max*[S])/(K_m+[S])", 5}, {"k=A*e^(-E_a/(R*T))", 6},
        {"y=x/(ax+b)", 7},  {"V=V_0*e^(-t/(R*C))", 8}, {"d=0.5*a*t^2", 9},
        {"F=k*x", 10},      {"E_k=0.5*m*v^2", 11}, {"F_c=(m*v^2)/r", 12},
        {"F=(G*m_1*m_2)/r^2", 13}, {"V=I*R", 14}, {"P=I^2*R", 15},
        {"X_L=2*pi*f*L", 16}, {"X_C=1/(2*pi*f*C)", 17},
        {"T=2*pi*sqrt(m/k)", 18}
    };

    int discoveredLibraryID = -1;
    for (auto const& item : library) {
        std::vector<std::string> templateTokens = getFormulaTokens(item.first);
        if (userTokens == templateTokens) {
            discoveredLibraryID = item.second;
            break;
        }
    }

    LinearisationResult res;
    res.success = true;

    if (discoveredLibraryID != -1) {
        res.message = "[System Router] Match found in preset library! Running Preset Laws #" + std::to_string(discoveredLibraryID) + "\n\n";
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
        res = runAlgorithmicLineariser(lhs, rhs, indepVar, depVar);
    }

    return res;
}

// ---------------------------------------------------------------------------
// Formula Lineariser (placeholder screen)
// ---------------------------------------------------------------------------

static void ShowFormulaLineariser(AppScreen& screen) {
    // Corrected to static character array buffers to prevent text box overflow/crashes
    static char lhsInput[1024] = "";
    static char rhsInput[1024] = "";
    static char indepVar[64] = "x";
    static char depVar[64] = "y";
    static std::string resultDisplayMessage = "";

    // Render texture backdrops
    if (g_LobbyBackgroundTexture != 0) {
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        ImVec2 p0 = ImGui::GetWindowPos();
        ImVec2 p1 = ImVec2(p0.x + ImGui::GetWindowSize().x, p0.y + ImGui::GetWindowSize().y);
        draw_list->AddImage((ImTextureID)(intptr_t)g_LobbyBackgroundTexture, p0, p1);
    }

    // Top Navigation Styling
    ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.44f, 0.22f, 0.26f, 1.0f)); 
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.44f, 0.78f, 0.22f, 0.8f)); 
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(0.44f, 0.25f, 0.45f, 1.0f)); 

    float btn2Width = 450.0f;  
    float btn2Height = 45.0f;  

    if (ImGui::Button("Back to Main Lobby", ImVec2(btn2Width, btn2Height))) {
        screen = AppScreen::MainLobby;
    }
    ImGui::PopStyleColor(3);

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    if (g_MathFont != nullptr) ImGui::PushFont(g_MathFont);

    float windowWidth = ImGui::GetWindowSize().x;

    // 1. Centered Instructional Subtitle Banner
    std::string hintStr = "Please use * for multiplication and / for division";
    float hintWidth = ImGui::CalcTextSize(hintStr.c_str()).x;
    ImGui::SetCursorPosX((windowWidth - hintWidth) * 0.5f);
    ImGui::Text("%s", hintStr.c_str());

    ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();

    // 2. Dynamic DUAL SIDE-BY-SIDE Black Panels Layout with Red Borders
    float boxHeight = 220.0f;
    float separatorWidth = ImGui::CalcTextSize(" = ").x;
    
    // Dynamically calculate individual box widths based on current window size to avoid overflow
    float totalAvailableWidth = ImGui::GetContentRegionAvail().x;
    float boxWidth = (totalAvailableWidth - separatorWidth - ImGui::GetStyle().ItemSpacing.x * 2) * 0.5f;
    
    // Cap maximum sizing so it remains centered and compact on massive high-res screens
    if (boxWidth > 450.0f) {
        boxWidth = 450.0f;
    }
    
    // Recalculate dynamic starting center alignment offset
    float startX = (windowWidth - (boxWidth * 2) - separatorWidth) * 0.5f;
    if (startX < ImGui::GetStyle().WindowPadding.x) {
        startX = ImGui::GetStyle().WindowPadding.x;
    }

    // Inject custom palette overrides to form red outline block boxes with deep dark interiors
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.55f, 0.05f, 0.10f, 1.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.0f);

    // Left Input Pane (LHS)
    ImGui::SetCursorPosX(startX);
    ImGui::InputTextMultiline("##LHSBox", lhsInput, IM_ARRAYSIZE(lhsInput), ImVec2(boxWidth, boxHeight));

    // Middle Equals Operator Character Vertical Alignment
    ImGui::SameLine();
    float currentY = ImGui::GetCursorPosY();
    ImGui::SetCursorPosY(currentY + (boxHeight * 0.45f));
    ImGui::Text(" = ");
    
    // Right Input Pane (RHS)
    ImGui::SameLine();
    ImGui::SetCursorPosY(currentY);
    ImGui::InputTextMultiline("##RHSBox", rhsInput, IM_ARRAYSIZE(rhsInput), ImVec2(boxWidth, boxHeight));

    ImGui::PopStyleVar();
    ImGui::PopStyleColor(2);

    // 3. Variables Definition Row Controls
    ImGui::Spacing(); ImGui::Spacing();
    ImGui::SetCursorPosX(startX);
    
    ImGui::Text("Independent: "); ImGui::SameLine();
    ImGui::SetNextItemWidth(80);
    ImGui::InputText("##IndepV", indepVar, IM_ARRAYSIZE(indepVar));

    ImGui::SameLine(); ImGui::Spacing(); ImGui::SameLine();
    ImGui::Text("Dependent: "); ImGui::SameLine();
    ImGui::SetNextItemWidth(80);
    ImGui::InputText("##DepV", depVar, IM_ARRAYSIZE(depVar));

    ImGui::SameLine(); ImGui::Spacing(); ImGui::SameLine();
    
    // Green Action Trigger Button
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.20f, 0.45f, 0.20f, 1.0f));
    if (ImGui::Button("Linearise Formula", ImVec2(160, 0))) {
        std::string fullEquation = std::string(lhsInput) + "=" + std::string(rhsInput);
        LinearisationResult res = processFormulaInput(fullEquation, lhsInput, rhsInput, indepVar, depVar);
        resultDisplayMessage = res.message;
    }
    ImGui::PopStyleColor();

    // 4. Output Summary Display Block Header
    ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();
    
    float titleOutputWidth = ImGui::CalcTextSize("Linearised Formula").x;
    ImGui::SetCursorPosX((windowWidth - titleOutputWidth) * 0.5f);
    ImGui::Text("Linearised Formula");

    ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

    // Render system string messages safely beneath dividers
    if (!resultDisplayMessage.empty()) {
        ImGui::SetCursorPosX(startX);
        ImGui::TextWrapped("%s", resultDisplayMessage.c_str());
    }

    if (g_MathFont != nullptr) ImGui::PopFont();
}

// ---------------------------------------------------------------------------
// Credits
// ---------------------------------------------------------------------------
static void ShowCredits(AppScreen& screen) {
    if (g_LobbyBackgroundTexture != 0) {
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        ImVec2 p0 = ImGui::GetWindowPos();
        ImVec2 p1 = ImVec2(p0.x + ImGui::GetWindowSize().x, p0.y + ImGui::GetWindowSize().y);
        draw_list->AddImage((ImTextureID)(intptr_t)g_LobbyBackgroundTexture, p0, p1);
    }

    // --- 1. PUSH THE CUSTOM COLORS ---
    // Customize your RGBA float colors here (Red, Green, Blue, Alpha)
    ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.44f, 0.22f, 0.26f, 1.0f)); // Normal state color
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.44f, 0.78f, 0.22f, 0.8f)); // Hover state color
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(0.44f, 0.25f, 0.45f, 1.0f)); // Clicked state color

    // --- 2. DEFINE BUTTON DIMENSIONS ---
    float btnWidth = 450.0f;  // Set width in pixels
    float btnHeight = 45.0f;  // Set height in pixels

    // --- 3. DRAW BUTTON ---
    if (ImGui::Button("Back to Main Lobby", ImVec2(btnWidth, btnHeight))) {
        screen = AppScreen::MainLobby;
    }

    // --- 4. POP THE COLORS IMMEDIATELY ---
    ImGui::PopStyleColor(3);

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();

    // Customize your credits text here
    ImGui::Text("Linear Calculators App");
    ImGui::Text("Version: 1.0.0");
    ImGui::Spacing();ImGui::Spacing();ImGui::Spacing();ImGui::Spacing();
    ImGui::Text("Developed by: Group 1\nKANG JUN XIN - Programmer\nETHAN HAU LINTIAN, MARCUS LEE WEI HENG - Assistant Programmers\nETHAN HAU LINTIAN, LIM XIN KIT - Product Life Cycle Developer\nBRYAN GOH WEI YI, BRYAN LUY ZHI KAI - Presentation Slides Developer");
    ImGui::Spacing();ImGui::Spacing();ImGui::Spacing();ImGui::Spacing();
    ImGui::Text("Language: C++, CMake");
    ImGui::Text("Built using: Dear ImGui, ImPlot, GLFW, and OpenGL3");
}

// ---------------------------------------------------------------------------
// Graph Plotter
// ---------------------------------------------------------------------------

static void ShowGraphPlotter(AppScreen& screen) {
    // Editable axis variable names (shown in the table header and used as axis labels).
    static char xLabel[64] = u8"x";
    static char yLabel[64] = u8"y";

    // Data points, edited directly via the table (no more click-to-add).
    static std::vector<DataPoint> points = { {0.0, 0.0}, {1.0, 1.0} };

    // --- 1. PUSH THE CUSTOM COLORS ---
    // Customize your RGBA float colors here (Red, Green, Blue, Alpha)
    ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.44f, 0.22f, 0.26f, 1.0f)); // Normal state color
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.44f, 0.78f, 0.22f, 0.8f)); // Hover state color
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(0.44f, 0.25f, 0.45f, 1.0f)); // Clicked state color

    // --- 2. DEFINE BUTTON DIMENSIONS ---
    float btn3Width = 450.0f;  // Set width in pixels
    float btn3Height = 45.0f;  // Set height in pixels

    if (ImGui::Button("Back to Main Lobby", ImVec2(btn3Width, btn3Height))) {
        screen = AppScreen::MainLobby;
    }
    ImGui::SameLine();
    if (ImGui::Button("Save Graph as Image", ImVec2(btn3Width, btn3Height))) {
        g_saveGraphRequested = true;
    }

    ImGui::Spacing();

    // Push the Math font at the very start of the function layout
    if (g_MathFont != nullptr) {
        ImGui::PushFont(g_MathFont);
    }

    // --- Least squares regression: y = m*x + b ---
    bool haveFit = false;
    double m = 0.0, b = 0.0;
    if (points.size() >= 2) {
        double n = (double)points.size();
        double sumX = 0, sumY = 0, sumXY = 0, sumXX = 0;
        for (const auto& p : points) {
            sumX += p.x;
            sumY += p.y;
            sumXY += p.x * p.y;
            sumXX += p.x * p.x;
        }
        double denom = (n * sumXX - sumX * sumX);
        if (denom != 0.0) {
            m = (n * sumXY - sumX * sumY) / denom;
            b = (sumY - m * sumX) / n;
            haveFit = true;
        }
    }

    if (haveFit) {
        ImGui::Text("Best fit: %s = %.4f * %s + %.4f", yLabel, m, xLabel, b);
    } else {
        ImGui::Text("Best fit: add at least 2 points");
    }

    ImGui::Spacing();

    // --- Left panel: editable two-column table ---
    ImGui::BeginChild("LeftPanel", ImVec2(280, -1), true);
    {
        if (ImGui::BeginTable("PointsTable", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
            // Header row: editable variable names.
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::SetNextItemWidth(-1);
            ImGui::InputText("##xLabel", xLabel, sizeof(xLabel));
            ImGui::TableSetColumnIndex(1);
            ImGui::SetNextItemWidth(-1);
            ImGui::InputText("##yLabel", yLabel, sizeof(yLabel));

            // Data rows.
            for (size_t i = 0; i < points.size(); i++) {
                ImGui::PushID((int)i);
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::SetNextItemWidth(-1);
                ImGui::InputDouble("##x", &points[i].x, 0.0, 0.0, "%.3f");
                ImGui::TableSetColumnIndex(1);
                ImGui::SetNextItemWidth(-1);
                ImGui::InputDouble("##y", &points[i].y, 0.0, 0.0, "%.3f");
                ImGui::PopID();
            }

            ImGui::EndTable();
        }

        float btn4Height = 45.0f;

        ImGui::Spacing();
        if (ImGui::Button("Add Point", ImVec2(-1, btn4Height))) {
            points.push_back({0.0, 0.0});
        }
        if (points.size() > 0) {
            if (ImGui::Button("Remove Last Point", ImVec2(-1, btn4Height))) {
                points.pop_back();
            }
        }
    }
    // --- 4. POP THE COLORS IMMEDIATELY ---
    ImGui::PopStyleColor(3);
    ImGui::EndChild();

    ImGui::SameLine();

    // --- Right panel: the plot ---
    ImGui::BeginChild("RightPanel", ImVec2(-1, -1));
    {
        if (ImPlot::BeginPlot("##scatter", ImVec2(-1, -1))) {
            ImPlot::SetupAxes(xLabel, yLabel);

            if (!points.empty()) {
                std::vector<double> xs, ys;
                xs.reserve(points.size());
                ys.reserve(points.size());
                for (const auto& p : points) {
                    xs.push_back(p.x);
                    ys.push_back(p.y);
                }
                ImPlot::PlotScatter("Points", xs.data(), ys.data(), (int)xs.size());

                if (haveFit) {
                    if (haveFit) {
                        ImPlotRect limits = ImPlot::GetPlotLimits();
                        double lineX[2] = { limits.X.Min, limits.X.Max };
                        double lineY[2] = { m * lineX[0] + b, m * lineX[1] + b };
                        ImPlot::PlotLine("Best Fit", lineX, lineY, 2);
                    }
                }
            }

            ImPlot::EndPlot();
        }
    }
    
    ImGui::EndChild();

    if (g_MathFont != nullptr) {
        ImGui::PopFont();
    }
}

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------

int main() {
    if (!glfwInit()) return 1;

    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    GLFWwindow* window = glfwCreateWindow(1100, 750, "Linear Calculators", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // vsync

    // ---------------------------------------------------------
    // --- SET WINDOW / TASKBAR ICON ---
    // ---------------------------------------------------------
    GLFWimage images[1]; 
    int channels;
    
    // Make sure "icon.png" is placed inside your project's working directory
    images[0].pixels = stbi_load("./font/nb.png", &images[0].width, &images[0].height, &channels, 4); 
    
    if (images[0].pixels != nullptr) {
        glfwSetWindowIcon(window, 1, images);
        stbi_image_free(images[0].pixels);
    }
    // ---------------------------------------------------------

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGui::StyleColorsDark();

        // --- RESTORED FONT ATLAS CONFIGURATION ---
    ImGuiIO& io = ImGui::GetIO();
    io.FontGlobalScale = 2.0f;

    // 1. Font 0: Loaded first = App-wide classic pixel font baseline
    io.Fonts->AddFontDefault();

    // 2. Prepare the glyph ranges for the Greek character set
    ImFontGlyphRangesBuilder builder;
    builder.AddRanges(io.Fonts->GetGlyphRangesDefault()); // Standard ASCII
    builder.AddRanges(io.Fonts->GetGlyphRangesGreek());   // Greek symbols (α, β, θ, Δ)
    
    static ImVector<ImWchar> ranges;
    ranges.clear();
    builder.BuildRanges(&ranges);

    // 3. Font 1: Custom Lobby Title Font
    ImFontConfig titleConfig;
    titleConfig.SizePixels = 100.0f;
    g_LargeFont = io.Fonts->AddFontFromFileTTF("./font/PowerRangers.ttf", 100.0f, &titleConfig);

    // 4. Font 2: Dedicated Mathematics Font (Now loaded safely as Font 2)
    ImFontConfig mathConfig;
    mathConfig.SizePixels = 12.0f; 
    g_MathFont = io.Fonts->AddFontFromFileTTF("./font/Arial.ttf", 12.0f, &mathConfig, ranges.Data);
    

    // Load the Main Lobby background image (put your image at ./images/lobby_bg.png,
    // or change the path here). If it fails to load, the lobby just has no background.
    LoadTextureFromFile("./font/bg.png", &g_LobbyBackgroundTexture, &g_LobbyBgWidth, &g_LobbyBgHeight);

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    AppScreen currentScreen = AppScreen::MainLobby;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
        ImGui::Begin("App", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar);

        switch (currentScreen) {
            case AppScreen::MainLobby:
                ShowMainLobby(currentScreen);
                break;
            case AppScreen::GraphPlotter:
                ShowGraphPlotter(currentScreen);
                break;
            case AppScreen::FormulaLineariser:
                ShowFormulaLineariser(currentScreen);
                break;
            case AppScreen::Credits: // <-- Add this case
                ShowCredits(currentScreen);
                break;
        }

        ImGui::End();

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Handle image save AFTER rendering so the captured frame is complete,
        // but BEFORE swapping buffers (glReadPixels reads the back buffer).
        if (g_saveGraphRequested) {
            SaveWindowScreenshotAsPng(window);
            g_saveGraphRequested = false;
        }

        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
