#include "Renderer.h"

void Renderer::drawScene() {
    for (const auto& pass : mRenderPipeline)
        pass();
}

void Renderer::clearPipeline(bool resetEntryPoint) {
    if (resetEntryPoint && mEntryPoint) {
        mEntryPoint->unsetEntry();
        mEntryPoint = nullptr;
    }
    mRenderPipeline.clear();
}

void Renderer::appendPipeline(IPipelineHandler::pipeline_callback callback) {
    mRenderPipeline.push_back(callback);
}

void Renderer::setEntryPoint(IPipelineEntry& entry) {
    if (mEntryPoint)
        mEntryPoint->unsetEntry();
    mEntryPoint = &entry;
    mEntryPoint->setEntry();
}
