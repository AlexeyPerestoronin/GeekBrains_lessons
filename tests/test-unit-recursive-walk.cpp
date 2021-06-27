#include "tests/test-unit-common.hpp"

#include "recursive_walk.hpp"

#include <optional>
#include <fstream>

class RecursiveWalkingTesting : public testing::Test {
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
            fs::path file_path = fs::path(dir).append(std::string("file_").append(std::to_string(i)));
            std::fstream(file_path.string(), std::ios_base::app).close();
        }
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

    public: // testing::Test
    // Tears down the test fixture.
    virtual void TearDown() override {
        if (_test_directory.has_value())
            fs::remove_all(_test_directory.value());
    }
};

TEST_F(RecursiveWalkingTesting, WalkTestOnLenght) {
    size_t deep = 4;
    CreateTestCatalogsTree(deep, 2, 2);
    RecursiveWalking(GetTestDirectory(), deep, WALK_TYPE::LENGTH);
}

TEST_F(RecursiveWalkingTesting, WalkTestOnWidth) {
    size_t deep = 4;
    CreateTestCatalogsTree(deep, 2, 2);
    RecursiveWalking(GetTestDirectory(), deep, WALK_TYPE::WIDTH);
}
