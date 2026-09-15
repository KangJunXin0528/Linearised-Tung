#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "implot.h"
#include <GLFW/glfw3.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

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
    FormulaLineariser
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
// Main Lobby
// ---------------------------------------------------------------------------
static ImFont* g_LargeFont = nullptr;

static void ShowMainLobby(AppScreen& screen) {
    // 1. Define the sizes of your UI elements so we can calculate the total block size
    float buttonWidth = 280.0f;
    float buttonHeight = 60.0f;
    float spacing = ImGui::GetStyle().ItemSpacing.y;

    // Estimate total content height: Title text (~38px) + Spacing + Button 1 + Spacing + Button 2
    float totalContentWidth = buttonWidth;
    float totalContentHeight = 38.0f + (spacing * 2) + buttonHeight + spacing + buttonHeight;

    // 2. Get the current available window dimensions
    ImVec2 windowSize = ImGui::GetWindowSize();

    // 3. Calculate the starting X and Y coordinates to hit dead-center
    float startX = (windowSize.x - totalContentWidth) * 0.5f;
    float startY = (windowSize.y - totalContentHeight) * 0.5f;

    // 4. Move the cursor to the calculated center starting position
    ImGui::SetCursorPos(ImVec2(startX, startY));

    // --- DRAW CONTENT ---
    
    // Draw the Title (Centered relative to the button width)
    ImGui::PushFont(g_LargeFont);
    float titleWidth = ImGui::CalcTextSize("Main Lobby").x;
    ImGui::SetCursorPosX(startX + (buttonWidth - titleWidth) * 0.5f); // Center text over buttons
    ImGui::Text("Main Lobby");
    ImGui::PopFont();

    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();

    // Draw Button 1
    ImGui::SetCursorPosX(startX);
    if (ImGui::Button("Graph Plotter", ImVec2(buttonWidth, buttonHeight))) {
        screen = AppScreen::GraphPlotter;
    }

    ImGui::Spacing();

    // Draw Button 2
    ImGui::SetCursorPosX(startX);
    if (ImGui::Button("Formula Lineariser", ImVec2(buttonWidth, buttonHeight))) {
        screen = AppScreen::FormulaLineariser;
    }
}

// ---------------------------------------------------------------------------
// Formula Lineariser (placeholder screen)
// ---------------------------------------------------------------------------

static void ShowFormulaLineariser(AppScreen& screen) {
    if (ImGui::Button("Back to Main Lobby")) {
        screen = AppScreen::MainLobby;
    }

    ImGui::Spacing();
    ImGui::Text("Formula Lineariser - coming soon.");
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

    if (ImGui::Button("Back to Main Lobby")) {
        screen = AppScreen::MainLobby;
    }
    ImGui::SameLine();
    if (ImGui::Button("Save Graph as Image")) {
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

        ImGui::Spacing();
        if (ImGui::Button("Add Point", ImVec2(-1, 0))) {
            points.push_back({0.0, 0.0});
        }
        if (points.size() > 0) {
            if (ImGui::Button("Remove Last Point", ImVec2(-1, 0))) {
                points.pop_back();
            }
        }
    }
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

    GLFWwindow* window = glfwCreateWindow(1100, 750, "Linear Fit Plotter", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // vsync

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
    config.SizePixels = 32.0f;
    g_LargeFont = io.Fonts->AddFontDefault(&config);


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
