#include "InstanceGroup.hpp"

#include <iostream>

#include <glm/gtc/type_ptr.hpp>

#include "GLDebug.hpp"

InstanceGroup::InstanceGroup(Mesh::ShPtr mesh, Material::ShPtr material, int submesh_index)
    : mesh_(mesh), material_(material), submesh_index_(submesh_index),
      vao_(0), instance_vbo_(0), uploaded_count_(0) {
}

InstanceGroup::~InstanceGroup() {
    if (instance_vbo_) glDeleteBuffers(1, &instance_vbo_);
    if (vao_) glDeleteVertexArrays(1, &vao_);
}

void InstanceGroup::add_instance(const glm::mat4& transform) {
    transforms_.push_back(transform);
}

void InstanceGroup::clear_instances() {
    transforms_.clear();
    uploaded_count_ = 0;
}

void InstanceGroup::setup_vao() {
    if (vao_) return;

    GL_CHECK(glGenVertexArrays(1, &vao_));
    GL_CHECK(glBindVertexArray(vao_));

    // Bind mesh's VBO for per-vertex attributes (locations 0-5)
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, mesh_->vbo()));

    // location 0 = position (vec3)
    GL_CHECK(glEnableVertexAttribArray(0));
    GL_CHECK(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0,
        reinterpret_cast<GLvoid*>(mesh_->vertex_offset())));
    // location 1 = texCoord (vec2)
    GL_CHECK(glEnableVertexAttribArray(1));
    GL_CHECK(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0,
        reinterpret_cast<GLvoid*>(mesh_->texcoord_offset())));
    // location 2 = normal (vec3)
    GL_CHECK(glEnableVertexAttribArray(2));
    GL_CHECK(glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0,
        reinterpret_cast<GLvoid*>(mesh_->normal_offset())));
    // location 3 = color (vec3)
    GL_CHECK(glEnableVertexAttribArray(3));
    GL_CHECK(glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0,
        reinterpret_cast<GLvoid*>(mesh_->color_offset())));
    // location 4 = tangent (vec3)
    GL_CHECK(glEnableVertexAttribArray(4));
    GL_CHECK(glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0,
        reinterpret_cast<GLvoid*>(mesh_->tangent_offset())));
    // location 5 = bitangent (vec3)
    GL_CHECK(glEnableVertexAttribArray(5));
    GL_CHECK(glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, 0,
        reinterpret_cast<GLvoid*>(mesh_->bitangent_offset())));

    // Create instance VBO for mat4 transforms
    GL_CHECK(glGenBuffers(1, &instance_vbo_));
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, instance_vbo_));
    // Allocate initial storage (will be resized in upload_instances if needed)
    // This ensures the buffer has storage while VAO is being configured
    GL_CHECK(glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4), nullptr, GL_DYNAMIC_DRAW));

    // mat4 uses 4 consecutive vec4 attribute slots (locations 6-9)
    // Stride is sizeof(glm::mat4) = 64 bytes
    size_t mat4Size = sizeof(glm::mat4);
    size_t vec4Size = sizeof(glm::vec4);

    for (int i = 0; i < 4; ++i) {
        GLuint loc = 6 + i;
        GL_CHECK(glEnableVertexAttribArray(loc));
        GL_CHECK(glVertexAttribPointer(loc, 4, GL_FLOAT, GL_FALSE, mat4Size,
            reinterpret_cast<GLvoid*>(i * vec4Size)));
        GL_CHECK(glVertexAttribDivisor(loc, 1));
    }

    GL_CHECK(glBindVertexArray(0));
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

void InstanceGroup::upload_instances() {
    if (transforms_.empty()) return;

    setup_vao();

    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, instance_vbo_));
    GL_CHECK(glBufferData(GL_ARRAY_BUFFER, transforms_.size() * sizeof(glm::mat4),
        transforms_.data(), GL_STATIC_DRAW));
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));

    uploaded_count_ = transforms_.size();
}

void InstanceGroup::draw_instanced() const {
    if (uploaded_count_ == 0) return;
    if (vao_ == 0) {
        std::cout << "InstanceGroup::draw_instanced: ERROR vao is 0" << std::endl;
        return;
    }

    // Calculate vertex offset and count for this submesh
    int offset = 0;
    int count = 0;
    const auto& submeshes = mesh_->submeshes();
    for (int i = 0; i < static_cast<int>(submeshes.size()); ++i) {
        if (i == submesh_index_) {
            count = submeshes[i].triangle_count * 3;
            break;
        }
        offset += submeshes[i].triangle_count * 3;
    }

    glBindVertexArray(vao_);

    GL_CHECK(glDrawArraysInstanced(GL_TRIANGLES, offset, count, uploaded_count_));
    GL_CHECK(glBindVertexArray(0));
}

void InstanceGroup::draw_instanced_shadow() const {
    if (uploaded_count_ == 0) return;

    // For shadow pass, draw all submeshes (entire mesh geometry)
    glBindVertexArray(vao_);
    glDrawArraysInstanced(GL_TRIANGLES, 0, mesh_->vertex_count(), uploaded_count_);
    glBindVertexArray(0);
}
