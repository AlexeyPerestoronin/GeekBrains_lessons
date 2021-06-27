#include "recursive_walk.hpp"

RecursiveWalking::RecursiveWalking(size_t deep, WALK_TYPE type)
    : _deep{ deep }
    , _type{ type } {}

void RecursiveWalking::WalkIn(const fs::path& catalog, std::optional<std::function<void(size_t /*deep*/, const fs::path& /*full_file_path*/)>> action) {
    fs::directory_entry initial_dir(static_cast<fs::directory_entry>(catalog));

    if (!static_cast<fs::directory_entry>(initial_dir).is_directory())
        throw std::exception("initial directory is not OS catalog");

    using UnchekedDirectory = std::tuple<size_t, fs::directory_entry>;

    std::list<UnchekedDirectory> unchecked_directories{ { 0, initial_dir } };

    while (!unchecked_directories.empty()) {
        auto [current_deep, current_dir] = unchecked_directories.front();
        unchecked_directories.pop_front();

        if (current_deep <= _deep) {
            for (const fs::directory_entry& sub_dir : fs::directory_iterator(current_dir, fs::directory_options::skip_permission_denied)) {
                if (!sub_dir.is_directory()) {
                    if (action.has_value())
                        action.value()(current_deep, sub_dir.path());
                } else {
                    UnchekedDirectory unchecked_element = std::make_tuple(current_deep + 1, sub_dir);
                    if (_type == WALK_TYPE::LENGTH) {
                        unchecked_directories.emplace_front(std::move(unchecked_element));
                    } else if (_type == WALK_TYPE::WIDTH) {
                        unchecked_directories.emplace_back(std::move(unchecked_element));
                    } else {
                        throw std::exception("unknown type of walk through OS catalogs");
                    }
                }
            }
        }
    }
}

// (?) - how do you think, is it need to implement `RecursiveWalking`-class as a template class? why? how exactly?
