#pragma once

#include "stdafx.hpp"
#include "parallel_executor.hpp"

enum class WALK_TYPE : uint8_t { LENGTH, WIDTH };

template<PARALLELIZATION_BASE Base = PARALLELIZATION_BASE::STL_ALGORITHMS>
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
        ThreadSafeList<UnchekedDirectory> unchecked_directories{ { 0, initial_dir } };

        // TODO: DF
        std::atomic<uint64_t> _counter{ 0 };
        // TODO: DT
        std::atomic<uint8_t> walker_counter{ 0 };
        auto Walker = [&](ThreadSafeList<UnchekedDirectory>& shared_list_unchecked_directories) -> bool {
            while (true) {
                // TODO: DF
                ++_counter;
                // TODO: DT
                if (std::optional<UnchekedDirectory> unchecked_directory{ shared_list_unchecked_directories.ExtractFront() }; unchecked_directory.has_value()) {
                    ++walker_counter;
                    if (auto [current_deep, current_dir] = unchecked_directory.value(); current_deep <= _deep) {
                        for (const fs::directory_entry& sub_dir : fs::directory_iterator(current_dir, fs::directory_options::skip_permission_denied)) {
                            if (!sub_dir.is_directory()) {
                                if (action.has_value())
                                    action.value()(current_deep, sub_dir.path());
                            } else {
                                UnchekedDirectory unchecked_element = std::make_tuple(current_deep + 1, sub_dir);
                                if (_type == WALK_TYPE::LENGTH) {
                                    shared_list_unchecked_directories.EmplaceFront(std::move(unchecked_element));
                                } else if (_type == WALK_TYPE::WIDTH) {
                                    shared_list_unchecked_directories.EmplaceBack(std::move(unchecked_element));
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
                    return true;
                }
            }
        };

        ParallelExecutor<Base>{ _thread_quantity }.Launch(Walker, unchecked_directories).WaitWhileAllFinished<1000>();
    }
};
