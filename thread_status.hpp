#pragma once

#include "stdafx.hpp"

// NOTE: any instance of this struct is impossible to copy
#define THREAD_STATUS_BODY                      \
    ThreadStatus() = default;                   \
    ThreadStatus(ThreadStatus&&) = delete;      \
    ThreadStatus(const ThreadStatus&) = delete; \
    ThreadStatus& operator=(const ThreadStatus&) = delete;

template<class ResultType>
struct ThreadStatus {
    std::optional<std::thread::id> th_id{ std::nullopt };
    std::optional<ResultType> result{ std::nullopt };
    std::atomic_bool is_finished{ false };
    THREAD_STATUS_BODY
};

template<>
struct ThreadStatus<void> {
    std::optional<std::thread::id> th_id{ std::nullopt };
    std::atomic_bool is_finished{ false };
    THREAD_STATUS_BODY
};
