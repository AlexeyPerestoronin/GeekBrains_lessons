#include "recursive_walk.hpp"

void RecursiveWalking(const fs::path& catalog, size_t deep, WALK_TYPE type) {
    fs::directory_entry initial_dir(static_cast<fs::directory_entry>(catalog));

    if (!static_cast<fs::directory_entry>(initial_dir).is_directory())
        throw std::exception("initial directory is not OS catalog");

    using UnchekedDirectory = std::tuple<size_t, fs::directory_entry>;

    std::list<UnchekedDirectory> unchecked_directories{ { 0, initial_dir } };

    while (!unchecked_directories.empty()) {
        auto [current_deep, current_dir] = unchecked_directories.front();
        unchecked_directories.pop_front();

        if (current_deep <= deep) {
            for (const fs::directory_entry& sub_dir : fs::directory_iterator(current_dir, fs::directory_options::skip_permission_denied)) {
                if (!sub_dir.is_directory()) {
                    std::cout << std::string(current_deep * 4, ' ') << sub_dir.path().string() << std::endl;
                } else {
                    UnchekedDirectory unchecked_element = std::make_tuple(current_deep + 1, sub_dir);
                    if (type == WALK_TYPE::LENGTH) {
                        unchecked_directories.emplace_front(std::move(unchecked_element));
                    } else if (type == WALK_TYPE::WIDTH) {
                        unchecked_directories.emplace_back(std::move(unchecked_element));
                    } else {
                        throw std::exception("unknown type of walk through OS catalogs");
                    }
                }
            }
        }
    }
}
