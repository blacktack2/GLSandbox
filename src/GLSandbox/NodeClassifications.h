#pragma once

enum class NodeType {
    Entry = 0,
    Exit  = 1,
    RenderPass = 100,

    Arithmetic = 200,

    Integer = 300,
    Float   = 320,

    Mesh   = 400,
    Shader = 401,

    Undefined = -1
};

enum class NodeGroup {
    Execution = 0,
    Maths,
    Numerics,
    Graphics,
};
