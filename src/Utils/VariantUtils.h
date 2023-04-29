#pragma once
#include <any>
#include <iostream>
#include <variant>
#include <vector>

/**
 * @brief Utility struct for handling a std::visit with multiple different type-dependant overloads.
 * @code
 * // Example
 * std::variant&lt;float, int&gt; value = 0.0f;
 * std::visit(VisitOverload{
 *  [](float arg) { std::cout &lt;&lt; "Float\n"; },
 *  [](int arg) { std::cout &lt;&lt; "Integer\n"; },
 * }, value);
 * @endcode
 * @brief Source: https://en.cppreference.com/w/cpp/utility/variant/visit
 */
template<typename... Ts>
struct VisitOverload : Ts... { using Ts::operator()...; };
template<typename... Ts>
VisitOverload(Ts...) -> VisitOverload<Ts...>;

template<template<typename...> typename target_t, typename variant_t>
struct VariantCastWrapper;
template<template<typename...> typename target_t, typename... Types>
struct VariantCastWrapper<target_t, std::variant<Types...>> {
    using type = target_t<Types...>;
};

/**
 * @brief Utility for converting a std::variant to a different class with the same variadic template arguments.
 * @code
 * // Example
 * template&lt;typename... Ts&gt;
 * class MyClass;
 * typedef std::variant&lt;int, float&gt; my_variant_t;
 * typedef VariantCast&lt;MyClass, my_variant_t&gt my_class_t;
 * // Equivalent to:
 * typedef MyClass&lt;int, float&gt; my_class_t;
 * @endcode
 */
template<template<typename...> typename target_t, typename variant_t>
using VariantCast = typename VariantCastWrapper<target_t, variant_t>::type;

template<typename... Ts>
struct VariantVectorWrapper;
template<typename... Ts>
struct VariantVectorWrapper<std::variant<Ts...>> {
    using type = std::variant<std::vector<Ts>...>;
};

/**
 * @brief Utility for converting a std::variant to an std::variant of std::vector with one std::vector per variadic
 * template type.
 * @code
 * // Example
 * typedef std::variant&lt;int, float&gt; my_variant_t;
 * tyepdef VectorVariant&lt;my_variant_t&gt; my_vector_t;
 * // Equivalent to:
 * typedef std::variant&lt;std::vector&lt;int&gt;, std::vector&lt;float&gt;&gt; my_vector_t;
 * @endcode
 */
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

/**
 * @brief Cast the given std::any to a std::variant<Types...>.
 * @brief If the real value of any is not one of Types, a <code>std::runtime_error</code> will be thrown.
 * @tparam Types
 * @param any
 * @return
 */
template <typename... Types>
std::variant<Types...> anyToVariant(const std::any& any) {
    return AnyToVariantWrapper<std::variant<Types...>, Types...>::convert(any);
}
