#pragma once

#include "parallelization_unit.hpp"

template<PARALLELIZATION_BASE Base>
class ParallelExecutor {
    size_t _parallel_threads_quantity;

    public:
    ParallelExecutor(const size_t parallel_threads_quantity = std::thread::hardware_concurrency())
        : _parallel_threads_quantity{ parallel_threads_quantity } {
        if (_parallel_threads_quantity == 0)
            throw std::exception("quantity of active threads must be greater then zero");
    }

    template<class FunctionType, class... ArgsTypes>
    decltype(auto) Launch(FunctionType&& function, ArgsTypes&&... args) {
        using FunctionReturnType = std::result_of_t<FunctionType && (ArgsTypes && ...)>;

        auto action = std::bind(function, std::forward<ArgsTypes>(args)...);
        return ParallelizationUnit<FunctionReturnType, Base>(_parallel_threads_quantity, std::move(action));
    }
};

using ParallelExecutorTHD = ParallelExecutor<PARALLELIZATION_BASE::STD_THREAD>;
using ParallelExecutorFUT = ParallelExecutor<PARALLELIZATION_BASE::STD_FUTURE>;
using ParallelExecutorSLT = ParallelExecutor<PARALLELIZATION_BASE::STL_ALGORITHMS>;
