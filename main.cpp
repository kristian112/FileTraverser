#include <iostream>
#include <filesystem>
#include <string>
#include <system_error>

// A common modern C++ practice to avoid typing std::filesystem over and over
namespace fs = std::filesystem;

void scanDirectory(const std::string& rootPath) {
    std::cout << "Scanning directory: " << rootPath << "...\n";

    // C++17 flag to gracefully skip folders we don't have OS permissions to read
    auto options = fs::directory_options::skip_permission_denied;

    try {
        // fs::recursive_directory_iterator walks the tree for us.
        // We use `const auto& entry` to pass by reference.
        for (const auto& entry : fs::recursive_directory_iterator(rootPath, options)) {
            
            // std::error_code is used for the non-throwing overloads of filesystem checks
            std::error_code ec; 

            // We only want to index actual files, skipping directories and symlinks
            if (entry.is_regular_file(ec)) {
                // In Step 2, we will insert these into our Hash Map. 
                // For now, we just print them to verify the traversal works.
                std::cout << "File: " << entry.path().filename().string() << "\n";
                // std::cout << "Path: " << entry.path().string() << "\n";
            }

            // If a specific file check failed, we log it but keep the loop running
            if (ec) {
                std::cerr << "[Warning] Could not read file info: " << ec.message() << "\n";
            }
        }
    } catch (const fs::filesystem_error& e) {
        // Catches fatal filesystem errors that the iterator couldn't bypass
        std::cerr << "[Fatal Error] Directory traversal failed: " << e.what() << "\n";
    }
}

int main() {
    // For testing, "." means the directory where the executable is located.
    // Try changing this to a specific Windows path like "C:\\Users\\Public" to test it.
    std::string pathToScan = "."; 
    
    scanDirectory(pathToScan);
    
    std::cout << "\nScanning complete.\n";
    return 0;
}