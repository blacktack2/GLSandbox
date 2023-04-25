#pragma once
#include <string>

// Warning - Changing the ID of a NodeType may invalidate existing serialized graphs with that type.
enum class NodeType {
    Entry = 0,
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

    Max,
    Undefined = -1
};

enum class NodeGroup {
    Execution = 0,
    Maths,
    Numerics,
    Data,
    Graphics,

    Max,
    Undefined = -1
};

struct NodeData {
    NodeType type;
    NodeGroup group;
    std::string displayName;
    std::string serializedName;
};

static const NodeData& getNodeData(NodeType type) {
    // Must be in the same order as NodeType enum
    static const NodeData cNODE_DATA[] = {
        {NodeType::Entry,       NodeGroup::Execution, "Entry",             "Entry"},
        {NodeType::RenderPass,  NodeGroup::Execution, "Render Pass",       "RenderPass"},

        {NodeType::Arithmetic,  NodeGroup::Maths,     "Arithmetic",        "Arithmetic"},

        {NodeType::Integer,     NodeGroup::Numerics,  "Integer",           "Integer"},
        {NodeType::IVec2,       NodeGroup::Numerics,  "IVec2",             "IVec2"},
        {NodeType::IVec3,       NodeGroup::Numerics,  "IVec3",             "IVec3"},
        {NodeType::IVec4,       NodeGroup::Numerics,  "IVec4",             "IVec4"},

        {NodeType::Float,       NodeGroup::Numerics,  "Float",             "Float"},
        {NodeType::Vec2,        NodeGroup::Numerics,  "Vec2",              "Vec2"},
        {NodeType::Vec3,        NodeGroup::Numerics,  "Vec3",              "Vec3"},
        {NodeType::Vec4,        NodeGroup::Numerics,  "Vec4",              "Vec4"},

        {NodeType::Mat2,        NodeGroup::Numerics,  "Mat2",              "Mat2"},
        {NodeType::Mat3,        NodeGroup::Numerics,  "Mat3",              "Mat3"},
        {NodeType::Mat4,        NodeGroup::Numerics,  "Mat4",              "Mat4"},

        {NodeType::Mesh,        NodeGroup::Graphics,  "Mesh",              "Mesh"},
        {NodeType::Shader,      NodeGroup::Graphics,  "Shader",            "Shader"},
        {NodeType::Texture,     NodeGroup::Graphics,  "Texture",           "Texture"},
        {NodeType::TextureFile, NodeGroup::Graphics,  "TextureFile",       "TextureFile"},
        {NodeType::Framebuffer, NodeGroup::Graphics,  "Framebuffer",       "Framebuffer"},

        {NodeType::UV,          NodeGroup::Data,      "UV",                "UV"},

        {NodeType::Colour,      NodeGroup::Data,      "Colour",            "Colour"},
        {NodeType::Direction,   NodeGroup::Data,      "Direction",         "Direction"},

        {NodeType::ModelMatrix, NodeGroup::Data,      "Model Matrix",      "ModelMatrix"},
        {NodeType::ViewMatrix,  NodeGroup::Data,      "View Matrix",       "ViewMatrix"},
        {NodeType::ProjMatrix,  NodeGroup::Data,      "Projection Matrix", "ProjMatrix"},
    };

    return cNODE_DATA[(size_t)type];
}

static const std::unordered_map<NodeGroup, const std::vector<NodeType>> gNODE_GROUPS = []() {
    std::unordered_map<NodeGroup, std::vector<NodeType>> groups{};
    for (int i = 0; i < (int)NodeGroup::Max; i++)
        groups.emplace((NodeGroup)i, std::vector<NodeType>());

    for (int i = 0; i < (int)NodeType::Max; i++) {
        NodeGroup group = getNodeData((NodeType)i).group;
        if (group != NodeGroup::Undefined)
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
