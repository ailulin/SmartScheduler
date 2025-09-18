#pragma once
#include <string>
#include <cstdint>
#include <chrono>

struct User {
    uint32_t id;
    std::string username;
    std::string password_hash;
};

struct Task {
    uint32_t id;
    uint32_t user_id;
    std::string name;
    std::string description;
    int priority;
    int duration_minutes;
    std::chrono::system_clock::time_point deadline;
    std::string type;
};

struct ScheduleItem {
    uint32_t id;
    uint32_t user_id;
    uint32_t task_id;
    std::chrono::system_clock::time_point start_time;
    std::chrono::system_clock::time_point end_time;
};

