#pragma once
#include <functional>
#include <vector>

class Texture;

class Framebuffer {
public:
    enum class ErrorState {
        Invalid,
        Valid,
        Undefined,
        IncompleteAttachment,
        IncompleteMissingAttachment,
        IncompleteDrawBuffer,
        IncompleteReadBuffer,
        IncompleteMultiSample,
        IncompleteLayerTargets,
    };

    Framebuffer();
    ~Framebuffer();

    void bind() const;
    static void unbind();

    void reset();

    void drawBuffers();
    void bindTexture(const Texture& texture);
    void bindTexture(const Texture& texture, unsigned int attachment);

    [[nodiscard]] inline ErrorState getState() const {
        return mErrorState;
    }
private:
    unsigned int mFBO = 0;

    unsigned int mNumColourBuffers = 0;

    ErrorState mErrorState = ErrorState::Invalid;
};

