#pragma once

#include "stdafx.hpp"
#include "parallel_executor.hpp"

enum class WALK_TYPE : uint8_t { LENGTH, WIDTH };

template<WALK_TYPE Type = WALK_TYPE::WIDTH, PARALLELIZATION_BASE Base = PARALLELIZATION_BASE::STL_ALGORITHMS>
class RecursiveWalking {
    // clang-format off
    using WalkerCounter         = std::atomic_uint8_t;
    using UnchekedDirectory     = std::tuple<size_t, fs::directory_entry>;
    using ListUnchekedDirectory = ThreadSafeList<UnchekedDirectory>;
    using ActionType            = std::function<void(size_t /*deep*/, const fs::path& /*full_file_path*/)>;
    using OptActionType         = std::optional<ActionType>;
    // clang-format on

    size_t _deep;
    size_t _thread_quantity;

    template<bool IsActionWithFile, bool IsActionWithDir>
    void _Walker(
        WalkerCounter& walker_counter,
        ListUnchekedDirectory& unchecked_directories,
        const ActionType* const action_with_file,
        const ActionType* const action_with_dir) {
        while (true) {
            if (std::optional<UnchekedDirectory> unchecked_directory{ unchecked_directories.ExtractFront() }; unchecked_directory.has_value()) {
                ++walker_counter;
                auto [current_deep, current_dir] = unchecked_directory.value();
                for (const fs::directory_entry& sub_dir : fs::directory_iterator(current_dir, fs::directory_options::skip_permission_denied)) {
                    if (!sub_dir.is_directory()) {
                        if constexpr (IsActionWithFile)
                            (*action_with_file)(current_deep, sub_dir.path());

                    } else if (current_deep < _deep) {
                        if constexpr (IsActionWithDir)
                            (*action_with_dir)(current_deep, sub_dir.path());

                        UnchekedDirectory unchecked_element = std::make_tuple(current_deep + 1, sub_dir);
                        if constexpr (Type == WALK_TYPE::LENGTH) {
                            unchecked_directories.emplace_front(std::move(unchecked_element));
                        } else if constexpr (Type == WALK_TYPE::WIDTH) {
                            unchecked_directories.emplace_back(std::move(unchecked_element));
                        } else {
                            static_assert(false, "unknown type of walk through OS catalogs");
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
    }

    public:
    RecursiveWalking(
        size_t deep = SIZE_MAX,
        size_t thread_quantity = std::max<size_t>(2 /*at least two threads will running*/, std::thread::hardware_concurrency()))
        : _deep{ deep }
        , _thread_quantity{ thread_quantity } {
        if (!_thread_quantity)
            throw std::exception("quantity of parallel threads must be greater then zero");
    }

    void WalkIn(const fs::path& catalog, const OptActionType& action_with_file = std::nullopt, const OptActionType& action_with_dir = std::nullopt) {
        fs::directory_entry initial_dir{ catalog };

        if (!static_cast<fs::directory_entry>(initial_dir).is_directory())
            throw std::exception("initial directory is not OS catalog");

        void (RecursiveWalking::*RealWalker)(WalkerCounter&, ListUnchekedDirectory&, const ActionType* const, const ActionType* const) = nullptr;
        if (bool is_f = action_with_file.has_value(), is_d = action_with_dir.has_value(); is_f && is_d)
            RealWalker = &RecursiveWalking::_Walker<true, true>;
        else if (is_f && !is_d)
            RealWalker = &RecursiveWalking::_Walker<true, false>;
        else if (!is_f && is_d)
            RealWalker = &RecursiveWalking::_Walker<false, true>;
        else
            throw std::exception("at least one action (with files or with directory) must be assigned");

        const ActionType* const action_file_ptr = action_with_file.has_value() ? &action_with_file.operator*() : nullptr;
        const ActionType* const action_dir_ptr = action_with_dir.has_value() ? &action_with_dir.operator*() : nullptr;
        ListUnchekedDirectory unchecked_directories{ { 0, initial_dir } };
        WalkerCounter walker_counter{};
        ParallelExecutor<Base>{ _thread_quantity }
            .Launch(RealWalker, this, std::ref(walker_counter), std::ref(unchecked_directories), action_file_ptr, action_dir_ptr)
            .WaitWhileAllFinished<10>();
    }
};
