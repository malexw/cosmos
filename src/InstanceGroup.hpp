#ifndef COSMOS_INSTANCEGROUP_HPP_
#define COSMOS_INSTANCEGROUP_HPP_

#include <memory>
#include <vector>

#include <SDL3/SDL_opengl.h>
#include <glm/glm.hpp>

#include "Material.hpp"
#include "Mesh.hpp"
#include "util.hpp"

class InstanceGroup {
public:
    using ShPtr = std::shared_ptr<InstanceGroup>;

    InstanceGroup(Mesh::ShPtr mesh, Material::ShPtr material, int submesh_index);
    ~InstanceGroup();

    void add_instance(const glm::mat4& transform);
    void clear_instances();
    void upload_instances();
    void draw_instanced() const;
    void draw_instanced_shadow() const;

    Mesh::ShPtr mesh() const { return mesh_; }
    Material::ShPtr material() const { return material_; }
    int submesh_index() const { return submesh_index_; }
    size_t instance_count() const { return transforms_.size(); }

private:
    Mesh::ShPtr mesh_;
    Material::ShPtr material_;
    int submesh_index_;

    std::vector<glm::mat4> transforms_;

    GLuint vao_;
    GLuint instance_vbo_;
    size_t uploaded_count_;

    void setup_vao();

    InstanceGroup(const InstanceGroup&) = delete;
    InstanceGroup& operator=(const InstanceGroup&) = delete;
};

#endif
