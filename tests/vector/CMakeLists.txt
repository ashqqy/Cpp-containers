cmake_minimum_required(VERSION 3.18)

set(TASK_NAME vector)

project(${TASK_NAME}-test)

set(AdditionalCompileOptions -Wno-self-assign-overloaded)
set(AdditionalLinkOptions -Wno-self-assign-overloaded)

add_container_test(${TASK_NAME})
