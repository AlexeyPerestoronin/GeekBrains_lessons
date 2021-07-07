#pragma once

#include "stdafx.hpp"
#include "parallel_executor.hpp"

enum class WALK_TYPE : uint8_t { LENGTH, WIDTH };

template<PARALLELIZATION_BASE Base = PARALLELIZATION_BASE::STL_ALGORITHMS>
class RecursiveWalking {
    using WalkerCounter = std::atomic_uint8_t;
    using ActionType = std::function<void(size_t /*deep*/, const fs::path& /*full_file_path*/)>;
    using OptActionType = std::optional<ActionType>;
    using UnchekedDirectory = std::tuple<size_t, fs::directory_entry>;
    using ListUnchekedDirectory = ThreadSafeList<UnchekedDirectory>;

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

    void WalkIn(const fs::path& catalog, const OptActionType& action = std::nullopt) {
        fs::directory_entry initial_dir{ catalog };

        if (!static_cast<fs::directory_entry>(initial_dir).is_directory())
            throw std::exception("initial directory is not OS catalog");

        auto Walker = [](
                          size_t deep, WALK_TYPE type, WalkerCounter* counter_ptr /* pointer to shared counter_ptr of current active walk-job */,
                          ListUnchekedDirectory* slud_ptr /* pointer to shared list unchecked directories */, const OptActionType* const action_ptr) -> bool {
            std::atomic_uint8_t& walker_counter = *counter_ptr;
            while (true) {
                if (std::optional<UnchekedDirectory> unchecked_directory{ slud_ptr->ExtractFront() }; unchecked_directory.has_value()) {
                    ++walker_counter;
                    auto [current_deep, current_dir] = unchecked_directory.value();
                    for (const fs::directory_entry& sub_dir : fs::directory_iterator(current_dir, fs::directory_options::skip_permission_denied)) {
                        if (!sub_dir.is_directory()) {
                            // TODO: too many checking of conditions
                            if (action_ptr && action_ptr->has_value())
                                action_ptr->value()(current_deep, sub_dir.path());
                        } else if (current_deep < deep) {
                            UnchekedDirectory unchecked_element = std::make_tuple(current_deep + 1, sub_dir);
                            if (type == WALK_TYPE::LENGTH) {
                                slud_ptr->emplace_front(std::move(unchecked_element));
                            } else if (type == WALK_TYPE::WIDTH) {
                                slud_ptr->emplace_back(std::move(unchecked_element));
                            } else {
                                throw std::exception("unknown type of walk through OS catalogs");
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

        WalkerCounter counter{};
        ListUnchekedDirectory unchecked_directories{ { 0, initial_dir } };
        ParallelExecutor<Base>{ _thread_quantity }.Launch(Walker, _deep, _type, &counter, &unchecked_directories, &action).WaitWhileAllFinished<1000>();
    }
};
