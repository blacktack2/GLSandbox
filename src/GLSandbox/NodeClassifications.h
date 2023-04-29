#pragma once
#include <string>

// Warning - Changing the ID of a NodeType may invalidate existing serialized graphs with that type.
enum class NodeType : std::size_t {
    Entry = 0,
    Input,
    Output,
    RenderPass,

    Arithmetic,

    Integer,
    IVec2,
    IVec3,
    IVec4,

    Float,
    Vec2,
    Vec3,
    Vec4,

    Mat2,
    Mat3,
    Mat4,

    Mesh,
    Shader,
    Texture,
    TextureFile,
    Framebuffer,

    UV,

    Colour,
    Direction,

    ModelMatrix,
    ViewMatrix,
    ProjMatrix,

    ScreenWidth,

    Max,
};

enum class NodeGroup : std::size_t {
    Execution = 0,
    Maths,
    Numerics,
    Data,
    Graphics,
    System,

    Max,
};

struct NodeData {
    NodeType type;
    NodeGroup group;
    std::string displayName;
    std::string serializedName;
    std::string tooltip;
};

static const NodeData& getNodeData(NodeType type) {
    // Must be in the same order as NodeType enum
    static const NodeData cNODE_DATA[] = {
        {NodeType::Entry,       NodeGroup::Execution, "Entry",             "Entry",       "Entry point of the graph."},
        {NodeType::Input,       NodeGroup::Execution, "Input",             "Input",       "Graph input."},
        {NodeType::Output,      NodeGroup::Execution, "Output",            "Output",      "Graph output."},
        {NodeType::RenderPass,  NodeGroup::Execution, "Render Pass",       "RenderPass",  "Single draw call."},

        {NodeType::Arithmetic,  NodeGroup::Maths,     "Arithmetic",        "Arithmetic",  "Arithmetic operations on numeric types."},

        {NodeType::Integer,     NodeGroup::Numerics,  "Integer",           "Integer",     "Whole number."},
        {NodeType::IVec2,       NodeGroup::Numerics,  "I-Vector-2",        "IVec2",       "Two-component integer."},
        {NodeType::IVec3,       NodeGroup::Numerics,  "I-Vector-3",        "IVec3",       "Three-component integer."},
        {NodeType::IVec4,       NodeGroup::Numerics,  "I-Vector-4",        "IVec4",       "Four-component integer."},

        {NodeType::Float,       NodeGroup::Numerics,  "Float",             "Float",       "Continuous number."},
        {NodeType::Vec2,        NodeGroup::Numerics,  "Vector-2",          "Vec2",        "Two-component float."},
        {NodeType::Vec3,        NodeGroup::Numerics,  "Vector-3",          "Vec3",        "Three-component float."},
        {NodeType::Vec4,        NodeGroup::Numerics,  "Vector-4",          "Vec4",        "Four-component float."},

        {NodeType::Mat2,        NodeGroup::Numerics,  "Matrix-2x2",        "Mat2",        "2x2 float matrix."},
        {NodeType::Mat3,        NodeGroup::Numerics,  "Matrix-3x3",        "Mat3",        "3x3 float matrix."},
        {NodeType::Mat4,        NodeGroup::Numerics,  "Matrix-4x4",        "Mat4",        "4x4 float matrix."},

        {NodeType::Mesh,        NodeGroup::Graphics,  "Mesh",              "Mesh",        ""},
        {NodeType::Shader,      NodeGroup::Graphics,  "Shader",            "Shader",      ""},
        {NodeType::Texture,     NodeGroup::Graphics,  "Texture",           "Texture",     "Custom format texture (for writing)."},
        {NodeType::TextureFile, NodeGroup::Graphics,  "File Texture",      "TextureFile", "Texture from a file."},
        {NodeType::Framebuffer, NodeGroup::Graphics,  "Framebuffer",       "Framebuffer", ""},

        {NodeType::UV,          NodeGroup::Data,      "UV",                "UV",          "Texture coords (clamped 0-1)."},

        {NodeType::Colour,      NodeGroup::Data,      "Colour",            "Colour",      "RGB Colour (Vec3 clamped 0-1)."},
        {NodeType::Direction,   NodeGroup::Data,      "Direction",         "Direction",   "Normalized direction Vec3."},

        {NodeType::ModelMatrix, NodeGroup::Data,      "Model Matrix",      "ModelMatrix", ""},
        {NodeType::ViewMatrix,  NodeGroup::Data,      "View Matrix",       "ViewMatrix",  ""},
        {NodeType::ProjMatrix,  NodeGroup::Data,      "Projection Matrix", "ProjMatrix",  ""},

        {NodeType::ScreenWidth, NodeGroup::System,    "Screen Width",      "ScreenWidth", "Vec2 which auto-matches screen width."},
    };

    return cNODE_DATA[(size_t)type];
}

static const std::unordered_map<NodeGroup, const std::vector<NodeType>> gNODE_GROUPS = []() {
    std::unordered_map<NodeGroup, std::vector<NodeType>> groups{};
    for (int i = 0; i < (int)NodeGroup::Max; i++)
        groups.emplace((NodeGroup)i, std::vector<NodeType>());

    for (int i = 0; i < (int)NodeType::Max; i++) {
        NodeGroup group = getNodeData((NodeType)i).group;
        groups.find(group)->second.emplace_back((NodeType)i);
    }

    std::unordered_map<NodeGroup, const std::vector<NodeType>> constGroups;
    for (const auto& group : groups)
        constGroups.emplace(group.first, group.second);
    return constGroups;
}();

static const std::unordered_map<std::string, NodeType> gNODE_SERIALS = []() {
    std::unordered_map<std::string, NodeType> serials{};

    for (int i = 0; i < (int)NodeType::Max; i++) {
        NodeType type = (NodeType)i;
        serials.emplace(getNodeData(type).serializedName, type);
    }

    return serials;
}();
