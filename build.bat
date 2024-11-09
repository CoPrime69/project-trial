@echo off
setlocal

REM Create build directory if it doesn't exist
if not exist "build" mkdir build

echo Compiling source files...
g++ -c "cpp files/main.cpp" -I"header files" -o build/main.o
g++ -c "cpp files/adjacency_matrix.cpp" -I"header files" -o build/adjacency_matrix.o
g++ -c "cpp files/connection_manager.cpp" -I"header files" -o build/connection_manager.o
g++ -c "cpp files/login_manager.cpp" -I"header files" -o build/login_manager.o
g++ -c "cpp files/graph_visualizer.cpp" -I"header files" -o build/graph_visualizer.o

echo Linking...
g++ build/main.o build/adjacency_matrix.o build/connection_manager.o build/login_manager.o build/graph_visualizer.o -o social_network.exe

echo Running program...
social_network.exe

echo Cleaning up build files...
rd /s /q build

echo Process complete!