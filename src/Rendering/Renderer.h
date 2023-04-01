#pragma once
#include "IPipelineHandler.h"

#include <vector>

class Renderer : public IPipelineHandler {
public:
    ~Renderer() override = default;

    virtual void update() = 0;
    virtual void drawDebug() = 0;

    void drawScene();

    void clearPipeline() final;
    void appendPipeline(pipeline_callback callback) final;
protected:
    Renderer() = default;

    std::vector<pipeline_callback> mRenderPipeline{};
};
