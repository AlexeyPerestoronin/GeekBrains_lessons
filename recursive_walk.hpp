#pragma once

#include "stdafx.hpp"

enum class WALK_TYPE : uint8_t { LENGTH, WIDTH };

class RecursiveWalking {
    size_t _deep;
    WALK_TYPE _type;

    public:
    RecursiveWalking(size_t deep = SIZE_MAX, WALK_TYPE type = WALK_TYPE::LENGTH);

    void WalkIn(const fs::path& catalog, std::optional<std::function<void(size_t /*deep*/, const fs::path& /*full_file_path*/)>> action = std::nullopt);
};
