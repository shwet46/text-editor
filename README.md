# Text Editor

A simple C++ text editor with a custom rendering engine using SFML and a modular architecture. The project is designed for extensibility and learning purposes, featuring its own document, view, and input handling logic.

## Features
- Custom text rendering using SFML
- Line and character navigation
- Multiple selection and cursor support
- File open/save functionality
- Modular codebase (separate classes for document, view, content, input, etc.)
- Syntax highlighting and special character support (via `SpecialChars.h`)
- Scroll, zoom, and rotate the view

## Project Structure

```
src/
│
├── TextDocument.* # Manages file I/O and text buffer
├── EditorContent.* # Handles cursor logic, selections, editing
├── EditorView.* # Handles rendering and camera/view manipulation
├── InputController.* # Processes keyboard/mouse input
│
├── Cursor.* # Cursor structure and logic
├── SelectionData.* # Multi-selection management
├── SpecialChars.* # Syntax highlighting and character utilities
├── ImplementationUtils.* # Utility functions
│
├── MainWindow.* # (Optional) Qt-based main application window
└── main.cpp # Application entry point
```

## Build Instructions

### Prerequisites
- C++17 or newer
- [SFML 2.x](https://www.sfml-dev.org/)
- (Optional) [Qt 5/6](https://www.qt.io/) for GUI integration
- CMake or your preferred build system

### Building with CMake
1. Clone the repository:
   ```sh
   git clone https://github.com/yourusername/text-editor.git
   cd text-editor
   ```
2. Create a build directory and run CMake:
   ```sh
   mkdir build
   cd build
   cmake .. -G "Visual Studio 16 2019" # or your generator
   cmake --build .
   ```
3. Run the executable from the build output.

### Building Manually
- Make sure to link against SFML libraries (and Qt if using the Qt UI).
- Example (SFML only):
  ```sh
  g++ -std=c++17 src/*.cpp -o texteditor -lsfml-graphics -lsfml-window -lsfml-system
  ```

## Usage
- Launch the application.
- Use keyboard and mouse to navigate, edit, and save files.
- Scroll, zoom, and rotate the view as needed.
