#pragma once

#include "stdafx.hpp"
#include "thread_safe_container.hpp"

enum class WALK_TYPE : uint8_t { LENGTH, WIDTH };

enum class BASED_OF : uint8_t { STD_THREAD, STD_FUTURE, STL_ALGORITHMS };

template<BASED_OF base = BASED_OF::STL_ALGORITHMS>
class RecursiveWalking {
    size_t _deep;
    WALK_TYPE _type;
    size_t _thread_quantity;

    public:
    RecursiveWalking(
        size_t deep = SIZE_MAX,
        WALK_TYPE type = WALK_TYPE::LENGTH,
        size_t thread_quantity = std::max<size_t>(2 /*at least two threads will running*/, std::thread::hardware_concurrency()))
        : _deep{ deep }
        , _type{ type }
        , _thread_quantity{ thread_quantity } {}

    void WalkIn(const fs::path& catalog, std::optional<std::function<void(size_t /*deep*/, const fs::path& /*full_file_path*/)>> action = std::nullopt) {
        fs::directory_entry initial_dir(static_cast<fs::directory_entry>(catalog));

        if (!static_cast<fs::directory_entry>(initial_dir).is_directory())
            throw std::exception("initial directory is not OS catalog");

        using UnchekedDirectory = std::tuple<size_t, fs::directory_entry>;
        ThreadSafeVector<UnchekedDirectory> unchecked_directories{ { 0, initial_dir } };

        std::atomic<uint8_t> walker_counter;
        auto Walker = [&](uint8_t /*stub*/ = 0ui8) {
            while (true) {
                if (std::optional<UnchekedDirectory> unchecked_directory{ unchecked_directories.ExtractFront() }; unchecked_directory.has_value()) {
                    ++walker_counter;
                    if (auto [current_deep, current_dir] = unchecked_directory.value(); current_deep <= _deep) {
                        for (const fs::directory_entry& sub_dir : fs::directory_iterator(current_dir, fs::directory_options::skip_permission_denied)) {
                            if (!sub_dir.is_directory()) {
                                if (action.has_value())
                                    action.value()(current_deep, sub_dir.path());
                            } else {
                                UnchekedDirectory unchecked_element = std::make_tuple(current_deep + 1, sub_dir);
                                if (_type == WALK_TYPE::LENGTH) {
                                    unchecked_directories.EmplaceFront(std::move(unchecked_element));
                                } else if (_type == WALK_TYPE::WIDTH) {
                                    unchecked_directories.EmplaceBack(std::move(unchecked_element));
                                } else {
                                    throw std::exception("unknown type of walk through OS catalogs");
                                }
                            }
                        }
                    }
                    --walker_counter;
                } else if (walker_counter) {
                    std::this_thread::yield();
                } else {
                    return;
                }
            }
        };

        if constexpr (base == BASED_OF::STL_ALGORITHMS) {
            // NOTE: STL-algorithms-parallelization based implementation
            std::vector<uint8_t> active_points(_thread_quantity, 0ui8);
            std::for_each(std::execution::par, active_points.begin(), active_points.end(), Walker);
        } else if constexpr (base == BASED_OF::STD_FUTURE) {
            // NOTE: std::future based implementation
            std::vector<std::future<void>> active_tasks;
            active_tasks.reserve(_thread_quantity);
            for (size_t i = 0; i < _thread_quantity; ++i)
                active_tasks.emplace_back(std::async(std::launch::async, Walker));
            for (size_t i = 0; i < _thread_quantity; ++i)
                active_tasks[i].get();
        } else if constexpr (base == BASED_OF::STD_THREAD) {
            // NOTE: std::thread based implementation
            std::vector<std::thread> active_threds;
            active_threds.reserve(_thread_quantity);
            for (size_t i = 0; i < _thread_quantity; ++i)
                active_threds.emplace_back(std::thread(Walker));
            for (size_t i = 0; i < _thread_quantity; ++i)
                active_threds[i].join();
        } else {
            static_assert(false, "undefined base for parallelization");
        }
    }
};
