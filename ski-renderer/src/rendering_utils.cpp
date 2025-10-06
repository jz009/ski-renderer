#pragma once
#include "includes_fwd.h"

#include "rendering_utils.h"

#include "camera.h"


ObjResult loadObj(const std::filesystem::path &geometry)
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    std::string warn;
    std::string err;
    std::vector<VertexAttributes> vertexData;

    tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, geometry.string().c_str());
    vertexData.clear();
    glm::vec3 min = {std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max()};
    glm::vec3 max = {std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest()};
    for (const auto &shape : shapes)
    {
        size_t offset = vertexData.size();
        vertexData.resize(offset + shape.mesh.indices.size());

        for (size_t i = 0; i < shape.mesh.indices.size(); ++i)
        {
            const tinyobj::index_t &idx = shape.mesh.indices[i];
            auto x = attrib.vertices[3 * idx.vertex_index + 0];
            auto y = attrib.vertices[3 * idx.vertex_index + 1];
            auto z = attrib.vertices[3 * idx.vertex_index + 2];

            min.x = std::min(x, min.x);
            min.y = std::min(-y, min.y);
            min.z = std::min(z, min.z);
            max.x = std::max(x, max.x);
            max.y = std::max(-y, max.y);
            max.z = std::max(z, max.z);

            vertexData[offset + i].position = {
                x,
                -y,
                z};

            vertexData[offset + i].normal = {
                attrib.normals[3 * idx.normal_index + 0],
                -attrib.normals[3 * idx.normal_index + 2],
                attrib.normals[3 * idx.normal_index + 1]};

            vertexData[offset + i].color = {
                attrib.colors[3 * idx.vertex_index + 0],
                attrib.colors[3 * idx.vertex_index + 1],
                attrib.colors[3 * idx.vertex_index + 2]};
        }
    }
    return ObjResult{vertexData, {min, max}};
}

std::vector<VertexAttributes> load2D(const std::filesystem::path &geometry)
{
    std::vector<VertexAttributes> vertexData;

    std::ifstream file(geometry);

    enum class Section
    {
        None,
        Points,
        Indices,
    };
    Section currentSection = Section::None;

    float x, y, z;
    std::string line;
    while (!file.eof())
    {
        getline(file, line);

        if (!line.empty() && line.back() == '\r')
        {
            line.pop_back();
        }

        if (line == "[points]")
        {
            currentSection = Section::Points;
        }
        else if (line == "[indices]")
        {
            currentSection = Section::Indices;
        }
        else if (line[0] == '#' || line.empty())
        {
        }
        else if (currentSection == Section::Points)
        {
            std::istringstream iss(line);
            iss >> x >> y >> z;
            vertexData.push_back({glm::vec3(x, y, z), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 0.0, 0.0)});
        }
    }
    return vertexData;
}

void updateModel(Model& model, const Transform& transform, const Camera& camera) {
    model.material.uniforms.modelMatrix = glm::translate(glm::mat4x4(1.0), transform.offset) * glm::translate(glm::mat4x4(1.0), transform.position) * glm::scale(glm::mat4x4(1.0), transform.scale);
    model.material.uniforms.viewMatrix = glm::lookAt(camera.position, camera.target, camera.up);
}