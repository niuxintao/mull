add_custom_target(build-docs
  COMMAND make html
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/docs
)
add_custom_target(clean-docs
  COMMAND make clean
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/docs
)