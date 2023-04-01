#include "Renderer.h"

void Renderer::drawScene() {
    for (const auto& pass : mRenderPipeline)
        pass();
}

void Renderer::clearPipeline() {
    mRenderPipeline.clear();
}

void Renderer::appendPipeline(IPipelineHandler::pipeline_callback callback) {
    mRenderPipeline.push_back(callback);
}
