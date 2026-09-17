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
static ImFont* g_LargeFont = nullptr;

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

// ---------------------------------------------------------------------------
// Formula Lineariser (placeholder screen)
// ---------------------------------------------------------------------------

static void ShowFormulaLineariser(AppScreen& screen) {
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
    float btn2Width = 450.0f;  // Set width in pixels
    float btn2Height = 45.0f;  // Set height in pixels

    // --- 3. DRAW BUTTON ---
    if (ImGui::Button("Back to Main Lobby", ImVec2(btn2Width, btn2Height))) {
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

    ImGui::Text("Formula Lineariser - coming soon.");
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
    static char xLabel[64] = "x";
    static char yLabel[64] = "y";

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

    // Configure crisp custom fonts
    ImGuiIO& io = ImGui::GetIO();
    io.FontGlobalScale = 2.0f;

    // Load default font sizes
    io.Fonts->AddFontDefault();
    ImFontConfig config;
    config.SizePixels = 100.0f;
    g_LargeFont = io.Fonts->AddFontFromFileTTF("./font/PowerRangers.ttf", 100.0f, &config);

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
