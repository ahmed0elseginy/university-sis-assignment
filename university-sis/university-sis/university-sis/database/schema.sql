CREATE DATABASE IF NOT EXISTS university_sis;
USE university_sis;

-- Students Table
CREATE TABLE IF NOT EXISTS students (
    id INT AUTO_INCREMENT PRIMARY KEY,
    first_name VARCHAR(50) NOT NULL,
    last_name VARCHAR(50) NOT NULL,
    email VARCHAR(100) UNIQUE,
    enrollment_date DATE,
    major VARCHAR(100)
);

-- Courses Table
CREATE TABLE IF NOT EXISTS courses (
    id INT AUTO_INCREMENT PRIMARY KEY,
    course_code VARCHAR(20) UNIQUE NOT NULL,
    course_name VARCHAR(100) NOT NULL,
    credits INT DEFAULT 3
);

-- Enrollments
CREATE TABLE IF NOT EXISTS enrollments (
    id INT AUTO_INCREMENT PRIMARY KEY,
    student_id INT,
    course_id INT,
    grade VARCHAR(5),
    FOREIGN KEY (student_id) REFERENCES students(id) ON DELETE CASCADE,
    FOREIGN KEY (course_id) REFERENCES courses(id) ON DELETE CASCADE
);

-- Users (for login)
CREATE TABLE IF NOT EXISTS users (
    id INT AUTO_INCREMENT PRIMARY KEY,
    username VARCHAR(50) UNIQUE NOT NULL,
    password VARCHAR(255) NOT NULL, -- In production, use hashing
    role VARCHAR(20) DEFAULT 'student'
);

-- Insert Admin
INSERT IGNORE INTO users (username, password, role) VALUES ('admin', '1234', 'admin');

-- Sample Data
INSERT INTO students (first_name, last_name, enrollment_date, major) VALUES 
('Alice', 'Smith', CURDATE(), 'Computer Science'),
('Bob', 'Jones', CURDATE(), 'Mathematics');

-- Infrastructure
CREATE TABLE IF NOT EXISTS buildings (
    building_id INT AUTO_INCREMENT PRIMARY KEY,
    name VARCHAR(100),
    code VARCHAR(20) UNIQUE,
    location VARCHAR(200)
);

CREATE TABLE IF NOT EXISTS rooms (
    room_id INT AUTO_INCREMENT PRIMARY KEY,
    building_id INT,
    room_number VARCHAR(20),
    type VARCHAR(50),
    capacity INT,
    FOREIGN KEY (building_id) REFERENCES buildings(building_id) ON DELETE CASCADE
);
