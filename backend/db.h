#pragma once
#include "models.h"
#include <vector>

namespace db {
    bool init(const std::string& host,
              const std::string& user,
              const std::string& password,
              const std::string& database);

    std::vector<Task> getTasks(uint32_t user_id);
    bool addTask(const Task& task);

    std::vector<ScheduleItem> generateSchedule(uint32_t user_id);
}

