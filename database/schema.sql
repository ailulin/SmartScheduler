-- Smart Schedule Planning Assistant Database Schema
-- Version: 1.0

CREATE DATABASE IF NOT EXISTS schedule_planner;
USE schedule_planner;

-- Users table
CREATE TABLE IF NOT EXISTS users (
    id INT AUTO_INCREMENT PRIMARY KEY,
    username VARCHAR(50) UNIQUE NOT NULL,
    password VARCHAR(255) NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP
);

-- Tasks table
CREATE TABLE IF NOT EXISTS tasks (
    id INT AUTO_INCREMENT PRIMARY KEY,
    user_id INT NOT NULL,
    name VARCHAR(100) NOT NULL,
    description TEXT,
    priority INT NOT NULL DEFAULT 5,
    duration INT NOT NULL COMMENT 'Duration in minutes',
    deadline DATETIME NOT NULL,
    type ENUM('study', 'work', 'personal') NOT NULL DEFAULT 'personal',
    status ENUM('pending', 'completed', 'cancelled') NOT NULL DEFAULT 'pending',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
    FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE,
    INDEX idx_user_deadline (user_id, deadline),
    INDEX idx_user_priority (user_id, priority)
);

-- Schedule table
CREATE TABLE IF NOT EXISTS schedule (
    id INT AUTO_INCREMENT PRIMARY KEY,
    user_id INT NOT NULL,
    task_id INT NOT NULL,
    start_time DATETIME NOT NULL,
    end_time DATETIME NOT NULL,
    status ENUM('scheduled', 'completed', 'missed') NOT NULL DEFAULT 'scheduled',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
    FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE,
    FOREIGN KEY (task_id) REFERENCES tasks(id) ON DELETE CASCADE,
    INDEX idx_user_time (user_id, start_time),
    INDEX idx_task_time (task_id, start_time)
);

-- Insert sample user for testing
INSERT INTO users (username, password) VALUES 
('demo_user', 'demo_password_hash'),
('test_user', 'test_password_hash');

-- Insert sample tasks for testing
INSERT INTO tasks (user_id, name, description, priority, duration, deadline, type) VALUES 
(1, 'Complete Math Homework', 'Finish calculus assignment chapter 5', 1, 120, '2024-01-15 23:59:59', 'study'),
(1, 'Project Meeting', 'Weekly team standup meeting', 2, 60, '2024-01-14 14:00:00', 'work'),
(1, 'Grocery Shopping', 'Buy ingredients for dinner', 5, 45, '2024-01-16 18:00:00', 'personal'),
(1, 'Read Research Paper', 'Review machine learning paper for thesis', 3, 90, '2024-01-17 22:00:00', 'study'),
(1, 'Gym Workout', 'Cardio and strength training session', 4, 75, '2024-01-15 19:00:00', 'personal');

-- Create indexes for better performance
CREATE INDEX idx_tasks_user_status ON tasks(user_id, status);
CREATE INDEX idx_schedule_user_status ON schedule(user_id, status);
CREATE INDEX idx_tasks_deadline ON tasks(deadline);
CREATE INDEX idx_schedule_start_time ON schedule(start_time);

-- Add constraints to ensure data integrity
ALTER TABLE tasks ADD CONSTRAINT chk_priority CHECK (priority >= 1 AND priority <= 10);
ALTER TABLE tasks ADD CONSTRAINT chk_duration CHECK (duration > 0);
ALTER TABLE schedule ADD CONSTRAINT chk_time_order CHECK (end_time > start_time);

-- Create view for user tasks with schedule info
CREATE VIEW user_task_schedule AS
SELECT 
    t.id as task_id,
    t.user_id,
    t.name as task_name,
    t.description,
    t.priority,
    t.duration,
    t.deadline,
    t.type,
    t.status as task_status,
    s.id as schedule_id,
    s.start_time,
    s.end_time,
    s.status as schedule_status
FROM tasks t
LEFT JOIN schedule s ON t.id = s.task_id
ORDER BY t.user_id, t.priority, t.deadline;

-- Create stored procedure for generating schedule statistics
DELIMITER //
CREATE PROCEDURE GetScheduleStats(IN user_id INT)
BEGIN
    SELECT 
        COUNT(*) as total_tasks,
        SUM(CASE WHEN status = 'completed' THEN 1 ELSE 0 END) as completed_tasks,
        SUM(CASE WHEN status = 'pending' THEN 1 ELSE 0 END) as pending_tasks,
        SUM(CASE WHEN deadline < NOW() AND status = 'pending' THEN 1 ELSE 0 END) as overdue_tasks,
        AVG(priority) as avg_priority,
        SUM(duration) as total_duration_minutes
    FROM tasks 
    WHERE user_id = user_id;
END //
DELIMITER ;

-- Grant permissions (adjust as needed for your setup)
-- GRANT ALL PRIVILEGES ON schedule_planner.* TO 'schedule_user'@'localhost' IDENTIFIED BY 'schedule_password';
-- FLUSH PRIVILEGES;

