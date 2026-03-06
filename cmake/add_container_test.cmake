include(${CMAKE_SOURCE_DIR}/cmake/options.cmake)

function(add_container_test TASK_NAME)
  set(TARGET_NAME "${TASK_NAME}-test")
  add_executable(${TARGET_NAME} tests.cpp)

    target_include_directories(${TARGET_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/tests
    ${CMAKE_SOURCE_DIR}/tests/${TASK_NAME}
    ${CMAKE_SOURCE_DIR}/${TASK_NAME}
  )

  target_link_libraries(${TARGET_NAME}
    ${GTEST_LIBRARIES}
  )

  target_compile_options(${TARGET_NAME} PUBLIC ${COMPILE_OPT} ${AdditionalCompileOptions})
  target_link_options(${TARGET_NAME} PUBLIC ${LINK_OPT} ${AdditionalLinkOptions})
endfunction()
