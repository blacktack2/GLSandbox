#pragma once
#include <variant>
#include <vector>

/**
 * @brief Utility struct for handling an std::visit with multiple different type-dependant overloads.
 * @code
 * // Example
 * std::variant<float, int> value = 0.0f;
 * std::visit(VisitOverload{
 *  [](float arg) { std::cout << "Float\n"; },
 *  [](int arg) { std::cout << "Integer\n"; },
 * }, value);
 * @endcode
 * @brief Source: https://en.cppreference.com/w/cpp/utility/variant/visit
 */
template<typename... Ts>
struct VisitOverload : Ts... { using Ts::operator()...; };
template<typename... Ts>
VisitOverload(Ts...) -> VisitOverload<Ts...>;

template <typename... Ts>
struct VariantVectorWrapper;
template <typename... Ts>
struct VariantVectorWrapper<std::variant<Ts...>> {
    using type = std::variant<std::vector<Ts>...>;
};
template <typename T>
using VectorVariant = typename VariantVectorWrapper<T>::type;
