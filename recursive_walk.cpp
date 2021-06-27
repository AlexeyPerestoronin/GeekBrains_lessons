#include "recursive_walk.hpp"

RecursiveWalking::RecursiveWalking(size_t deep, WALK_TYPE type, size_t thread_quantity)
    : _deep{ deep }
    , _type{ type }
    , _thread_quantity{ thread_quantity } {}

void RecursiveWalking::WalkIn(const fs::path& catalog, std::optional<std::function<void(size_t /*deep*/, const fs::path& /*full_file_path*/)>> action) {
    fs::directory_entry initial_dir(static_cast<fs::directory_entry>(catalog));

    if (!static_cast<fs::directory_entry>(initial_dir).is_directory())
        throw std::exception("initial directory is not OS catalog");

    using UnchekedDirectory = std::tuple<size_t, fs::directory_entry>;

    std::shared_mutex recursive_walking_shared_mutex;
    std::shared_mutex is_walk_executing_shared_mutex;

    std::list<UnchekedDirectory> unchecked_directories{ { 0, initial_dir } };
    std::mutex unchecked_directories_mutex;

    auto Walker = [&]() {
        std::shared_lock rwsm_lock(recursive_walking_shared_mutex);
        while (true) {
            unchecked_directories_mutex.lock();
            std::optional<UnchekedDirectory> unchecked_directory;
            if (!unchecked_directories.empty()) {
                unchecked_directory = unchecked_directories.front();
                unchecked_directories.pop_front();
            }
            unchecked_directories_mutex.unlock();

            if (unchecked_directory.has_value()) {
                std::shared_lock is_wesm_lock(is_walk_executing_shared_mutex);

                auto [current_deep, current_dir] = unchecked_directory.value();

                if (current_deep <= _deep) {
                    for (const fs::directory_entry& sub_dir : fs::directory_iterator(current_dir, fs::directory_options::skip_permission_denied)) {
                        if (!sub_dir.is_directory()) {
                            if (action.has_value())
                                action.value()(current_deep, sub_dir.path());
                        } else {
                            UnchekedDirectory unchecked_element = std::make_tuple(current_deep + 1, sub_dir);
                            unchecked_directories_mutex.lock();
                            if (_type == WALK_TYPE::LENGTH) {
                                unchecked_directories.emplace_front(std::move(unchecked_element));
                            } else if (_type == WALK_TYPE::WIDTH) {
                                unchecked_directories.emplace_back(std::move(unchecked_element));
                            } else {
                                throw std::exception("unknown type of walk through OS catalogs");
                            }
                            unchecked_directories_mutex.unlock();
                        }
                    }
                }
            } else if (is_walk_executing_shared_mutex.try_lock()) {
                is_walk_executing_shared_mutex.unlock();
                return;
            }
        }
    };

    std::vector<std::thread> active_threds;
    active_threds.reserve(_thread_quantity);

    for (size_t i = 0; i < _thread_quantity; ++i)
        active_threds.emplace_back(std::thread(Walker));

    for (size_t i = 0; i < _thread_quantity; ++i)
        active_threds[i].join();

    std::unique_lock lock(recursive_walking_shared_mutex);
}

// (!) - estimate this code from 1 to 10 points: where 1 - is terrible, 10 - is perfect
// (?) - is the code have dangerous section ?
// (?) - how we could beauty or improve this code ?
