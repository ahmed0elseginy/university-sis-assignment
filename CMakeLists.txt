cmake_minimum_required(VERSION 3.16)

project(university-sis-assignment_root)

# Verify Qt6 is available globally if possible, or let the subdirectory handle it.
# We just include the subdirectory where the real project lives.

add_subdirectory(university-sis)
