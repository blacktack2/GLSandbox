#include "MathNodes.h"

ArithmeticNode::ArithmeticNode() : Node("Arithmetic") {
    mValueBIn = nullptr;
    mValueOut = nullptr;
    addPort(mValueAIn);

    mValueAIn.addOnLinkEvent([this]() {
        std::visit([this](const auto& arg) {
            using port_type = std::decay_t<decltype(arg)>;
            mValueBIn = std::make_unique<Port<port_type>>(*this, IPort::Direction::In, "ValueBIn", "B");
            addPort(*mValueBIn);
            mValueBIn->addOnUpdateEvent([this]() {
                mValueOut->valueUpdated();
            });
            mValueOut = std::make_unique<Port<port_type>>(*this, IPort::Direction::Out, "ValueOut", "Result",
                [&]() { return std::visit(VisitOverload{
                    [](const port_type& arg2)->port_type { return arg2; },
                    [](const auto& arg2)->port_type { return port_type{}; },
                }, calculateValue()); });
            addPort(*mValueOut);
        }, mValueAIn.getLinkedValue());
    });
    mValueAIn.addOnUnlinkEvent([this]() {
        removePort(*mValueBIn);
        mValueBIn = nullptr;
        removePort(*mValueOut);
        mValueOut = nullptr;
    });
    mValueAIn.addOnUpdateEvent([this]() {
        if (mValueOut)
            mValueOut->valueUpdated();
    });
}

std::vector<std::pair<std::string, std::string>> ArithmeticNode::generateSerializedData() const {
    std::vector<std::pair<std::string, std::string>> data{};
    data.emplace_back("Operation", SerializationUtils::serializeData((int&)mCurrentOperation));
    return data;
}

void ArithmeticNode::deserializeData(const std::string& dataID, std::ifstream& stream) {
    if (dataID == "Operation")
        SerializationUtils::deserializeData(stream, (int&)mCurrentOperation);
}

void ArithmeticNode::drawContents() {
    ImUtils::cycleButton(generateNodeLabelID("Combo"), (size_t&)mCurrentOperation, (size_t)Operation::Max,
                         [](size_t index) { return getOperationLabel((Operation)index); });
}

ArithmeticNode::numeric_variant_t ArithmeticNode::calculateValue() {
    if (!mValueAIn.isLinked() || !mValueBIn || !mValueBIn->isLinked())
        return 0;

    return std::visit([this](const auto& valueA)->numeric_variant_t {
        using value_type = std::decay_t<decltype(valueA)>;
        value_type valueB = std::visit([](const auto& arg) { return arg; },
            dynamic_cast<Port<value_type>*>(mValueBIn.get())->getLinkedValue());
        return operate(mCurrentOperation, valueA, valueB);
    }, mValueAIn.getLinkedValue());
}
