# OpenGL Shader Viewer

A lightweight OpenGL application for rendering custom shaders with support for both command-line and interactive parameter configuration.  It also now includes optional video recording.

## 🚀 Quick Start

### Prerequisites

- OpenGL 4.1+
- GLFW3
- GLAD
- FFmpeg (Optional, for video recording)

### Compilation

#### Linux/macOS
```bash
clang main.c glad.c -o shaderapp \
    -Iinclude -Iinclude/KHR \
    -I/opt/homebrew/Cellar/glfw/3.4/include \
    -L/opt/homebrew/Cellar/glfw/3.4/lib \
    -lglfw -framework OpenGL
```

#### Windows (MinGW)
```bash
gcc main.c glad.c -o shaderapp -Iinclude -Llib -lglfw3 -lopengl32 -lgdi32 -luser32 -lshell32 -lkernel32 -lwinmm -ladvapi32
```

Make sure GLAD and GLFW header files are in your include path.  If you plan to use the video recording functionality, ensure FFmpeg is installed and accessible in your system's PATH.

## 🎮 Usage

### Command Line Interface

```bash
./shaderapp [width] [height] [window_title] [vertex_shader_path] [fragment_shader_path] [--video <record> <fps> <duration> <folder> <filename>]
```

**Arguments:**

*   `width`: Window width in pixels.
*   `height`: Window height in pixels.
*   `window_title`: Title of the application window (in quotes if it contains spaces).
*   `vertex_shader_path`: Path to the vertex shader file.
*   `fragment_shader_path`: Path to the fragment shader file.
*   `--video`:  (Optional) Enables video recording.  Followed by:
    *   `<record>`:  0 or 1.  1 enables video recording, 0 disables.
    *   `<fps>`:  Frames per second for the video.
    *   `<duration>`:  Duration of the video in seconds.
    *   `<folder>`:  Output folder for the frame images.
    *   `<filename>`:  Output filename for the video (e.g., `output.mp4`).

**Examples:**

*   Basic usage:
    ```bash
    ./shaderapp 1920 1080 "Custom Shader" shaders/vertex.glsl shaders/fragment.glsl
    ```

*   Enabling video recording:
    ```bash
    ./shaderapp 1280 720 "My Animated Shader" shaders/vertex.glsl shaders/fragment.glsl --video 1 30 10 frames output.mp4
    ```

### Interactive Mode

When launched without arguments, the program provides an interactive menu to:

1.  Use default settings.
2.  Customize all parameters.

## ⚙️ Default Configuration

```
Window Size    : 2560 x 1440
Title          : "My First Shader!"
Vertex Path    : "shaders/vertex_shader.glsl"
Fragment Path  : "shaders/fragment_shader.glsl"
Video Capture  : NO
```

## 📁 Project Structure

```
.
├── main.c                         # Main source code
├── glad.c                         # GLAD source code
├── include/                      # Include directory
│   ├── KHR/                      # Khronos extensions
│   └── ...                      # GLAD and other headers
├── shaders/
│   ├── vertex_shader.glsl        # Default vertex shader
│   └── fragment_shader.glsl      # Default fragment shader
├── shaderapp_logs.log            # Generated log file
└── README.md
```

## 🎨 Shader Requirements

### Vertex Shader

```glsl
#version 410 core
layout (location = 0) in vec2 position;
// Your vertex shader code... Don't change it unless you know what you're doing
```

### Fragment Shader

```glsl
#version 410 core
out vec4 FragColor;
// Your fragment shader code... Here goes your artistry
```

## 📝 Logging System

The program maintains a detailed `shaderapp_logs.log` file that tracks:

*   Configuration parameters.
*   Shader compilation status.
*   Program initialization steps.
*   Runtime events.
*   Error messages.
*   Video recording status and FFmpeg commands.

Example log output:

```
----- Program Start -----
Configuration:
  Dimensions    : 2560 x 1440
  Title         : My First Shader
  Vertex Shader : shaders/vertex_shader.glsl
  Fragment Shader : shaders/fragment_shader.glsl
  Video Capture : NO
GLFW initialized successfully.
...
```

## 🛠️ Features

*   **Flexible Window Configuration**: Custom size and title.
*   **Shader Hot-loading**: Load custom shaders at runtime.
*   **Interactive UI**: User-friendly parameter configuration.
*   **Robust Error Handling**: Comprehensive error checking and reporting.
*   **Cross-platform Support**: Works on Windows, Linux, and macOS.
*   **Retina Display Support**: Automatic scaling for high-DPI displays.
*   **Video Recording**: Capture shader output to video files (requires FFmpeg).

## 🔍 Error Handling

The program includes extensive error checking for:

*   GLFW initialization.
*   Window creation.
*   GLAD loading.
*   Shader compilation and loading.
*   Program linking.
*   Resource allocation/deallocation.
*   FFmpeg execution for video creation.

## 🔄 Resource Management

Automatic cleanup of:

*   OpenGL buffers and vertex arrays.
*   Shader programs.
*   GLFW windows.
*   File handles.
*   Dynamic memory allocations.

## 🐛 Debugging

For debugging, the program provides:

*   Detailed console output.
*   Comprehensive log file.
*   Error messages with specific failure points.
*   Shader compilation error details.
*   Output of FFmpeg commands.

## ⚡ Performance

The program uses:

*   Single fullscreen triangle for efficient rendering.
*   Minimal state changes.
*   Proper resource cleanup.
*   Optimized viewport handling.

## 📚 API Reference

### Key Functions

```c
void log_and_print(const char* fmt, ...);
char* loadShaderSource(const char* filePath);
unsigned int compileShader(int type, const char* source);
unsigned int createShaderProgram(unsigned int vertexShader, unsigned int fragmentShader);
void captureFrame(const char* filename, int width, int height);
```

## 🤝 Contributing

Contributions are welcome! Areas for improvement:

*   Additional shader features.
*   Enhanced error reporting.
*   Performance optimizations.
*   UI improvements.
*   Documentation updates.
*   More sophisticated video encoding options.

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🆘 Support

For issues, questions, or contributions:

1.  Check existing documentation.
2.  Review the log file.
3.  Create a detailed issue report.

---

**Notes:**

*   Always ensure your shader files exist in the correct location before running the program.
*   FFmpeg must be installed and accessible in your system's PATH for video recording to function.
