#pragma once

#include "stdafx.hpp"

enum class WALK_TYPE : uint8_t { LENGTH, WIDTH };

class RecursiveWalking {
    size_t _deep;
    WALK_TYPE _type;
    size_t _thread_quantity;

    public:
    RecursiveWalking(
        size_t deep = SIZE_MAX,
        WALK_TYPE type = WALK_TYPE::LENGTH,
        size_t thread_quantity = std::max<size_t>(2 /*at least two threads will running*/, std::thread::hardware_concurrency()));

    void WalkIn(const fs::path& catalog, std::optional<std::function<void(size_t /*deep*/, const fs::path& /*full_file_path*/)>> action = std::nullopt);
};
