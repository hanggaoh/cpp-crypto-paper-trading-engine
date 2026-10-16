cmake_minimum_required(VERSION 3.20)
project(cpp_crypto_paper_trading_engine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(paper_trading_demo src/main.cpp)
target_include_directories(paper_trading_demo PRIVATE include)
target_compile_options(paper_trading_demo PRIVATE -Wall -Wextra -Wpedantic)

add_executable(risk_engine_test tests/risk_engine_test.cpp)
target_include_directories(risk_engine_test PRIVATE include)
target_compile_options(risk_engine_test PRIVATE -Wall -Wextra -Wpedantic)

include(CTest)
if(BUILD_TESTING)
  add_test(NAME risk_engine_test COMMAND risk_engine_test)
endif()
