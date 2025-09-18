#include "algo.h"
#include <queue>
#include <algorithm>
#include <chrono>

namespace algo {
    std::vector<ScheduleItem> generateSchedule(const std::vector<Task>& tasks) {
        std::vector<ScheduleItem> schedule;
        
        // Sort tasks by deadline first
        std::vector<Task> sorted_tasks = tasks;
        std::sort(sorted_tasks.begin(), sorted_tasks.end(), 
                  [](const Task& a, const Task& b) {
                      return a.deadline < b.deadline;
                  });
        
        // Priority queue for task scheduling (min heap by priority)
        auto task_priority = [](const Task& a, const Task& b) {
            return a.priority > b.priority; // Lower priority number = higher priority
        };
        std::priority_queue<Task, std::vector<Task>, decltype(task_priority)> task_queue(task_priority);
        
        // Current time (start from now)
        auto current_time = std::chrono::system_clock::now();
        
        // Working hours (8 AM to 10 PM)
        auto work_start = std::chrono::hours(8);
        auto work_end = std::chrono::hours(22);
        
        for (const auto& task : sorted_tasks) {
            // Check if task can be scheduled before deadline
            auto available_time = std::chrono::duration_cast<std::chrono::minutes>(
                task.deadline - current_time
            ).count();
            
            if (available_time < task.duration_minutes) {
                // Task cannot be completed before deadline, skip or handle error
                continue;
            }
            
            // Find next available time slot
            auto schedule_start = current_time;
            auto schedule_end = schedule_start + std::chrono::minutes(task.duration_minutes);
            
            // Ensure we're within working hours
            auto day_start = std::chrono::floor<std::chrono::days>(schedule_start) + work_start;
            auto day_end = std::chrono::floor<std::chrono::days>(schedule_start) + work_end;
            
            if (schedule_start < day_start) {
                schedule_start = day_start;
                schedule_end = schedule_start + std::chrono::minutes(task.duration_minutes);
            }
            
            if (schedule_end > day_end) {
                // Move to next day
                schedule_start = day_start + std::chrono::days(1);
                schedule_end = schedule_start + std::chrono::minutes(task.duration_minutes);
            }
            
            // Create schedule item
            ScheduleItem item;
            item.user_id = task.user_id;
            item.task_id = task.id;
            item.start_time = schedule_start;
            item.end_time = schedule_end;
            
            schedule.push_back(item);
            
            // Update current time
            current_time = schedule_end;
        }
        
        return schedule;
    }
}

