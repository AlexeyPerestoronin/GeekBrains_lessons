#pragma once

#include "stdafx.hpp"

template<class ResultType>
struct ThreadStatus {
    std::optional<std::thread::id> th_id{ std::nullopt };
    std::optional<ResultType> result{ std::nullopt };
    std::atomic_bool is_finished{ false };

    ThreadStatus() = default;

    // NOTE: any instance of this struct is impossible to copy
    ThreadStatus(ThreadStatus&&) = delete;
    ThreadStatus(const ThreadStatus&) = delete;
    ThreadStatus& operator=(const ThreadStatus&) = delete;
};
