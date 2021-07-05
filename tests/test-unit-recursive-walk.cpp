#include "tests/test-unit-common.hpp"

#include "recursive_walk.hpp"

class RecursiveWalkingTesting : public testing::Test {
    public:
    using SetID = std::set<std::thread::id>;

    private:
    static std::optional<fs::path> _test_directory;
    static size_t _test_dirrectory_size;

    private:
    std::mutex cout_mutex;
    SetID thread_ids;

#pragma region testing::Test
    public:
    // Sets up the stuff shared by all tests in this test suite.
    //
    // Google Test will call Foo::SetUpTestSuite() before running the first
    // test in test suite Foo.  Hence a sub-class can define its own
    // SetUpTestSuite() method to shadow the one defined in the super
    // class.
    static void SetUpTestSuite() {
        std::srand(std::time(nullptr));
        CreateTestCatalogsTree(GetDeep(), 5 /*catalogs*/, 5 /*files*/);
    }

    // Tears down the stuff shared by all tests in this test suite.
    //
    // Google Test will call Foo::TearDownTestSuite() after running the last
    // test in test suite Foo.  Hence a sub-class can define its own
    // TearDownTestSuite() method to shadow the one defined in the super
    // class.
    static void TearDownTestSuite() {
        if (_test_directory.has_value())
            fs::remove_all(_test_directory.value());
    }

    // Tears down the test fixture.
    virtual void TearDown() override {
        for (auto [i, it] = std::make_tuple(1ui32, thread_ids.begin()); i <= thread_ids.size(); ++i, ++it)
            std::cout << "\n" << i << ") " << *it;
        std::cout << std::endl;
    }
#pragma endregion testing::Test

#pragma region target
    private:
    static void _CreateTestCatalog(const fs::path& dir, size_t deep, size_t catalogs, size_t files) {
        static const std::vector<std::string> extensions{ ".txt", ".json", ".jaml", ".md" };
        if (deep > 0) {
            for (size_t i = 1; i <= catalogs; i++) {
                fs::path sub_dir = fs::path(dir).append(std::string("sub_dir_").append(std::to_string(i)));
                fs::create_directory(sub_dir);
                _CreateTestCatalog(sub_dir, deep - 1, catalogs, files);
            }
        }

        for (size_t i = 1; i <= files; ++i) {
            fs::path file_path = fs::path(dir).append(std::string("file_").append(std::to_string(i)).append(extensions[std::rand() % extensions.size()]));
            std::fstream(file_path.string(), std::ios_base::app).close();
        }
    }

    public:
    static size_t GetDeep() {
        return 5;
    }

    /**
    * brief: creates catalogs-tree-structure
    * param: deep - the depth of a last sub catalog from the root catalog
    * param: catalogs - quantity of sub catalogs for all catalogs in tree
    * param: files - quantity of files for all catalogs in tree
    */
    static void CreateTestCatalogsTree(size_t deep, size_t catalogs, size_t files) {
        _test_directory = fs::current_path().append("test_directory");
        const fs::path& dir = _test_directory.value();
        _test_dirrectory_size = dir.string().size();
        fs::create_directory(dir);
        _CreateTestCatalog(dir, deep, catalogs, files);
    }

    fs::path GetTestDirectory() const {
        if (!_test_directory.has_value())
            throw std::exception("test directory isn't created");
        return _test_directory.value();
    }

    void PrintingTreeCatalogs(size_t deep, const fs::path& full_file_path) {
        if (full_file_path.extension().string().compare(".json") == 0) {
            //cout_mutex.lock();
            //std::thread::id current_id = std::this_thread::get_id();
            ///*std::cout << std::string(deep != 0 ? deep * 4 - 1 : 0, '-') << '>' << full_file_path.string().replace(0, _test_dirrectory_size, "")
            //          << " id: " << current_id << std::endl;*/
            //thread_ids.emplace(std::move(current_id));
            //cout_mutex.unlock();
        }
    }
#pragma endregion target
}; // class RecursiveWalkingTesting

std::optional<fs::path> RecursiveWalkingTesting::_test_directory{};
size_t RecursiveWalkingTesting::_test_dirrectory_size{};

// NOTE: walk on length tests

TEST_F(RecursiveWalkingTesting, WalkTestOnLenght_STD_THREAD) {
    RecursiveWalking<BASED_OF::STD_THREAD>(GetDeep(), WALK_TYPE::LENGTH).WalkIn(GetTestDirectory(), [&](size_t deep, const fs::path& dir) {
        PrintingTreeCatalogs(deep, dir);
    });
}

TEST_F(RecursiveWalkingTesting, WalkTestOnLenght_STD_FUTURE) {
    RecursiveWalking<BASED_OF::STD_FUTURE>(GetDeep(), WALK_TYPE::LENGTH).WalkIn(GetTestDirectory(), [&](size_t deep, const fs::path& dir) {
        PrintingTreeCatalogs(deep, dir);
    });
}

TEST_F(RecursiveWalkingTesting, WalkTestOnLenght_STL_ALGORITHMS) {
    RecursiveWalking<BASED_OF::STL_ALGORITHMS>(GetDeep(), WALK_TYPE::LENGTH).WalkIn(GetTestDirectory(), [&](size_t deep, const fs::path& dir) {
        PrintingTreeCatalogs(deep, dir);
    });
}

// NOTE: walk on width tests

TEST_F(RecursiveWalkingTesting, WalkTestOnWidth_STD_THREAD) {
    RecursiveWalking<BASED_OF::STD_THREAD>(GetDeep(), WALK_TYPE::WIDTH).WalkIn(GetTestDirectory(), [&](size_t deep, const fs::path& dir) {
        PrintingTreeCatalogs(deep, dir);
    });
}

TEST_F(RecursiveWalkingTesting, WalkTestOnWidth_STD_FUTURE) {
    RecursiveWalking<BASED_OF::STD_FUTURE>(GetDeep(), WALK_TYPE::WIDTH).WalkIn(GetTestDirectory(), [&](size_t deep, const fs::path& dir) {
        PrintingTreeCatalogs(deep, dir);
    });
}

TEST_F(RecursiveWalkingTesting, WalkTestOnWidth_STL_ALGORITHMS) {
    RecursiveWalking<BASED_OF::STL_ALGORITHMS>(GetDeep(), WALK_TYPE::WIDTH).WalkIn(GetTestDirectory(), [&](size_t deep, const fs::path& dir) {
        PrintingTreeCatalogs(deep, dir);
    });
}
