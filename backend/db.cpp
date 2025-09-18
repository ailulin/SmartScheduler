#include "db.h"
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <iostream>
#include <sstream>
#include <iomanip>

static sql::mysql::MySQL_Driver* driver = nullptr;
static sql::Connection* connection = nullptr;

bool db::init(const std::string& host, const std::string& user, 
              const std::string& password, const std::string& database) {
    try {
        driver = sql::mysql::get_mysql_driver_instance();
        std::string connection_str = "tcp://" + host + ":3306";
        connection = driver->connect(connection_str, user, password);
        connection->setSchema(database);
        return true;
    } catch (sql::SQLException& e) {
        std::cerr << "Database connection failed: " << e.what() << std::endl;
        return false;
    }
}

std::vector<Task> db::getTasks(uint32_t user_id) {
    std::vector<Task> tasks;
    try {
        sql::PreparedStatement* stmt = connection->prepareStatement(
            "SELECT id, user_id, name, description, priority, duration, deadline, type FROM tasks WHERE user_id = ?"
        );
        stmt->setUInt(1, user_id);
        sql::ResultSet* res = stmt->executeQuery();
        
        while (res->next()) {
            Task task;
            task.id = res->getUInt("id");
            task.user_id = res->getUInt("user_id");
            task.name = res->getString("name");
            task.description = res->getString("description");
            task.priority = res->getInt("priority");
            task.duration_minutes = res->getInt("duration");
            
            std::string deadline_str = res->getString("deadline");
            std::tm tm = {};
            std::istringstream ss(deadline_str);
            ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
            task.deadline = std::chrono::system_clock::from_time_t(std::mktime(&tm));
            
            task.type = res->getString("type");
            tasks.push_back(task);
        }
        delete res;
        delete stmt;
    } catch (sql::SQLException& e) {
        std::cerr << "Failed to get tasks: " << e.what() << std::endl;
    }
    return tasks;
}

bool db::addTask(const Task& task) {
    try {
        sql::PreparedStatement* stmt = connection->prepareStatement(
            "INSERT INTO tasks (user_id, name, description, priority, duration, deadline, type) VALUES (?, ?, ?, ?, ?, ?, ?)"
        );
        stmt->setUInt(1, task.user_id);
        stmt->setString(2, task.name);
        stmt->setString(3, task.description);
        stmt->setInt(4, task.priority);
        stmt->setInt(5, task.duration_minutes);
        
        std::time_t deadline_time = std::chrono::system_clock::to_time_t(task.deadline);
        std::tm* tm = std::localtime(&deadline_time);
        char buffer[32];
        std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", tm);
        stmt->setString(6, std::string(buffer));
        
        stmt->setString(7, task.type);
        stmt->execute();
        delete stmt;
        return true;
    } catch (sql::SQLException& e) {
        std::cerr << "Failed to add task: " << e.what() << std::endl;
        return false;
    }
}

std::vector<ScheduleItem> db::generateSchedule(uint32_t user_id) {
    auto tasks = getTasks(user_id);
    auto schedule = algo::generateSchedule(tasks);
    
    // Save schedule to database
    try {
        sql::PreparedStatement* stmt = connection->prepareStatement(
            "INSERT INTO schedule (user_id, task_id, start_time, end_time) VALUES (?, ?, ?, ?)"
        );
        for (const auto& item : schedule) {
            stmt->setUInt(1, item.user_id);
            stmt->setUInt(2, item.task_id);
            
            std::time_t start_time = std::chrono::system_clock::to_time_t(item.start_time);
            std::tm* tm = std::localtime(&start_time);
            char buffer[32];
            std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", tm);
            stmt->setString(3, std::string(buffer));
            
            std::time_t end_time = std::chrono::system_clock::to_time_t(item.end_time);
            tm = std::localtime(&end_time);
            std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", tm);
            stmt->setString(4, std::string(buffer));
            
            stmt->execute();
        }
        delete stmt;
    } catch (sql::SQLException& e) {
        std::cerr << "Failed to save schedule: " << e.what() << std::endl;
    }
    
    return schedule;
}

