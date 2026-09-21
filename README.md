# Linear Fit Plotter

A graphical calculator application built in C++ that calculates linear regression lines (y = m*x + b) using the method of least squares and plots data points on a scatter graph.


## How To Install

If you don't care about the source code,

1. find "Releases" at the repository (normally at the right) and click it. (Here is the link if you can't find it: https://github.com/KangJunXin0528/Linearised-Tung/releases)

2. Find "Assets", click "calculator-release.zip"

3. Once your file finish downloading, find the location you saved it and extract it to your desired location

4. Go into the folder you extracted, double click "calculator.exe" to open it

5. It might trigger your Windows Protector; click "Run Anyways" because there is no virus (trust me bro)

6. Then you would open my program successfully

But if you want to see the source code, and run it in the hard way, then clone my repository, and follow these steps:

## Prerequisites

To build and run this project locally, you will need to download and include the following dependencies in your project directory:
*   **Dear ImGui** (Core library files)
*   **ImPlot** (Plotting extension library)
*   **stb_image_write.h** (Image exporting utility)

## File Structure

```text
├── build/                 # Compiler output directory
├── graphs/                # Saved graph screenshot targets
├── imgui/                 # Place third-party ImGui library files here
├── CMakeLists.txt         # Core project layout configurations
└── main.cpp               # Application engine source code
```

## System Setup (Windows)

Before compiling, ensure you have Git, CMake, and a C++ compiler installed on your system. You can install all of them quickly using Windows Package Manager (`winget`) in your terminal.

### 1. Install Git
Run this command to install Git:
```bash
winget install -e --id Git.Git
```
To check if Git installed properly, restart your terminal and run:
```bash
git --version
```

### 2. Install the C++ Compiler (MinGW/GCC)
Run this command to install the compiler tools (g++):
```bash
winget install -e --id BrechtSanders.WinLibs.POSIX.UCRT
```
To check if the compiler installed properly, run:
```bash
g++ --version
```

### 3. Install CMake
Run this command to install CMake:
```bash
winget install -e --id Kitware.CMake
```
To check if CMake installed properly, run:
```bash
cmake --version
```

## How to Compile and Run

1. Download or clone this repository.
2. Ensure the required library dependencies are placed inside your `imgui/` subdirectory.
3. Open your terminal or compiler terminal and change the directory (`cd`) to the folder address where it was cloned/downloaded:
   ```bash
   cd "C:\(\path\to\your\downloaded\folder\\)"
   ```
4. Configure and build the project using CMake:
   ```bash
   cmake -S . -B build
   cmake --build build
   ```
5. To run the program, execute the generated application from the terminal:
   ```bash
   .\build\calculator.exe
   ```
