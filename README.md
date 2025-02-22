# OpenGL Shader Viewer

A lightweight OpenGL application for rendering custom shaders with support for both command-line and interactive parameter configuration.

## 🚀 Quick Start

### Prerequisites

- OpenGL 4.1+
- GLFW3
- GLAD

### Compilation

#### Linux/macOS
```bash
gcc main.c -o shader_viewer -lglfw -lGL -ldl -lm -framework OpenGL
```

#### Windows (MinGW)
```bash
gcc main.c -o shader_viewer.exe -lglfw3 -lopengl32 -lgdi32 -framework OpenGL
```

Make sure GLAD and GLFW header files are in your include path.

## 🎮 Usage

### Command Line Interface
```bash
./shader_viewer [width] [height] [window_title] [vertex_shader_path] [fragment_shader_path]
```

Example:
```bash
./shader_viewer 1920 1080 "Custom Shader" shaders/vertex.glsl shaders/fragment.glsl
```

### Interactive Mode
When launched without arguments, the program provides an interactive menu to:
1. Use default settings
2. Customize all parameters

## ⚙️ Default Configuration

```
Window Size : 2560x1440
Title       : "My First Shader!"
Vertex Path : "shaders/vertex_shader.glsl"
Fragment Path: "shaders/fragment_shader.glsl"
```

## 📁 Project Structure

```
.
├── main.c                         # Main source code
├── shaders/
│   ├── vertex_shader.glsl        # Default vertex shader
│   └── fragment_shader.glsl      # Default fragment shader
└── program.log                   # Generated log file
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

The program maintains a detailed `program.log` file that tracks:

- Configuration parameters
- Shader compilation status
- Program initialization steps
- Runtime events
- Error messages

Example log output:
```
----- Program Start -----
Configuration:
  Dimensions: 2560 x 1440
  Title     : My First Shader
  Vertex Shader : shaders/vertex_shader.glsl
  Fragment Shader : shaders/fragment_shader.glsl
GLFW initialized successfully.
...
```

## 🛠️ Features

- **Flexible Window Configuration**: Custom size and title
- **Shader Hot-loading**: Load custom shaders at runtime
- **Interactive UI**: User-friendly parameter configuration
- **Robust Error Handling**: Comprehensive error checking and reporting
- **Cross-platform Support**: Works on Windows, Linux, and macOS
- **Retina Display Support**: Automatic scaling for high-DPI displays

## 🔍 Error Handling

The program includes extensive error checking for:

- GLFW initialization
- Window creation
- GLAD loading
- Shader compilation and loading
- Program linking
- Resource allocation/deallocation

## 🔄 Resource Management

Automatic cleanup of:
- OpenGL buffers and vertex arrays
- Shader programs
- GLFW windows
- File handles
- Dynamic memory allocations

## 🐛 Debugging

For debugging, the program provides:
- Detailed console output
- Comprehensive log file
- Error messages with specific failure points
- Shader compilation error details

## ⚡ Performance

The program uses:
- Single fullscreen triangle for efficient rendering
- Minimal state changes
- Proper resource cleanup
- Optimized viewport handling

## 📚 API Reference

### Key Functions

```c
void log_and_print(const char* fmt, ...);
char* loadShaderSource(const char* filePath);
unsigned int compileShader(int type, const char* source);
unsigned int createShaderProgram(unsigned int vertexShader, unsigned int fragmentShader);
```

## 🤝 Contributing

Contributions are welcome! Areas for improvement:
- Additional shader features
- Enhanced error reporting
- Performance optimizations
- UI improvements
- Documentation updates

## 📄 License

[Add your license here]

## 🆘 Support

For issues, questions, or contributions:
1. Check existing documentation
2. Review the log file
3. Create a detailed issue report

---

**Note**: Always ensure your shader files exist in the correct location before running the program.
