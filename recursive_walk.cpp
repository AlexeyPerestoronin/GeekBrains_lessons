#include "recursive_walk.hpp"

void RecursiveWalking(const fs::path& catalog, int16_t deep) {
    fs::directory_entry initial_dir(static_cast<fs::directory_entry>(catalog));

    if (!static_cast<fs::directory_entry>(initial_dir).is_directory())
        throw std::exception("initial directory is not OS catalog");

    std::list<std::tuple<int16_t, fs::directory_entry>> unchecked_directory{ { 0, initial_dir } };

    while (!unchecked_directory.empty()) {
        auto [current_deep, current_dir] = unchecked_directory.front();
        unchecked_directory.pop_front();

        if (current_deep <= deep) {
            for (const fs::directory_entry& sub_dir : fs::directory_iterator(current_dir, fs::directory_options::skip_permission_denied)) {
                if (!sub_dir.is_directory()) {
                    std::cout << sub_dir.path().string() << std::endl;
                } else {
                    unchecked_directory.emplace_back(std::make_tuple(current_deep + 1, sub_dir));
                }
            }
        }
    }
}
