#include "tests/test-unit-common.hpp"

#include "recursive_walk.hpp"

TEST(SomeTest, TestTest) {
    { // negative
        ASSERT_THROW(RecursiveWalking("C:/Program Files (x85)", 2), std::exception);
    }

    { // positive
        std::cout << "\n\nDeep = 0:";
        RecursiveWalking("C:/Program Files (x86)", 0);

        std::cout << "\n\nDeep = 1:";
        RecursiveWalking("C:/Program Files (x86)", 1);

        std::cout << "\n\nDeep = 2:";
        RecursiveWalking("C:/Program Files (x86)", 2);

        std::cout << "\n\nDeep = 3:";
        RecursiveWalking("C:/Program Files (x86)", 3);
    }
}
