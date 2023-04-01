#pragma once
#include <functional>

class IPipelineHandler {
public:
    typedef std::function<void()> pipeline_callback;

    virtual ~IPipelineHandler() = default;

    virtual void clearPipeline() = 0;
    virtual void appendPipeline(pipeline_callback callback) = 0;
protected:
    IPipelineHandler() = default;
};