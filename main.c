#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Global log file pointer used for logging messages
FILE* g_logFile = NULL;

/**
 * Log and print a formatted message to both the console and the log file.
 *
 * @param fmt The format string.
 * @param ... Variable arguments to be formatted.
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
 * Load the shader source code from a file.
 *
 * @param filePath The path to the shader file.
 * @return A dynamically allocated string containing the shader source code.
 *         The caller is responsible for freeing the allocated memory.
 *         Returns NULL if the file cannot be read.
 */
char* loadShaderSource(const char* filePath) {
    FILE* fp = fopen(filePath, "r");
    if (!fp) {
        log_and_print("Error: Unable to open shader file '%s'\n", filePath);
        return NULL;
    }
    log_and_print("Loading shader from '%s'...\n", filePath);

    // Move to the end of the file to determine its length
    fseek(fp, 0, SEEK_END);
    long length = (long) ftell(fp);
    fseek(fp, 0, SEEK_SET);

    // Allocate memory for the shader source (+1 for the null terminator)
    char* buffer = malloc(length + 1);
    if (!buffer) {
        log_and_print("Error: Not enough memory to load shader '%s'\n", filePath);
        fclose(fp);
        return NULL;
    }

    // Read the file contents into the buffer
    size_t bytesRead = fread(buffer, 1, length, fp);
    if (bytesRead != length) {
        log_and_print("Error: Incomplete read of shader '%s'\n", filePath);
        free(buffer);
        fclose(fp);
        return NULL;
    }
    buffer[length] = '\0';  // Null-terminate the string
    fclose(fp);
    log_and_print("Shader '%s' loaded successfully.\n", filePath);
    return buffer;
}

/**
 * Compile a shader from its source code.
 *
 * @param type The type of shader (GL_VERTEX_SHADER or GL_FRAGMENT_SHADER).
 * @param source The shader source code.
 * @return The compiled shader object. Returns 0 on failure.
 */
unsigned int compileShader(int type, const char* source) {
    if (source == NULL) {
        log_and_print("Error: Shader source is NULL\n");
        return 0;
    }
    const char* shaderType = (type == GL_VERTEX_SHADER)     ? "vertex"
                             : (type == GL_FRAGMENT_SHADER) ? "fragment"
                                                            : "unknown";
    log_and_print("Compiling %s shader...\n", shaderType);

    // Create the shader object
    unsigned int shader = glCreateShader(type);
    if (shader == 0) {
        log_and_print("Error: Failed to create %s shader\n", shaderType);
        return 0;
    }

    // Attach the source code to the shader and compile it
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
 * Create and link a shader program using a vertex shader and a fragment shader.
 *
 * @param vertexShader The compiled vertex shader.
 * @param fragmentShader The compiled fragment shader.
 * @return The linked shader program. Returns 0 on failure.
 */
unsigned int createShaderProgram(unsigned int vertexShader, unsigned int fragmentShader) {
    if (vertexShader == 0 || fragmentShader == 0) {
        log_and_print("Error: Invalid shader for program creation\n");
        return 0;
    }
    log_and_print("Creating shader program...\n");

    // Create the shader program object
    unsigned int program = glCreateProgram();
    if (program == 0) {
        log_and_print("Error: Failed to create shader program\n");
        return 0;
    }

    // Attach the shaders and link the program
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

    // Detach and delete the shaders after successful linking
    glDetachShader(program, vertexShader);
    glDetachShader(program, fragmentShader);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    log_and_print("Shader program created and linked successfully.\n");

    return program;
}

/**
 * Callback function that adjusts the viewport when the window is resized.
 *
 * @param window The GLFW window.
 * @param width The new width of the window.
 * @param height The new height of the window.
 */
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
#ifdef __APPLE__
    // Retina displays on macOS require a scaling factor of 2
    glViewport(0, 0, width * 2, height * 2);
#else
    glViewport(0, 0, width, height);
#endif
    log_and_print("Window resized: width = %d, height = %d\n", width, height);
}

/**
 * Main entry point of the application.
 */
int main(int argc, char** argv) {
    // Open the log file for writing
    g_logFile = fopen("program.log", "w");
    if (!g_logFile) {
        printf("Error: Unable to open log file.\n");
        return -1;
    }

    log_and_print("----- Program Start -----\n");

    // Default configuration parameters
    int         windowWidth        = 2560;
    int         windowHeight       = 1440;
    const char* windowTitle        = "My First Shader!";
    const char* vertexShaderPath   = "shaders/vertex_shader.glsl";
    const char* fragmentShaderPath = "shaders/fragment_shader.glsl";

    // Use command line arguments to override defaults if provided
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
        log_and_print("Command line parameters received.\n");
    } else {
        // Interactive menu if no command line arguments are provided
        log_and_print("No command line parameters detected, launching interactive menu.\n");
        printf("Welcome to the OpenGL application!\n");
        printf("Choose an option:\n");
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

            log_and_print("User provided custom parameters.\n");
        } else {
            log_and_print("Using default parameters.\n");
        }
    }

    // Log the final configuration
    log_and_print("Configuration:\n");
    log_and_print("  Dimensions: %d x %d\n", windowWidth, windowHeight);
    log_and_print("  Title     : %s\n", windowTitle);
    log_and_print("  Vertex Shader : %s\n", vertexShaderPath);
    log_and_print("  Fragment Shader : %s\n", fragmentShaderPath);

    // Initialize GLFW
    if (!glfwInit()) {
        log_and_print("Error initializing GLFW.\n");
        fclose(g_logFile);
        return -1;
    }
    log_and_print("GLFW initialized successfully.\n");

    // Set GLFW window hints for the desired OpenGL version and profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // Create the GLFW window
    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, windowTitle, NULL, NULL);
    if (!window) {
        log_and_print("Error creating the window.\n");
        glfwTerminate();
        fclose(g_logFile);
        return -1;
    }
    log_and_print("Window created successfully.\n");

    // Make the window's OpenGL context current
    glfwMakeContextCurrent(window);

    // Load OpenGL functions using GLAD
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        log_and_print("Error loading GLAD.\n");
        glfwTerminate();
        fclose(g_logFile);
        return -1;
    }
    log_and_print("GLAD loaded successfully.\n");

    // Get the actual framebuffer size and set the viewport accordingly
    int fbWidth, fbHeight;
    glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
    glViewport(0, 0, fbWidth, fbHeight);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Load and compile the shader sources
    char* vertexSource   = loadShaderSource(vertexShaderPath);
    char* fragmentSource = loadShaderSource(fragmentShaderPath);
    if (!vertexSource || !fragmentSource) {
        log_and_print("Error loading shader sources.\n");
        glfwTerminate();
        fclose(g_logFile);
        return 1;
    }

    unsigned int vertexShader = compileShader(GL_VERTEX_SHADER, vertexSource);
    unsigned int fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSource);
    free(vertexSource);
    free(fragmentSource);

    if (vertexShader == 0 || fragmentShader == 0) {
        log_and_print("Shader compilation failed.\n");
        glfwTerminate();
        fclose(g_logFile);
        return 1;
    }

    // Create and link the shader program
    unsigned int shaderProgram = createShaderProgram(vertexShader, fragmentShader);
    if (shaderProgram == 0) {
        log_and_print("Shader program linking failed.\n");
        glfwTerminate();
        fclose(g_logFile);
        return 1;
    }

    // Define vertices for a fullscreen triangle
    float vertices[] = {
        -1.0f, -1.0f,  // Bottom left
         3.0f, -1.0f,  // Bottom right
        -1.0f,  3.0f   // Top left
    };

    // Generate and bind the Vertex Array Object (VAO) and Vertex Buffer Object (VBO)
    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Set the vertex attribute pointers
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*) 0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    log_and_print("Starting render loop.\n");
    // Main render loop
    while (!glfwWindowShouldClose(window)) {
        // Clear the color buffer
        glClear(GL_COLOR_BUFFER_BIT);

        // Use the shader program and draw the triangle
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // Swap the front and back buffers and poll for events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    log_and_print("Exiting render loop.\n");

    // Clean up and free resources
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
    glfwTerminate();
    log_and_print("Resources released and GLFW terminated.\n");
    log_and_print("----- Program End -----\n");

    fclose(g_logFile);
    return 0;
}
