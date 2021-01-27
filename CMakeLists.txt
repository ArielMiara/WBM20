cmake_minimum_required(VERSION 3.10)
project(WBM20)

set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -g")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g")

if(${GHAASDIR})
    include_directories(${GHAASDIR}/include)
    link_directories(${GHAASDIR}/lib)
else(${GHAASDIR})
    include_directories(/usr/local/share/ghaas/include)
    link_directories(/usr/local/share/ghaas/lib)
endif(${GHAASDIR})

FILE(GLOB sources src/*.c)
add_executable(WBM20 ${sources})

if(${CMAKE_HOST_APPLE})
    target_link_libraries(WBM20 MF30 CM30 m)
else(${CMAKE_HOST_APPLE})
    target_link_libraries(WBM20 MF30 CM30 m -pthread)
endif(${CMAKE_HOST_APPLE})

target_include_directories(WBM20 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
install (TARGETS WBM20 RUNTIME DESTINATION ghaas/bin)
