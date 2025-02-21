#include <stdio.h>
#include <stdlib.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    #ifdef __APPLE__
        glViewport(0, 0, width * 2, height * 2); // Retina displays need *2
    #else
        glViewport(0, 0, width, height);
    #endif
}

int main() {
    // Initialisation de GLFW
    if (!glfwInit()) {
        printf("Erreur lors de l'initialisation de GLFW\n");
        return -1;
    }

    // Création de la fenêtre
    GLFWwindow* window = glfwCreateWindow(2560, 1440, "Mon premier shader !", NULL, NULL);
    if (!window) {
        printf("Erreur lors de la création de la fenêtre\n");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // Initialisation de GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        printf("Erreur lors du chargement de GLAD\n");
        return -1;
    }

    glViewport(0, 0, 800, 600);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Boucle principale
    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
