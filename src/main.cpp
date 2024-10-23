#define NOMINMAX
#include "graphics_lab/project.hpp"

#include "pybind11/pybind11.h"
#include "pybind11/stl.h"

class BezierEditor : public IGraphicsLabProject {
public:
    virtual void tick() override {
        std::cout << "tick" << std::endl;
    }

    void afterLoad() override {
        auto logManager = context.logManager;
        logManager->setLogger("BezierEditor");
    }

    std::string name() override {
        return "BezierEditor";
    }

    ReflectDataType reflect() override {
        return IGraphicsLabProject::reflect();
    }

    void bindPython(pybind11::module &m) override {
    }
};

extern "C" __declspec(dllexport) IGraphicsLabProject* createProject() {
    return new BezierEditor();
}