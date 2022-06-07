
#pragma once
#include <glm/glm.hpp>
#include <vector>


// // 1st attribute buffer : vertices
// glEnableVertexAttribArray(0);
// glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
// glVertexAttribPointer(
//     0,                  // attribute
//     3,                  // size
//     GL_FLOAT,           // type
//     GL_FALSE,           // normalized?
//     0,                  // stride
//     (void*)0            // array buffer offset
// );

// // 2nd attribute buffer : UVs
// glEnableVertexAttribArray(1);
// glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
// glVertexAttribPointer(
//     1,                                // attribute
//     2,                                // size
//     GL_FLOAT,                         // type
//     GL_FALSE,                         // normalized?
//     0,                                // stride
//     (void*)0                          // array buffer offset
// );

// // 3rd attribute buffer : normals
// glEnableVertexAttribArray(2);
// glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
// glVertexAttribPointer(
//     2,                                // attribute
//     3,                                // size
//     GL_FLOAT,                         // type
//     GL_FALSE,                         // normalized?
//     0,                                // stride
//     (void*)0                          // array buffer offset
// );

class Mesh2D
{

private:
    std::vector<glm::vec2> vertices_;
    std::vector<glm::vec2> uvs_;
    std::vector<glm::vec4> tints_;
    std::vector<glm::ivec3> triangles_;
};

class Mesh3D
{

private:
    std::vector<glm::vec3> vertices_;
    std::vector<glm::vec3> normals_;
    std::vector<glm::vec2> uvs_;
    std::vector<glm::vec4> tints_;
    std::vector<glm::ivec3> triangles_;
};