#include "tests/test-unit-common.hpp"

#include "parallel_executor.hpp"

auto& main_cout = std::cout;

template<PARALLELIZATION_BASE Base>
class ParallelExecutor_Tests : public testing::Test {
    public:
    void Test1() {
        std::atomic<uint32_t> counter{};
        ThreadSafeList<int32_t> int_list{};
        auto Func = [](std::atomic<uint32_t>* const io_counter, ThreadSafeList<int32_t>* const io_int_list_ref) -> bool {
            int32_t local_counter{ 0 };
            while (++local_counter < INT8_MAX) {
                io_int_list_ref->emplace_back(std::rand());
                ++(*io_counter);
            }
            return true;
        };

        size_t threads_quantity{ 32 };

        ParallelExecutor<Base> pe(threads_quantity);
        auto unit = pe.Launch(Func, &counter, &int_list);
        unit.WaitWhileAllFinished<1000>();

        ASSERT_EQ(unit.GetActiveThreads(), 0);
        ASSERT_EQ(unit.GetLaunchedThreads(), threads_quantity);
        ASSERT_EQ(static_cast<size_t>(counter), int_list.size());
    }

    void Test2() {
        std::atomic<uint32_t> counter{};
        ThreadSafeList<int32_t> int_list{};
        auto Func = [&]() -> bool {
            int32_t local_counter{ 0 };
            while (++local_counter < INT8_MAX) {
                int_list.emplace_back(std::rand());
                ++counter;
            }
            return true;
        };

        size_t threads_quantity{ 32 };

        ParallelExecutor<Base> pe(threads_quantity);
        auto unit = pe.Launch(Func);
        unit.WaitWhileAllFinished<1000>();

        ASSERT_EQ(unit.GetActiveThreads(), 0);
        ASSERT_EQ(unit.GetLaunchedThreads(), threads_quantity);
        ASSERT_EQ(static_cast<size_t>(counter), int_list.size());
    }

    void LaunchAllTests() {
        Test1();
        Test2();
    }
};

using PE_ByThreads = ParallelExecutor_Tests<PARALLELIZATION_BASE::STD_THREAD>;
TEST_F(PE_ByThreads, Test) {
    LaunchAllTests();
}

using PE_ByFuture = ParallelExecutor_Tests<PARALLELIZATION_BASE::STD_FUTURE>;
TEST_F(PE_ByFuture, Test) {
    LaunchAllTests();
}

using PE_ByAlg = ParallelExecutor_Tests<PARALLELIZATION_BASE::STD_FUTURE>;
TEST_F(PE_ByAlg, Test) {
    LaunchAllTests();
}