#ifndef GENERAL_FUNCTIONS_H
#define GENERAL_FUNCTIONS_H

#include <string>
#include <windows.h>
#include <filesystem>

namespace details {
    // Создаем папку, если ее нет, и создаём путь к файлу
    inline std::string CreatePathDokument(const std::string& name_dir, const std::string& name_file) {
        // 1. Создаем папку "save" в месте расположения программы
        std::string exe_path;
        char buffer[MAX_PATH];
        GetModuleFileNameA(NULL, buffer, MAX_PATH);
        exe_path = buffer;

        std::filesystem::path exe_dir = std::filesystem::path(exe_path).parent_path();
        std::filesystem::path save_folder = exe_dir / name_dir;

        // Создаем папку (STL filesystem)
        if (!std::filesystem::exists(save_folder)) {
            std::filesystem::create_directory(save_folder);
        }

        // Полный путь к файлу Word
        std::filesystem::path doc_path = save_folder / name_file;
    //    return QString::fromStdWString(doc_path.wstring());
        return doc_path.string();
    }
} // namespace details
#endif // GENERAL_FUNCTIONS_H
