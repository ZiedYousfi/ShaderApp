#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "include/stb_image_write.h"

/*
 * Copyright (c) 2025 Yousfi Zied
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Global log file pointer for logging messages
FILE* g_logFile = NULL;

/**
 * Logs a formatted message both to the console and to the log file.
 *
 * @param fmt Format string (printf-style).
 * @param ... Additional arguments for the format string.
 */
void log_and_print(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);  // Print to console
    va_end(args);

    if (g_logFile) {
        va_start(args, fmt);
        vfprintf(g_logFile, fmt, args);  // Write to log file
        va_end(args);
    }
}

/**
 * Loads the entire shader source from a file into a dynamically allocated char buffer.
 *
 * @param filePath Path to the shader file.
 * @return Pointer to the buffer containing the shader code (caller must free).
 *         Returns NULL if file cannot be read or if allocation fails.
 */
char* loadShaderSource(const char* filePath) {
    FILE* fp = fopen(filePath, "r");
    if (!fp) {
        log_and_print("Error: Unable to open shader file '%s'\n", filePath);
        return NULL;
    }
    log_and_print("Loading shader from '%s'...\n", filePath);

    fseek(fp, 0, SEEK_END);
    long length = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char* buffer = (char*)malloc(length + 1);
    if (!buffer) {
        log_and_print("Error: Not enough memory to load shader '%s'\n", filePath);
        fclose(fp);
        return NULL;
    }

    size_t bytesRead = fread(buffer, 1, length, fp);
    if (bytesRead != (size_t)length) {
        log_and_print("Error: Incomplete read of shader file '%s'\n", filePath);
        free(buffer);
        fclose(fp);
        return NULL;
    }
    buffer[length] = '\0';
    fclose(fp);

    log_and_print("Shader '%s' loaded successfully.\n", filePath);
    return buffer;
}

/**
 * Compiles a given shader source (vertex or fragment).
 *
 * @param type OpenGL shader type (e.g., GL_VERTEX_SHADER or GL_FRAGMENT_SHADER).
 * @param source The actual shader source code as a C string.
 * @return The compiled shader object, or 0 if compilation failed.
 */
unsigned int compileShader(int type, const char* source) {
    if (!source) {
        log_and_print("Error: Shader source is NULL\n");
        return 0;
    }

    const char* shaderType = (type == GL_VERTEX_SHADER)
                                 ? "vertex"
                                 : (type == GL_FRAGMENT_SHADER)
                                       ? "fragment"
                                       : "unknown";
    log_and_print("Compiling %s shader...\n", shaderType);

    unsigned int shader = glCreateShader(type);
    if (shader == 0) {
        log_and_print("Error: Failed to create %s shader\n", shaderType);
        return 0;
    }

    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    int success;
    char logBuffer[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, logBuffer);
        log_and_print("Error compiling %s shader:\n%s\n", shaderType, logBuffer);
        glDeleteShader(shader);
        return 0;
    }

    log_and_print("%s shader compiled successfully.\n", shaderType);
    return shader;
}

/**
 * Links a vertex shader and a fragment shader into a single shader program.
 *
 * @param vertexShader Compiled vertex shader.
 * @param fragmentShader Compiled fragment shader.
 * @return The linked shader program ID, or 0 if linking failed.
 */
unsigned int createShaderProgram(unsigned int vertexShader, unsigned int fragmentShader) {
    if (vertexShader == 0 || fragmentShader == 0) {
        log_and_print("Error: Invalid shader(s) provided for program creation\n");
        return 0;
    }

    log_and_print("Creating shader program...\n");
    unsigned int program = glCreateProgram();
    if (program == 0) {
        log_and_print("Error: Failed to create shader program\n");
        return 0;
    }

    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    int success;
    char logBuffer[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, NULL, logBuffer);
        log_and_print("Error linking shader program:\n%s\n", logBuffer);
        glDeleteProgram(program);
        return 0;
    }

    // Shaders can be deleted once they are linked
    glDetachShader(program, vertexShader);
    glDetachShader(program, fragmentShader);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    log_and_print("Shader program created and linked successfully.\n");
    return program;
}

/**
 * Callback used by GLFW to adjust the OpenGL viewport when the window is resized.
 */
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
#ifdef __APPLE__
    // On macOS with Retina displays, the viewport typically needs a 2x scaling.
    glViewport(0, 0, width * 2, height * 2);
#else
    glViewport(0, 0, width, height);
#endif
    log_and_print("Window resized: width = %d, height = %d\n", width, height);
}

/**
 * Reads the current framebuffer (RGBA) and saves it as a PNG file using stb_image_write.
 *
 * @param filename Output filename for the PNG (e.g., "frames/frame_00000.png").
 * @param width    Current framebuffer width.
 * @param height   Current framebuffer height.
 */
void captureFrame(const char* filename, int width, int height) {
    unsigned char* pixels = (unsigned char*)malloc(width * height * 4);
    if (!pixels) {
        log_and_print("Error: Unable to allocate memory for pixel data.\n");
        return;
    }
    glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    // Flip the image vertically because OpenGL's origin is at the lower left.
    unsigned char* flipped = (unsigned char*)malloc(width * height * 4);
    if (!flipped) {
        log_and_print("Error: Unable to allocate memory for flipped data.\n");
        free(pixels);
        return;
    }
    for (int y = 0; y < height; y++) {
        memcpy(flipped + (height - 1 - y) * width * 4,
               pixels + y * width * 4,
               width * 4);
    }

    if (!stbi_write_png(filename, width, height, 4, flipped, width * 4)) {
        log_and_print("Error: Failed to write PNG file: %s\n", filename);
    } else {
        log_and_print("Saved frame to: %s\n", filename);
    }

    free(flipped);
    free(pixels);
}

int main(int argc, char** argv) {
    // Default parameters
    int         windowWidth        = 2560;
    int         windowHeight       = 1440;
    const char* windowTitle        = "My First Shader!";
    const char* vertexShaderPath   = "shaders/vertex_shader.glsl";
    const char* fragmentShaderPath = "shaders/fragment_shader.glsl";

    // Parameters for optional video recording
    bool        recordVideo       = false;
    int         fps               = 30;
    float       duration          = 5.0f;
    char        outputFolder[256] = "frames";
    char        outputVideo[256]  = "output.mp4";

    // Open log file
    g_logFile = fopen("shaderapp_logs.log", "w");
    if (!g_logFile) {
        printf("Error: Unable to open log file.\n");
        return -1;
    }
    log_and_print("----- Program Start -----\n");

    // Parsing command-line arguments
    // Example:
    //   ./app 1024 768 "Window Title" vertex.glsl fragment.glsl --video 1 30 5 frames out.mp4
    if (argc >= 3) {
        windowWidth  = atoi(argv[1]);
        windowHeight = atoi(argv[2]);
        if (argc >= 4) {
            windowTitle = argv[3];
        }
        if (argc >= 5) {
            vertexShaderPath = argv[4];
        }
        if (argc >= 6) {
            fragmentShaderPath = argv[5];
        }
        // Look for --video flag
        for (int i = 6; i < argc; i++) {
            if (strcmp(argv[i], "--video") == 0) {
                // Expecting 5 more arguments (bool, fps, duration, folder, filename)
                if (i + 5 < argc) {
                    recordVideo = (atoi(argv[i + 1]) != 0);
                    fps         = atoi(argv[i + 2]);
                    duration    = (float)atof(argv[i + 3]);
                    strncpy(outputFolder, argv[i + 4], sizeof(outputFolder) - 1);
                    strncpy(outputVideo, argv[i + 5], sizeof(outputVideo) - 1);
                } else {
                    log_and_print("Warning: --video flag provided but not enough parameters.\n");
                }
                break;
            }
        }
        log_and_print("Command line parameters received.\n");
    } else {
        // If insufficient arguments, ask interactively
        log_and_print("No command line parameters detected, launching interactive menu.\n");
        printf("Welcome to ShaderApp!\n");
        printf("1. Use default parameters\n");
        printf("2. Customize parameters\n");
        printf("Enter your choice (1 or 2): ");
        int choice = 0;
        if (scanf("%d", &choice) != 1) {
            log_and_print("Error reading choice. Using default parameters.\n");
            choice = 1;
        }
        if (choice == 2) {
            printf("Enter window width: ");
            scanf("%d", &windowWidth);
            printf("Enter window height: ");
            scanf("%d", &windowHeight);

            static char titleBuffer[256];
            printf("Enter window title: ");
            scanf(" %[^\n]", titleBuffer);
            windowTitle = titleBuffer;

            static char vertexBuffer[256];
            printf("Enter vertex shader path: ");
            scanf(" %[^\n]", vertexBuffer);
            vertexShaderPath = vertexBuffer;

            static char fragmentBuffer[256];
            printf("Enter fragment shader path: ");
            scanf(" %[^\n]", fragmentBuffer);
            fragmentShaderPath = fragmentBuffer;

            // Video recording options
            printf("Record video? (0/1): ");
            int videoChoice = 0;
            scanf("%d", &videoChoice);
            recordVideo = (videoChoice != 0);
            if (recordVideo) {
                printf("Enter FPS: ");
                scanf("%d", &fps);
                printf("Enter duration (seconds): ");
                scanf("%f", &duration);

                printf("Output frames folder: ");
                scanf(" %[^\n]", outputFolder);

                printf("Output video file: ");
                scanf(" %[^\n]", outputVideo);
            }
            log_and_print("User provided custom parameters.\n");
        } else {
            log_and_print("Using default parameters.\n");
        }
    }

    // Log final configuration
    log_and_print("Configuration:\n");
    log_and_print("  Window Size   : %d x %d\n", windowWidth, windowHeight);
    log_and_print("  Title         : %s\n", windowTitle);
    log_and_print("  Vertex Shader : %s\n", vertexShaderPath);
    log_and_print("  Fragment Shdr : %s\n", fragmentShaderPath);
    if (recordVideo) {
        log_and_print("  Video Capture : YES\n");
        log_and_print("    FPS         : %d\n", fps);
        log_and_print("    Duration    : %.2f sec\n", duration);
        log_and_print("    Frames Dir  : %s\n", outputFolder);
        log_and_print("    Output Video: %s\n", outputVideo);
    } else {
        log_and_print("  Video Capture : NO\n");
    }

    // Initialize GLFW
    if (!glfwInit()) {
        log_and_print("Error initializing GLFW.\n");
        fclose(g_logFile);
        return -1;
    }
    log_and_print("GLFW initialized successfully.\n");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, windowTitle, NULL, NULL);
    if (!window) {
        log_and_print("Error creating the window.\n");
        glfwTerminate();
        fclose(g_logFile);
        return -1;
    }
    log_and_print("Window created successfully.\n");

    glfwMakeContextCurrent(window);

    // Load OpenGL function pointers via GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        log_and_print("Error loading GLAD.\n");
        glfwTerminate();
        fclose(g_logFile);
        return -1;
    }
    log_and_print("GLAD loaded successfully.\n");

    // Adjust viewport based on actual framebuffer size
    int fbWidth, fbHeight;
    glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
    glViewport(0, 0, fbWidth, fbHeight);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Load and compile shaders
    char* vertexSource   = loadShaderSource(vertexShaderPath);
    char* fragmentSource = loadShaderSource(fragmentShaderPath);
    if (!vertexSource || !fragmentSource) {
        log_and_print("Error: Failed to load shader sources.\n");
        glfwTerminate();
        fclose(g_logFile);
        return 1;
    }

    unsigned int vertexShader   = compileShader(GL_VERTEX_SHADER, vertexSource);
    unsigned int fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSource);
    free(vertexSource);
    free(fragmentSource);

    if (vertexShader == 0 || fragmentShader == 0) {
        log_and_print("Error: Shader compilation failed.\n");
        glfwTerminate();
        fclose(g_logFile);
        return 1;
    }

    unsigned int shaderProgram = createShaderProgram(vertexShader, fragmentShader);
    if (shaderProgram == 0) {
        log_and_print("Error: Shader program linking failed.\n");
        glfwTerminate();
        fclose(g_logFile);
        return 1;
    }

    // A large triangle to cover the entire screen
    float vertices[] = {
        -1.0f, -1.0f,  // bottom-left
         3.0f, -1.0f,  // bottom-right
        -1.0f,  3.0f   // top-left
    };

    // Create VAO/VBO
    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // If we want to record video, create the output folder if it doesn't exist
    if (recordVideo) {
#ifdef _WIN32
        char mkdirCmd[512];
        snprintf(mkdirCmd, 512, "mkdir %s 2> NUL", outputFolder);
        system(mkdirCmd);
#else
        char mkdirCmd[512];
        snprintf(mkdirCmd, 512, "mkdir -p %s", outputFolder);
        system(mkdirCmd);
#endif
    }

    log_and_print("Starting render loop.\n");

    int frameCount   = 0;
    int totalFrames  = recordVideo ? (int)(fps * duration) : -1;

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // Capture frames if recording
        if (recordVideo) {
            char frameFile[512];
            snprintf(frameFile, 512, "%s/frame_%05d.png", outputFolder, frameCount);
            captureFrame(frameFile, fbWidth, fbHeight);

            frameCount++;
            if (frameCount >= totalFrames) {
                break;
            }
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    log_and_print("Exiting render loop.\n");

    glfwDestroyWindow(window);
    glfwTerminate();
    log_and_print("OpenGL resources released; GLFW terminated.\n");

    // If recording was enabled, assemble the frames into a video
    if (recordVideo) {
        log_and_print("Combining frames into video using ffmpeg...\n");
        char ffmpegCmd[1024];
        // Compute the bitrate: fps * 2 (in Mbps)
        int bitrate_mbps = fps * 2;

        snprintf(
            ffmpegCmd,
            sizeof(ffmpegCmd),
            "ffmpeg -y -framerate %d -i \"%s/frame_%%05d.png\" "
            "-c:v libx264 -preset veryslow -qp 0 -pix_fmt yuv444p -g 1 -b:v %dM \"%s\"",
            fps, outputFolder, bitrate_mbps, outputVideo
        );

        int ret = system(ffmpegCmd);
        if (ret != 0) {
            log_and_print("Error: ffmpeg command failed.\n");
        } else {
            log_and_print("Video created successfully: %s\n", outputVideo);
            log_and_print("Removing temporary frame images...\n");
#ifdef _WIN32
            char rmCmd[512];
            snprintf(rmCmd, 512, "del /Q %s\\frame_*.png", outputFolder);
#else
            char rmCmd[512];
            snprintf(rmCmd, 512, "rm -f %s/frame_*.png", outputFolder);
#endif
            system(rmCmd);
        }
    }

    log_and_print("----- Program End -----\n");
    fclose(g_logFile);
    return 0;
}
