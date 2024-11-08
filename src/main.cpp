#define NOMINMAX
#include "graphics_lab/project.hpp"

#include "pybind11/pybind11.h"
#include "pybind11/stl.h"

#include "geometry/curve/bezier_curve_2d.hpp"
#include "render_pass/bezier_render_pass.hpp"

class BezierEditor : public IGraphicsLabProject {
public:
    virtual void tick() override {
        std::cout << "tick" << std::endl;
    }

    void afterLoad() override {
        auto logManager = context.logManager;
        logManager->setLogger("BezierEditor");

        sceneTree = std::make_unique<SceneTree::VklSceneTree>(*context.device);
        bezierRenderPass = std::make_unique<GraphicsLab::RenderGraph::BezierRenderPass>(*context.device, sceneTree.get());

        std::vector<std::array<float, 2>> ctrl_pts = {
                {0.0, 0.0}, {0.5, 1.0}, {1.0, 0.0}
        };

//        auto node = std::make_unique<SceneTree::GeometryNode<BezierCurve2D>>(BezierCurve2D(std::move(ctrl_pts)));

        sceneTree->addGeometryNode(std::move(BezierCurve2D(std::move(ctrl_pts))));

        {
            std::scoped_lock renderGraphLock(context.applicationContext->renderGraphMutex);
            context.applicationContext->renderGraph->add_pass(bezierRenderPass.get(), "BezierEditor");
            context.applicationContext->compileRenderGraph();
        }
    }

    std::string name() override {
        return "BezierEditor";
    }

    ReflectDataType reflect() override {
        return IGraphicsLabProject::reflect();
    }

    void bindPython(pybind11::module &m) override {
    }

private:
    std::vector<BezierCurve2D> bezier_curves;
    std::unique_ptr<SceneTree::VklSceneTree> sceneTree;
    std::unique_ptr<GraphicsLab::RenderGraph::BezierRenderPass> bezierRenderPass = nullptr;
};

extern "C" __declspec(dllexport) IGraphicsLabProject* createProject() {
    return new BezierEditor();
}