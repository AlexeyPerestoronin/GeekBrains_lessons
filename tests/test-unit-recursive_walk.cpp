#include "tests/test-unit-common.hpp"

#include "recursive_walk.hpp"

class RecursiveWalkingTesting : public testing::Test {
    public:
    using SetID = std::set<std::thread::id>;
    using Action = std::function<void(size_t, const fs::path)>;

    private:
    static std::optional<fs::path> _test_directory;
    static size_t _test_dirrectory_size;
    std::mutex _cout_mutex;
    SetID _thread_ids;

    public:
    Action action_with_file;
    Action action_with_dir;

#pragma region testing::Test
    public:
    void SetUp() override {
        action_with_file = std::bind(&RecursiveWalkingTesting::PrintingTree<'-'>, this, std::placeholders::_1, std::placeholders::_2);
        action_with_dir = std::bind(&RecursiveWalkingTesting::PrintingTree<'*'>, this, std::placeholders::_1, std::placeholders::_2);
    }

    static void SetUpTestSuite() {
        std::srand(std::time(nullptr));
        CreateTestCatalogsTree(GetDeep(), 5 /*catalogs*/, 5 /*files*/);
    }

    static void TearDownTestSuite() {
        if (_test_directory.has_value())
            fs::remove_all(_test_directory.value());
    }

    void TearDown() override {
        for (auto [i, it] = std::make_tuple(1ui32, _thread_ids.begin()); i <= _thread_ids.size(); ++i, ++it)
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
        return 2;
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

    template<char placeholder>
    void PrintingTree(size_t deep, const fs::path& full_file_path) {
        std::thread::id current_id = std::this_thread::get_id();
        {
            std::lock_guard locker(_cout_mutex);
            std::cout << std::string(deep != 0 ? deep * 4 - 1 : 0, placeholder) << '>' << full_file_path.string().replace(0, _test_dirrectory_size, "")
                      << " id: " << current_id << std::endl;
        }
        _thread_ids.emplace(std::move(current_id));
    }
#pragma endregion target
}; // class RecursiveWalkingTesting

std::optional<fs::path> RecursiveWalkingTesting::_test_directory{};
size_t RecursiveWalkingTesting::_test_dirrectory_size{};

TEST_F(RecursiveWalkingTesting, WalkTestOnLenght_STD_THREAD) {
    RecursiveWalking<WALK_TYPE::LENGTH, PARALLELIZATION_BASE::STD_THREAD>(GetDeep()).WalkIn(GetTestDirectory(), action_with_file, action_with_dir);
}

TEST_F(RecursiveWalkingTesting, WalkTestOnLenght_STD_FUTURE) {
    RecursiveWalking<WALK_TYPE::LENGTH, PARALLELIZATION_BASE::STD_FUTURE>(GetDeep()).WalkIn(GetTestDirectory(), action_with_file, action_with_dir);
}

TEST_F(RecursiveWalkingTesting, WalkTestOnLenght_STL_ALGORITHMS) {
    RecursiveWalking<WALK_TYPE::LENGTH, PARALLELIZATION_BASE::STL_ALGORITHMS>(GetDeep()).WalkIn(GetTestDirectory(), action_with_file, action_with_dir);
}

// NOTE: walk on width tests

TEST_F(RecursiveWalkingTesting, WalkTestOnWidth_STD_THREAD) {
    RecursiveWalking<WALK_TYPE::WIDTH, PARALLELIZATION_BASE::STD_THREAD>(GetDeep()).WalkIn(GetTestDirectory(), action_with_file, action_with_dir);
}

TEST_F(RecursiveWalkingTesting, WalkTestOnWidth_STD_FUTURE) {
    RecursiveWalking<WALK_TYPE::WIDTH, PARALLELIZATION_BASE::STD_FUTURE>(GetDeep()).WalkIn(GetTestDirectory(), action_with_file, action_with_dir);
}

TEST_F(RecursiveWalkingTesting, WalkTestOnWidth_STL_ALGORITHMS) {
    RecursiveWalking<WALK_TYPE::WIDTH, PARALLELIZATION_BASE::STL_ALGORITHMS>(GetDeep()).WalkIn(GetTestDirectory(), action_with_file, action_with_dir);
}
