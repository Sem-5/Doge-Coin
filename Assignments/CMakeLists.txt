cmake_minimum_required( VERSION 3.13 )
project(P2P-cryptocurrency-simulation)

add_executable(sim src/main.cpp src/NetSim.cpp src/Event.cpp src/getID.cpp src/Node.cpp)
target_include_directories(sim PRIVATE include)
target_compile_features(sim PRIVATE cxx_std_17)