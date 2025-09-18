# Backend Build Instructions

## Dependencies

### Required Libraries
1. **MySQL Connector/C++**
   ```bash
   # Ubuntu/Debian
   sudo apt-get install libmysqlcppconn-dev
   
   # macOS with Homebrew
   brew install mysql-connector-c++
   
   # Windows - Download from Oracle website
   # https://dev.mysql.com/downloads/connector/cpp/
   ```

2. **Crow Framework**
   ```bash
   git clone https://github.com/CrowCpp/Crow.git
   cd Crow
   mkdir build && cd build
   cmake .. -DCROW_BUILD_EXAMPLES=OFF
   make -j4
   sudo make install
   ```

3. **JSON Library**
   ```bash
   # Ubuntu/Debian
   sudo apt-get install libjsoncpp-dev
   
   # macOS with Homebrew
   brew install jsoncpp
   
   # Windows - Download from GitHub releases
   # https://github.com/open-source-parsers/jsoncpp
   ```

## Database Setup

1. **Install MySQL Server**
   ```bash
   # Ubuntu/Debian
   sudo apt-get install mysql-server
   
   # macOS with Homebrew
   brew install mysql
   
   # Windows - Download from Oracle website
   ```

2. **Create Database and Tables**
   ```sql
   CREATE DATABASE schedule_planner;
   USE schedule_planner;
   
   CREATE TABLE users (
       id INT AUTO_INCREMENT PRIMARY KEY,
       username VARCHAR(50) UNIQUE NOT NULL,
       password VARCHAR(255) NOT NULL
   );
   
   CREATE TABLE tasks (
       id INT AUTO_INCREMENT PRIMARY KEY,
       user_id INT NOT NULL,
       name VARCHAR(100) NOT NULL,
       description TEXT,
       priority INT NOT NULL,
       duration INT NOT NULL,
       deadline DATETIME NOT NULL,
       type ENUM('study', 'work', 'personal') NOT NULL,
       FOREIGN KEY (user_id) REFERENCES users(id)
   );
   
   CREATE TABLE schedule (
       id INT AUTO_INCREMENT PRIMARY KEY,
       user_id INT NOT NULL,
       task_id INT NOT NULL,
       start_time DATETIME NOT NULL,
       end_time DATETIME NOT NULL,
       FOREIGN KEY (user_id) REFERENCES users(id),
       FOREIGN KEY (task_id) REFERENCES tasks(id)
   );
   ```

## Build Instructions

1. **Clone and Build**
   ```bash
   mkdir build
   cd build
   cmake ..
   make -j4
   ```

2. **Run Server**
   ```bash
   ./schedule_server
   ```

## Configuration

Update database connection settings in `main.cpp`:
```cpp
db::init("localhost", "your_username", "your_password", "schedule_planner")
```

## API Endpoints

- `GET /tasks/{user_id}` - Get all tasks for a user
- `POST /tasks` - Add a new task
- `POST /schedule/{user_id}` - Generate schedule for a user
- `GET /` - Serve frontend

## Testing

Test the API endpoints:
```bash
# Get tasks
curl http://localhost:8080/tasks/1

# Add task
curl -X POST http://localhost:8080/tasks \
  -H "Content-Type: application/json" \
  -d '{"user_id":1,"name":"Study Math","description":"Complete calculus homework","priority":1,"duration":120,"deadline":"2024-01-15 23:59:59","type":"study"}'

# Generate schedule
curl -X POST http://localhost:8080/schedule/1
```

