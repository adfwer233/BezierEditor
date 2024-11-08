#pragma once

#include "vkl/system/render_system/simple_render_system.hpp"

struct Line2DRenderSystemModifier{
    static void modifyPipeline(PipelineConfigInfo &configInfo) {
        configInfo.inputAssemblyInfo.topology = VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
    }
};

using Line2DRenderSystem = SimpleRenderSystem<0, SimplePushConstantInfoList, Line2DRenderSystemModifier>;