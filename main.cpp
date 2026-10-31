#include <iostream>
#include <filesystem>
#include <string>
#include <system_error>
#include <unordered_map>
#include <vector>

namespace fs = std::filesystem;

using FileIndex = std::unordered_map<std::string, std::vector<std::string>>;

void scanDirectory(const std::string& rootPath, FileIndex& index) {
    std::cout << "Scanning directory and building index...\n";
    auto options = fs::directory_options::skip_permission_denied;

    try {
        for (const auto& entry : fs::recursive_directory_iterator(rootPath, options)) {
            std::error_code ec; 
            if (entry.is_regular_file(ec)) {
                std::string filename = entry.path().filename().string();
                std::string fullPath = fs::absolute(entry.path()).string();
                index[filename].push_back(fullPath);
            }
        }
    } catch (const fs::filesystem_error& e) {
        std::cerr << "[Fatal Error] Directory traversal failed: " << e.what() << "\n";
    }
}

// Step 3: Interactive Search Loop
void searchLoop(const FileIndex& index) {
    std::string query;
    std::cout << "\n--- Local File Search Engine ---\n";
    std::cout << "Type a filename (with extension) to find its paths.\n";
    std::cout << "Type '!quit' to exit.\n\n";

    while (true) {
        std::cout << "Search> ";
        std::getline(std::cin, query); // Using getline to handle spaces in filenames

        if (query == "!quit") {
            break;
        }

        if (query.empty()) {
            continue;
        }

        // Use .find() to look up the key without accidentally modifying the map
        auto it = index.find(query);

        if (it != index.end()) {
            // it->second gives us access to the vector of paths
            const auto& paths = it->second;
            std::cout << "Found " << paths.size() << " result(s):\n";
            
            for (const auto& path : paths) {
                std::cout << "  -> " << path << "\n";
            }
        } else {
            std::cout << "File not found.\n";
        }
        std::cout << "\n";
    }
}

int main() {
    std::string pathToScan = "."; 
    FileIndex myIndex;
    
    scanDirectory(pathToScan, myIndex);
    std::cout << "Total unique filenames indexed: " << myIndex.size() << "\n";

    // Launch the interactive loop
    searchLoop(myIndex);

    std::cout << "Exiting Search Engine. Goodbye!\n";
    return 0;
}