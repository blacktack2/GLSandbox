#pragma once

// Warning - Changing the ID of a NodeType may invalidate existing serialized graphs with that type.
enum class NodeType {
    Entry = 0,
    Exit  = 1,
    RenderPass = 100,

    Arithmetic = 200,

    Integer = 300,
    IVec2   = 301,
    IVec3   = 302,
    IVec4   = 303,

    Float = 320,
    Vec2  = 321,
    Vec3  = 322,
    Vec4  = 323,

    Mesh   = 400,
    Shader = 401,

    Max,

    Undefined = -1
};

enum class NodeGroup {
    Execution = 0,
    Maths,
    Numerics,
    Graphics,

    Max,

    Undefined = -1
};

static NodeGroup getNodeGroup(NodeType type) {
    switch (type) {
        case NodeType::Entry: case NodeType::RenderPass:
            return NodeGroup::Execution;
        case NodeType::Integer: case NodeType::Float: case NodeType::Vec2: case NodeType::Vec3: case NodeType::Vec4:
            return NodeGroup::Numerics;
        case NodeType::Arithmetic:
            return NodeGroup::Maths;
        case NodeType::Mesh: case NodeType::Shader:
            return NodeGroup::Graphics;
        default:
            return NodeGroup::Undefined;
    }
}

static const std::unordered_map<NodeGroup, const std::vector<NodeType>> gNODE_GROUPS = []() {
    std::unordered_map<NodeGroup, std::vector<NodeType>> groups;
    for (int i = 0; i < (int)NodeGroup::Max; i++)
        groups.emplace((NodeGroup)i, std::vector<NodeType>());

    for (int i = 0; i < (int)NodeType::Max; i++) {
        NodeGroup group = getNodeGroup((NodeType)i);
        if (group != NodeGroup::Undefined)
            groups.find(group)->second.emplace_back((NodeType)i);
    }

    std::unordered_map<NodeGroup, const std::vector<NodeType>> constGroups;
    for (const auto& group : groups)
        constGroups.emplace(group.first, group.second);
    return constGroups;
}();
