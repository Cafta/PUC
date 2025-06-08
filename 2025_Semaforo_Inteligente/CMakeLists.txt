cmake_minimum_required(VERSION 3.10)
project(ContadorOpenCV)

set(CMAKE_CXX_STANDARD 17)

# -------------------------------------------------------------------
# 1. OpenCV monolítico (opencv_world) instalado em /usr/local
# -------------------------------------------------------------------
set(OpenCV_DIR "/usr/local/lib/cmake/opencv4")   # onde está OpenCVConfig.cmake
find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS} include)
link_directories(/usr/local/lib)                 # onde está libopencv_world.so

# -------------------------------------------------------------------
# 2. Executável 'contador'
# -------------------------------------------------------------------
add_executable(contador
    src/main.cpp
    src/contador.cpp
)
target_link_libraries(contador PRIVATE opencv_world)
set_target_properties(contador PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})  # gera na raiz

# -------------------------------------------------------------------
# 3. Executável 'testa_cuda'
# -------------------------------------------------------------------
add_executable(testa_cuda
    src/testa_cuda.cpp
)
target_link_libraries(testa_cuda PRIVATE opencv_world)
set_target_properties(testa_cuda PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})  # gera na raiz

# -------------------------------------------------------------------
# 4. Executável 'testa_comunicacao'
# -------------------------------------------------------------------
add_executable(testa_comunicacao
    src/testa_comunicacao.cpp
)
# (nenhuma dependência do OpenCV)
set_target_properties(testa_comunicacao PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})  # gera na raiz


# -------------------------------------------------------------------
# 5. Executável 'decisao.cpp'
# -------------------------------------------------------------------
add_executable(decisao
    src/decisao.cpp
)
# (nenhuma dependência do OpenCV)
set_target_properties(decisao PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})  # gera na raiz