# University SIS (Student Information System)

A modern C++ Windows Desktop Application built with Qt 6 and MySQL.

## Features

- **Modern Dashboard UI**: Sidebar navigation, responsive layout, and flat design styling.
- **Student Portal**: View, Add, and Delete students with a connected database table.
- **Database Integration**: Automatically connects to MySQL and creates necessary tables if they don't exist.
- **Modular Architecture**: Separate UI and Database logic.

## Project Structure

- `main.cpp`: Entry point.
- `mainwindow.h/cpp`: Main application shell with sidebar navigation.
- `ui/`: Contains UI modules (e.g., StudentPortal).
- `database/`: Database connection manager and schema logic.
- `database/schema.sql`: Raw SQL schema for reference.

## Requirements

- Qt 6 (Widgets, Sql)
- MySQL Server
- C++17 Compiler

## Database Configuration

The application allows connection to:
- **Host**: localhost
- **Database**: university_sis
- **User**: university_sis
- **Password**: university_sis@123

Ensure your MySQL server is running and the user exists. The application will attempt to create the database schema automatically.
