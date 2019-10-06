#ifndef GLOOM_VAO_HPP
#define GLOOM_VAO_HPP

#include <vector>
#include <lib/mesh.hpp>

unsigned int createVAO(
        std::vector<float> vertices,
        std::vector<unsigned int> indices,
        std::vector<float> colors,
        std::vector<float> normals,
        unsigned int numPoints);

unsigned int VAOFromMesh(Mesh mesh);
#endif //GLOOM_VAO_HPP
