#pragma once

#include "stdafx.hpp"

enum class WALK_TYPE : uint8_t { LENGTH, WIDTH };

void RecursiveWalking(const fs::path& initial_dir, size_t deep = SIZE_MAX, WALK_TYPE type = WALK_TYPE::LENGTH);
