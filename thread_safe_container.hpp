#pragma once

#include "stdafx.hpp"

template<class ElementType, template<class> class ContainerType>
class ThreadSafeContainer : protected ContainerType<ElementType> {
#pragma region inner types and aliases
    using BaseType = ContainerType<ElementType>;
    using OptElementType = std::optional<ElementType>;

    template<template<class> class _ContainerType>
    using IsBasedOf = std::is_same<BaseType, _ContainerType<ElementType>>;

    template<template<class> class _ContainerType>
    static constexpr bool IsBasedOf_V = IsBasedOf<_ContainerType>::value;
#pragma endregion inner types and aliases

    std::shared_ptr<std::mutex> _access_mutex_ptr{};

#define GET_LOCK std::lock_guard _lock(*_access_mutex_ptr.get());

    public:
#pragma region constructors / destructor
    ThreadSafeContainer(std::initializer_list<ElementType> args)
        : BaseType(args)
        , _access_mutex_ptr{ std::make_shared<std::mutex>() } {}

    template<class... ArgsTypes>
    ThreadSafeContainer(ArgsTypes&&... args)
        : BaseType(std::forward<ArgsTypes>(args)...)
        , _access_mutex_ptr{ std::make_shared<std::mutex>() } {}

    virtual ~ThreadSafeContainer() {
        // TODO: breakpoint ...
    }
#pragma endregion constructors / destructor

    OptElementType ExtractFront() noexcept {
        GET_LOCK
        OptElementType result;

        if (BaseType::empty())
            return result;

        if constexpr (IsBasedOf_V<std::list>) {
            result = BaseType::front();
            BaseType::pop_front();
        } else if constexpr (IsBasedOf_V<std::vector>) {
            result = BaseType::front();
            BaseType::erase(BaseType::begin());
        } else {
            static_assert(false, "ExtractFront-method cannot be used with currently defined ContainerType-type");
        }
        return result;
    }

    template<class... ArgsTypes>
    decltype(auto) emplace_front(ArgsTypes&&... args) {
        GET_LOCK
        ElementType new_element(std::forward<ArgsTypes>(args)...);
        if constexpr (IsBasedOf_V<std::list>) {
            return BaseType::emplace_front(std::move(new_element));
        } else if constexpr (IsBasedOf_V<std::vector>) {
            return BaseType::insert(BaseType::begin(), std::move(new_element));
        } else {
            static_assert(false, "emplace_front-method cannot be used with currently defined ContainerType-type");
        }
    }

    template<class... ArgsTypes>
    decltype(auto) emplace_back(ArgsTypes&&... args) {
        GET_LOCK
        if constexpr (IsBasedOf_V<std::list> || IsBasedOf_V<std::vector>) {
            return BaseType::emplace_back(std::forward<ArgsTypes>(args)...);
        } else {
            static_assert(false, "emplace_back-method cannot be used with currently defined ContainerType-type");
        }
    }

#pragma region iterator(s)
    // TODO: (?) is it need implement thread-safe iterator-class

    typename BaseType::iterator begin() noexcept {
        GET_LOCK
        return BaseType::begin();
    }

    typename BaseType::const_iterator begin() const noexcept {
        GET_LOCK
        return BaseType::begin();
    }

    typename BaseType::iterator end() noexcept {
        GET_LOCK
        return BaseType::end();
    }

    typename BaseType::const_iterator end() const noexcept {
        GET_LOCK
        return BaseType::end();
    }
#pragma endregion iterator(s)

    size_t size() const {
        GET_LOCK
        return BaseType::size();
    }

    void reserve(size_t reserved_size) {
        GET_LOCK
        if constexpr (IsBasedOf_V<std::vector>)
            BaseType::reserve(reserved_size);
    }

#undef GET_LOCK;
};

template<class ElementType>
using ThreadSafeList = ThreadSafeContainer<ElementType, std::list>;

template<class ElementType>
using ThreadSafeVector = ThreadSafeContainer<ElementType, std::vector>;
