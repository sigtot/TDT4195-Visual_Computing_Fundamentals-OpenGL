#include <glad/glad.h>
#include "vao.hpp"

#define NUM_COORDINATES 3
#define NUM_COLOR_COORDINATES 4

unsigned int createVAO(
        std::vector<float> vertices,
        std::vector<unsigned int> indices,
        std::vector<float> colors,
        std::vector<float> normals,
        unsigned int numPoints)
{
    unsigned int VAO = 0;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    int num_vbos = 3;
    unsigned int VBO[3];
    unsigned int vertexIndex = 0;
    unsigned int colorIndex = 1;
    unsigned int normalIndex = 2;
    glGenBuffers(num_vbos, VBO);

    // Vertices
    glBindBuffer(GL_ARRAY_BUFFER, VBO[vertexIndex]);
    glBufferData(GL_ARRAY_BUFFER, NUM_COORDINATES * numPoints * sizeof(float), &vertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(vertexIndex, NUM_COORDINATES, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(vertexIndex);

    // Colors
    glBindBuffer(GL_ARRAY_BUFFER, VBO[colorIndex]);
    glBufferData(GL_ARRAY_BUFFER, NUM_COLOR_COORDINATES * numPoints * sizeof(float), &colors[0], GL_STATIC_DRAW);
    glVertexAttribPointer(colorIndex, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(colorIndex);

    // Normals
    glBindBuffer(GL_ARRAY_BUFFER, VBO[normalIndex]);
    glBufferData(GL_ARRAY_BUFFER, NUM_COORDINATES * numPoints * sizeof(float), &normals[0], GL_STATIC_DRAW);
    glVertexAttribPointer(normalIndex, NUM_COORDINATES, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(normalIndex);

    // IBO
    unsigned int IBO = 0;
    glGenBuffers(1, &IBO );
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, NUM_COORDINATES * numPoints * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
    return VAO;
}

unsigned int VAOFromMesh(Mesh mesh)
{
    return createVAO(
            mesh.vertices,
            mesh.indices,
            mesh.colours,
            mesh.normals,
            mesh.vertexCount());
}
