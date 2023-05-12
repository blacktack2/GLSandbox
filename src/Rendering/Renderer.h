#pragma once
#include "IPipelineHandler.h"

#include <vector>

class Renderer : public IPipelineHandler {
public:
    ~Renderer() override = default;

    virtual void update() = 0;
    virtual void drawDebug() = 0;

    void drawScene();

    void clearPipeline(bool resetEntryPoint) final;
    void appendPipeline(pipeline_callback callback) final;

    void setEntryPoint(IPipelineEntry& entry) final;
protected:
    Renderer() = default;
private:
    IPipelineEntry* mEntryPoint = nullptr;

    std::vector<pipeline_callback> mRenderPipeline{};
};
