#include "recursive_walk.hpp"

void RecursiveWalking(const fs::path& initial_path) {
    for (const auto& dir : fs::directory_iterator(initial_path, fs::directory_options::skip_permission_denied))
        if (!dir.is_directory())
            std::cout << dir.path().string() << std::endl;
        else
            RecursiveWalking(dir.path());
}