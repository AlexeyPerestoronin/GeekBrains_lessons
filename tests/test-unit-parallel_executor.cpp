#include "tests/test-unit-common.hpp"

#include "parallel_executor.hpp"

auto& main_cout = std::cout;

//ThreadSafeList<int32_t> int_list{};

TEST(Test1, Simple) {
ThreadSafeList<int32_t> int_list{};
    auto Func = [&](int _1, double _2) -> bool {
        while (true) {
            int_list.EmplaceBack(std::rand());
            std::this_thread::sleep_for(std::chrono::milliseconds(std::rand() % 1000));
            main_cout << int_list.ExtractFront().value_or(-1);
        }
        return true;
    };

    ParallelExecutorTHD pe(2);
    pe.Launch(Func, 4, 2).WaitWhileAllFinished<1000>();
}