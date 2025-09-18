#include <crow.h>
#include <crow/middlewares/cors.h>
#include "db.h"
#include "models.h"
#include <iostream>
#include <json/json.h>

int main() {
    // Initialize database
    if (!db::init("localhost", "root", "password", "schedule_planner")) {
        std::cerr << "Failed to initialize database" << std::endl;
        return 1;
    }
    
    // Create Crow app with CORS
    crow::App<crow::CORSHandler> app;
    
    // Configure CORS
    auto& cors = app.get_middleware<crow::CORSHandler>();
    cors.global()
        .headers("X-Custom-Header", "Upgrade-Insecure-Requests")
        .methods("GET"_method, "POST"_method, "PUT"_method, "DELETE"_method)
        .origin("*");
    
    // Get tasks endpoint
    CROW_ROUTE(app, "/tasks/<int>")
    .methods("GET"_method)
    ([&](int user_id) {
        auto tasks = db::getTasks(user_id);
        
        Json::Value json_tasks(Json::arrayValue);
        for (const auto& task : tasks) {
            Json::Value json_task;
            json_task["id"] = task.id;
            json_task["user_id"] = task.user_id;
            json_task["name"] = task.name;
            json_task["description"] = task.description;
            json_task["priority"] = task.priority;
            json_task["duration"] = task.duration_minutes;
            
            // Convert deadline to string
            std::time_t deadline_time = std::chrono::system_clock::to_time_t(task.deadline);
            std::tm* tm = std::localtime(&deadline_time);
            char buffer[32];
            std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", tm);
            json_task["deadline"] = std::string(buffer);
            
            json_task["type"] = task.type;
            json_tasks.append(json_task);
        }
        
        crow::response res;
        res.add_header("Content-Type", "application/json");
        res.body = json_tasks.toStyledString();
        return res;
    });
    
    // Add task endpoint
    CROW_ROUTE(app, "/tasks")
    .methods("POST"_method)
    ([&](const crow::request& req) {
        Json::Value json_req;
        Json::Reader reader;
        
        if (!reader.parse(req.body, json_req)) {
            return crow::response(400, "Invalid JSON");
        }
        
        Task task;
        task.user_id = json_req["user_id"].asUInt();
        task.name = json_req["name"].asString();
        task.description = json_req["description"].asString();
        task.priority = json_req["priority"].asInt();
        task.duration_minutes = json_req["duration"].asInt();
        task.type = json_req["type"].asString();
        
        // Parse deadline
        std::string deadline_str = json_req["deadline"].asString();
        std::tm tm = {};
        std::istringstream ss(deadline_str);
        ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
        task.deadline = std::chrono::system_clock::from_time_t(std::mktime(&tm));
        
        if (db::addTask(task)) {
            return crow::response(201, "Task added successfully");
        } else {
            return crow::response(500, "Failed to add task");
        }
    });
    
    // Generate schedule endpoint
    CROW_ROUTE(app, "/schedule/<int>")
    .methods("POST"_method)
    ([&](int user_id) {
        auto schedule = db::generateSchedule(user_id);
        
        Json::Value json_schedule(Json::arrayValue);
        for (const auto& item : schedule) {
            Json::Value json_item;
            json_item["id"] = item.id;
            json_item["user_id"] = item.user_id;
            json_item["task_id"] = item.task_id;
            
            // Convert times to strings
            std::time_t start_time = std::chrono::system_clock::to_time_t(item.start_time);
            std::tm* tm = std::localtime(&start_time);
            char buffer[32];
            std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", tm);
            json_item["start_time"] = std::string(buffer);
            
            std::time_t end_time = std::chrono::system_clock::to_time_t(item.end_time);
            tm = std::localtime(&end_time);
            std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", tm);
            json_item["end_time"] = std::string(buffer);
            
            json_schedule.append(json_item);
        }
        
        crow::response res;
        res.add_header("Content-Type", "application/json");
        res.body = json_schedule.toStyledString();
        return res;
    });
    
    // Serve static files
    CROW_ROUTE(app, "/")
    ([]() {
        return crow::load_text("frontend/index.html");
    });
    
    app.port(8080).multithreaded().run();
    
    return 0;
}

