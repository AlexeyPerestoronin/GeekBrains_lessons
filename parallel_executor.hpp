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

    // brief: parallelization unit created of an instance of ParallelExecutor-class
    // t-param: IsSafeMode - flag to control behaviour of destructor of the instance of ParallelizationUnit-class
    // t-param: FunctionType - data-type of callable object that must be launch in parallel mode by ParallelizationUnit-class instance
    // t-param: ArgsTypes - data-types parameter pack with which target callable object must be launch
    // note: IsSafeMode-template-option description
    // | If you extremely sure in quality of the callable object that you sent inside this class (see FunctionType-template-parameter),
    // | you can not wait when all launched threads finish, in other case you must wait while they finished for avoided fatal terminate error.
    template<bool IsSafeMode = true, class FunctionType, class... ArgsTypes>
    decltype(auto) Launch(FunctionType&& function, ArgsTypes&&... args) {
        auto action = std::bind(function, std::forward<ArgsTypes>(args)...);
        return ParallelizationUnit<IsSafeMode, Base, decltype(action)>(_parallel_threads_quantity, std::move(action));
    }
};

using ParallelExecutorTHD = ParallelExecutor<PARALLELIZATION_BASE::STD_THREAD>;
using ParallelExecutorFUT = ParallelExecutor<PARALLELIZATION_BASE::STD_FUTURE>;
using ParallelExecutorSLT = ParallelExecutor<PARALLELIZATION_BASE::STL_ALGORITHMS>;
