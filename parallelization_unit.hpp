#pragma once

#include "thread_safe_container.hpp"
#include "thread_status.hpp"

enum class PARALLELIZATION_BASE : uint8_t { STD_THREAD, STD_FUTURE, STL_ALGORITHMS };

template<PARALLELIZATION_BASE base>
class ParallelExecutor;

template<class ActionReturnType, PARALLELIZATION_BASE Base>
class ParallelizationUnit {
    friend class ParallelExecutor<Base>;

    using ThreadStatusType = ThreadStatus<ActionReturnType>;
    using ThreadStatusTypeShrPtr = std::shared_ptr<ThreadStatusType>;

    size_t _threads_quantity{};
    std::atomic<uint8_t> _active_threads_counter{};
    ThreadSafeVector<ThreadStatusTypeShrPtr> _threads{};

    template<class ActionType>
    void _RunThreadsByStdThread(ActionType&& action) {
        for (size_t i{ 0 }; i < _threads_quantity; ++i) {
            ThreadStatusTypeShrPtr tsPtr = _threads.emplace_back(std::make_shared<ThreadStatusType>());
            std::thread(action, std::move(tsPtr)).detach();
        }
    }

    template<class ActionType>
    void _RunThreadsByStdAsync(ActionType&& action) {
        for (size_t i{ 0 }; i < _threads_quantity; ++i) {
            ThreadStatusTypeShrPtr tsPtr = _threads.emplace_back(std::make_shared<ThreadStatusType>());
            std::async(std::launch::async, action, std::move(tsPtr));
        }
    }

    template<class ActionType>
    void _RunThreadsByStdAlgorithm(ActionType&& action) {
        for (size_t i{ 0 }; i < _threads_quantity; ++i)
            _threads.emplace_back(std::make_shared<ThreadStatusType>());
        std::thread([&]() { std::for_each(std::execution::par, _threads.begin(), _threads.end(), action); }).detach();
    }

    void _WaitWhileAllLaunched() {
        for (auto it_b = _threads.begin(), it_e = _threads.end();
             std::find_if_not(it_b, it_e, [](const ThreadStatusTypeShrPtr&st_ptr) { return st_ptr->th_id.has_value(); }) != it_e;
             /* INFINITY */)
            std::this_thread::yield();
    }

    template<class ActionType>
    ParallelizationUnit(const size_t threads_quantity, ActionType&& action)
        : _threads_quantity(threads_quantity) {
        _threads.reserve(_threads_quantity);

        auto target_action = [&action, this](const ThreadStatusTypeShrPtr& ts_ptr) {
            ++this->_active_threads_counter;
            ts_ptr->th_id = std::this_thread::get_id();
            ts_ptr->result = action();
            ts_ptr->is_finished = true;
            --this->_active_threads_counter;
        };

        if constexpr (Base == PARALLELIZATION_BASE::STD_THREAD)
            _RunThreadsByStdThread(std::move(target_action));
        else if constexpr (Base == PARALLELIZATION_BASE::STD_FUTURE)
            _RunThreadsByStdAsync(std::move(target_action));
        else if constexpr (Base == PARALLELIZATION_BASE::STL_ALGORITHMS)
            _RunThreadsByStdAlgorithm(std::move(target_action));
        else
            static_assert(false, "target palatalization type is not implemented");

        _WaitWhileAllLaunched();
    }

    public:
    ParallelizationUnit(ParallelizationUnit&&) noexcept = default;

    size_t GetLaunchedThreads() const {
        return _threads.size();
    }

    size_t GetActiveThreads() const {
        size_t result{};
        for (const auto& ts_ptr : _threads)
            if (!static_cast<bool>(ts_ptr->is_finished))
                ++result;
        return result;
    }

    template<uint8_t milliseconds = 0ui8>
    void WaitWhileAllFinished() const {
        while (static_cast<uint8_t>(_active_threads_counter) > 0ui8)
            if constexpr (milliseconds == 0ui8)
                std::this_thread::yield();
            else
                std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
    }
};
