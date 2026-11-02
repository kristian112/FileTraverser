#include <iostream>
#include <filesystem>
#include <string>
#include <system_error>
#include <unordered_map>
#include <vector>
#include <fstream> // Required for file I/O

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

// Step 4a: Save Index to Disk
void saveIndex(const FileIndex& index, const std::string& dbPath) {
    std::ofstream out(dbPath);
    if (!out.is_open()) {
        std::cerr << "Error: Could not open file for writing.\n";
        return;
    }

    // C++17 Structured Binding: [key, value]
    for (const auto& [filename, paths] : index) {
        out << filename << '\n';
        out << paths.size() << '\n'; // Explicitly store the count of paths
        for (const auto& path : paths) {
            out << path << '\n';
        }
    }
    out.close();
}

// Step 4b: Load Index from Disk
bool loadIndex(FileIndex& index, const std::string& dbPath) {
    std::ifstream in(dbPath);
    if (!in.is_open()) return false;

    std::string filename, countStr, path;
    
    // Read the filename. If it succeeds, continue.
    while (std::getline(in, filename)) {
        if (!std::getline(in, countStr)) break;
        
        int count = std::stoi(countStr); // Convert string count to integer
        
        for (int i = 0; i < count; ++i) {
            std::getline(in, path);
            index[filename].push_back(path);
        }
    }
    in.close();
    return true;
}

void searchLoop(const FileIndex& index) {
    std::string query;
    std::cout << "\n--- Local File Search Engine ---\n";
    std::cout << "Type a filename (with extension) to find its paths.\n";
    std::cout << "Type '!quit' to exit.\n\n";

    while (true) {
        std::cout << "Search> ";
        std::getline(std::cin, query);

        if (query == "!quit") break;
        if (query.empty()) continue;

        auto it = index.find(query);
        if (it != index.end()) {
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
    std::string dbFile = "index_data.txt";
    FileIndex myIndex;
    
    // Check if the serialized index file exists using C++17 filesystem
    if (fs::exists(dbFile)) {

        std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n";
        std::cout << "Found existing index. Loading from disk...\n";
        loadIndex(myIndex, dbFile);
    } else {
        std::cout << "0000000000000000000000000000000000000\n";
        scanDirectory(pathToScan, myIndex);
        std::cout << "Saving index to disk...\n";
        saveIndex(myIndex, dbFile);
    }
    
    std::cout << "Total unique filenames indexed: " << myIndex.size() << "\n";
    searchLoop(myIndex);

    std::cout << "Exiting Search Engine. Goodbye!\n";
    return 0;
}