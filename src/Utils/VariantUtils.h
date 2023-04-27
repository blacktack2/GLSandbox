#pragma once
#include <any>
#include <iostream>
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

template<typename... Ts>
struct VariantVectorWrapper;
template<typename... Ts>
struct VariantVectorWrapper<std::variant<Ts...>> {
    using type = std::variant<std::vector<Ts>...>;
};
template<typename T>
using VectorVariant = typename VariantVectorWrapper<T>::type;

template <typename variant_t, typename... Types>
struct AnyToVariantWrapper;

template <typename variant_t, typename T, typename... Types>
struct AnyToVariantWrapper<variant_t, T, Types...> {
    static variant_t convert(const std::any& any) {
        if (auto ptr = std::any_cast<T>(&any)) {
            return variant_t{ *ptr };
        } else {
            return AnyToVariantWrapper<variant_t, Types...>::convert(any);
        }
    }
};

template <typename variant_t>
struct AnyToVariantWrapper<variant_t> {
    static variant_t convert(const std::any&) {
        throw std::runtime_error("Unsupported type");
    }
};

template <typename... Types>
std::variant<Types...> anyToVariant(const std::any& any) {
    return AnyToVariantWrapper<std::variant<Types...>, Types...>::convert(any);
}
