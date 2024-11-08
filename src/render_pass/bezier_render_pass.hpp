#pragma once


#include "scene.hpp"
#include "graphics_lab/render_graph/render_pass.hpp"
#include "render_system/line2d_render_system.hpp"

#include "vkl/scene_tree/vkl_mesh.hpp"
#include "vkl/scene_tree/vkl_mesh_types.hpp"
#include "vkl/scene_tree/vkl_geometry_mesh.hpp"

#ifndef SHADER_DIR
#define SHADER_DIR "./shader/"
#endif

namespace SceneTree {
    template<> struct VklNodeMesh<BezierCurve2D> {
        using render_type = SceneTree::VklCurveMesh2D;
        std::unique_ptr<render_type> mesh;

        void recreateMeshes() {
            createMesh();
        }

        VklNodeMesh(VklDevice& device, SceneTree::GeometryNode<BezierCurve2D> * node): device_ (device), node_(node) {
            createMesh();
        }

    private:
        void createMesh() {
            typename render_type::Builder builder;
            size_t n = 100;

            for (int i = 0; i <= n; i++) {
                Vertex2D vert;
                vert.position = node_->data.evaluate_linear(float(i) / n);
                vert.color = {1.0f, 0.0f, 0.0f};
                builder.vertices.push_back(vert);
            }
            for (int i = 0; i < n; i++) {
                builder.indices.emplace_back(i, i + 1);
            }
            mesh = std::make_unique<render_type>(device_, builder);
        }
        VklDevice &device_;
        SceneTree::GeometryNode<BezierCurve2D> *node_;
    };
}
namespace GraphicsLab::RenderGraph {

struct BezierRenderPass: public RenderPass {
    explicit BezierRenderPass(VklDevice& device, SceneTree::VklSceneTree *scene_tree): RenderPass(device), scene(scene_tree) {}

    RenderPassReflection render_pass_reflect() override {
        RenderPassReflection reflection;
        reflection.add_output("bezier_output", "output of bezier editor")
            .type(RenderPassReflection::Field::Type::Texture2D)
            .sample_count(8)
            .format(VK_FORMAT_R8G8B8A8_SRGB)
            .extent(2048, 2048)
            .set_annotation("imgui_show", true)
            .set_annotation("do_not_release", true);
        return reflection;
    }

    void post_compile(RenderContext *render_context) override {
        spdlog::info("{}/curve2d.vert", SHADER_DIR);
        render_system = std::make_unique<Line2DRenderSystem>(device_, vkl_render_pass->renderPass, std::vector<VklShaderModuleInfo>{
                {std::format("{}/curve2d.vert.spv", SHADER_DIR), VK_SHADER_STAGE_VERTEX_BIT},
                {std::format("{}/curve2d.frag.spv", SHADER_DIR), VK_SHADER_STAGE_FRAGMENT_BIT}
        });
    }

    void execute(RenderContext *render_context, const RenderPassExecuteData &execute_data) override {
        auto commandBuffer = render_context->get_current_command_buffer();

        begin_render_pass(commandBuffer);

        uint32_t frame_index = render_context->get_current_frame_index();
        std::scoped_lock sceneTreeLock(scene->sceneTreeMutex);
        auto bezier_mesh_buffer = SceneTree::VklNodeMeshBuffer<BezierCurve2D>::instance();
        for (auto node: scene->traverse_geometry_nodes<BezierCurve2D>()) {
            auto node_mesh =  bezier_mesh_buffer->getGeometryModel(device_, node);
            FrameInfo<std::decay_t<decltype(*node_mesh)>::render_type> frameInfo{
                    .frameIndex = static_cast<int>(frame_index) % 2,
                    .frameTime = 0,
                    .commandBuffer = commandBuffer,
                    .camera = scene->active_camera->camera,
                    .model = *node_mesh->mesh,
            };
            render_system->renderObject(frameInfo);
        }

        end_render_pass(commandBuffer);
    }

private:
    std::unique_ptr<Line2DRenderSystem> render_system = nullptr;
    SceneTree::VklSceneTree *scene;
};

}