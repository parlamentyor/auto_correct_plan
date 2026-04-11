#ifndef MY_LOGGER_H
#define MY_LOGGER_H

#pragma once

#include "general_functions.h"

#include <algorithm>
#include <chrono>
#include <iomanip>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <string_view>
#include <optional>
#include <mutex>
#include <thread>

using namespace std::literals;

//static const std::string LOG_PATH = "/var/log/"s; // для linux, только нужно учесть и установить права доступа к этой папке
//static const std::string LOG_PATH = "./log/"s; // , попытка допилить и писать файлы только в эту папку

#define LOG(...) MyLogger::GetInstance().Log(__VA_ARGS__)

class MyLogger {
    auto GetTime() const {
        if (manual_ts_) {
            return *manual_ts_;
        }

        return std::chrono::system_clock::now();
    }

    auto GetTimeStamp() const {
        const auto now = GetTime();
        //        const auto now = std::chrono::system_clock::now();
        const auto t_c = std::chrono::system_clock::to_time_t(now);

        struct tm time_info;
        localtime_s(&time_info, &t_c);

        char buffer[100];
        std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &time_info);
        return std::string(buffer);
    }

    // Для имени файла возьмём дату с форматом "%Y_%m_%d"
    std::string GetFileTimeStamp() const {
        std::ostringstream oss;
        oss << GetTimeStamp();
        std::string time = oss.str();
        std::string name_file = time.substr(0, time.find_first_of(" "));
        std::replace(name_file.begin(), name_file.end(), '-', '_');
        return name_file;
    }

    MyLogger() = default;
    MyLogger(const MyLogger&) = delete;

public:
    static MyLogger& GetInstance() {
        static MyLogger obj;
        return obj;
    }

    template<class... Ts>
    void Log(const Ts&... args) {
        std::lock_guard<std::mutex> g(m_);

        file_name_ = details::CreatePathDokument("log", "sample_log_") + GetFileTimeStamp() + ".log";
        //  abs_path_ = std::filesystem::weakly_canonical(std::filesystem::path(LOG_PATH) / std::filesystem::path(file_name_));
        abs_path_ = std::filesystem::weakly_canonical(std::filesystem::path(file_name_));

        log_file_.open(abs_path_, std::ios::app);
        log_file_ << GetTimeStamp() << ": ";
        ((log_file_ << args), ...);
        log_file_ << std::endl;
        log_file_.close();
    }

    // Устанливаем manual_ts_. Учитываем, что эта операция может выполняться параллельно с выводом в поток.
    // Нужно предусмотреть синхронизацию.
    void SetTimestamp(std::chrono::system_clock::time_point ts) {
        std::lock_guard<std::mutex> g(m_);
        manual_ts_ = ts;
    }


private:
    std::optional<std::chrono::system_clock::time_point> manual_ts_;
    std::string file_name_;
    std::filesystem::path abs_path_;
//    bool isTimeChanged_ = true;
    std::mutex m_;
    std::ofstream log_file_;
};


#endif // MY_LOGGER_H
