cmake_minimum_required(VERSION 3.18)

set(TASK_NAME vector)

project(${TASK_NAME}-test)

add_container_test(${TASK_NAME})
