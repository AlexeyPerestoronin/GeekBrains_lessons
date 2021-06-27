#include "tests/test-unit-common.hpp"

#include "recursive_walk.hpp"

class RecursiveWalkingTesting : public testing::Test {
    std::vector<std::string> _extensions{ ".txt", ".json", ".jaml", ".md" };
    std::optional<fs::path> _test_directory;

    void _CreateTestCatalog(const fs::path& dir, size_t deep, size_t catalogs, size_t files) {
        if (deep > 0) {
            for (size_t i = 1; i <= catalogs; i++) {
                fs::path sub_dir = fs::path(dir).append(std::string("sub_dir_").append(std::to_string(i)));
                fs::create_directory(sub_dir);
                _CreateTestCatalog(sub_dir, deep - 1, catalogs, files);
            }
        }

        for (size_t i = 1; i <= files; ++i) {
            fs::path file_path = fs::path(dir).append(std::string("file_").append(std::to_string(i)).append(_extensions[std::rand() % _extensions.size()]));
            std::fstream(file_path.string(), std::ios_base::app).close();
        }
    }

    public: // testing::Test
    // Sets up the test fixture.
    virtual void SetUp() override {
        std::srand(std::time(nullptr));
    }

    // Tears down the test fixture.
    virtual void TearDown() override {
        if (_test_directory.has_value())
            fs::remove_all(_test_directory.value());
    }

    public:
    /**
    * brief: creates catalogs-tree-structure
    * param: deep - the depth of a last sub catalog from the root catalog
    * param: catalogs - quantity of sub catalogs for all catalogs in tree
    * param: files - quantity of files for all catalogs in tree
    */
    void CreateTestCatalogsTree(size_t deep, size_t catalogs, size_t files) {
        _test_directory = fs::current_path().append("test_directory");
        const fs::path& dir = _test_directory.value();
        fs::create_directory(dir);
        _CreateTestCatalog(dir, deep, catalogs, files);
    }

    fs::path GetTestDirectory() {
        if (!_test_directory.has_value())
            throw std::exception("test directory isn't created");
        return _test_directory.value();
    }
};

TEST_F(RecursiveWalkingTesting, WalkTestOnLenght) {
    size_t deep = 4;
    CreateTestCatalogsTree(deep, 2, 2);
    fs::path test_dirrectory = GetTestDirectory();
    size_t test_dirrectory_size = test_dirrectory.string().size();

    std::mutex cout_mutex;
    RecursiveWalking(deep, WALK_TYPE::LENGTH).WalkIn(test_dirrectory, [&](size_t deep, const fs::path& full_file_path) {
        if (full_file_path.extension().string().compare(".json") == 0) {
            cout_mutex.lock();
            std::cout << std::string(deep != 0 ? deep * 4 - 1 : 0, '-') << '>' << full_file_path.string().replace(0, test_dirrectory_size, "")
                      << " id: " << std::this_thread::get_id() << std::endl;
            cout_mutex.unlock();
        }
    });
}

TEST_F(RecursiveWalkingTesting, WalkTestOnWidth) {
    size_t deep = 4;
    CreateTestCatalogsTree(deep, 2, 2);
    fs::path test_dirrectory = GetTestDirectory();

    std::mutex cout_mutex;

    class Action {
        // (?) - Why we must use use reference type for std::mutex
        std::mutex& _cout_mutex;
        size_t _file_path_length;

        public:
        Action(size_t file_path_lenght, std::mutex& cout_mutex)
            : _file_path_length{ file_path_lenght }
            , _cout_mutex{ cout_mutex } {}

        void operator()(size_t deep, const fs::path& full_file_path) {
            if (full_file_path.extension().string().compare(".md") == 0) {
                _cout_mutex.lock();
                std::cout << std::string(deep != 0 ? deep * 4 - 1 : 0, '-') << '>' << full_file_path.string().replace(0, _file_path_length, "")
                          << " id: " << std::this_thread::get_id() << std::endl;
                _cout_mutex.unlock();
            }
        }
    };

    RecursiveWalking(deep, WALK_TYPE::WIDTH).WalkIn(test_dirrectory, Action(test_dirrectory.string().size(), cout_mutex));
}
