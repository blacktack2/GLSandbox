#pragma once
#include <functional>

class IPipelineEntry {
public:
    virtual ~IPipelineEntry() = default;

    virtual void setEntry() = 0;
    virtual void unsetEntry() = 0;
protected:
    IPipelineEntry() = default;
};

class IPipelineHandler {
public:
    typedef std::function<void()> pipeline_callback;

    virtual ~IPipelineHandler() = default;

    virtual void clearPipeline() = 0;
    virtual void resetPipeline() = 0;
    virtual void appendPipeline(pipeline_callback callback) = 0;

    virtual void loadAnalysisPipeline(unsigned int bulk, std::size_t iterations) = 0;

    virtual void setEntryPoint(IPipelineEntry& entry) = 0;
protected:
    IPipelineHandler() = default;
};
