#!/bin/bash

# Vérifie si les fichiers source existent
if [ ! -f main.c ] || [ ! -f glad.c ]; then
    echo "Erreur: fichiers source manquants (main.c ou glad.c)"
    exit 1
fi

# find main.c | xargs clang-format -i

# Compilation
clang main.c glad.c -o shader_app \
    -Iinclude -Iinclude/KHR \
    -I/opt/homebrew/Cellar/glfw/3.4/include \
    -L/opt/homebrew/Cellar/glfw/3.4/lib \
    -lglfw -framework OpenGL

# Vérifie si la compilation a réussi
if [ $? -eq 0 ]; then
    echo "Compilation réussie!"
    echo "Exécution du programme..."
    ./shader_app
else
    echo "Erreur lors de la compilation"
    exit 1
fi
