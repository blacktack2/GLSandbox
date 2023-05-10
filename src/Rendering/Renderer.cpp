#include "Renderer.h"

void Renderer::drawScene() {
    for (const auto& pass : mRenderPipeline)
        pass();
}

void Renderer::clearPipeline() {
    if (mEntryPoint)
        mEntryPoint->unsetEntry();
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
