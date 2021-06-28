#pragma once

#include "stdafx.hpp"

template<class ElementType>
class ThreadSafeList : protected std::list<ElementType> {
    using BaseType = std::list<ElementType>;
    using OptElementType = std::optional<ElementType>;

    std::mutex _access{};
#define GET_LOCK std::lock_guard _lock(_access);

    public:
    ThreadSafeList(std::initializer_list<ElementType> args)
        : BaseType(args){};

    template<class... ArgsTypes>
    ThreadSafeList(ArgsTypes&&... args)
        : BaseType(std::forward<ArgsTypes>(args)...) {}

    OptElementType ExtractFront() noexcept {
        GET_LOCK
        OptElementType result;
        if (!BaseType::empty()) {
            result = BaseType::front();
            BaseType::pop_front();
        }
        return result;
    }

    template<class... ArgsTypes>
    decltype(auto) emplace_front(ArgsTypes&&... args) {
        GET_LOCK
        return BaseType::emplace_front(std::forward<ArgsTypes>(args)...);
    }

    template<class... ArgsTypes>
    decltype(auto) emplace_back(ArgsTypes&&... args) {
        GET_LOCK
        return BaseType::emplace_back(std::forward<ArgsTypes>(args)...);
    }

#undef GET_LOCK;
};