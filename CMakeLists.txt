cmake_minimum_required( VERSION 3.6 )

project( PasswordManager )

list( APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake )

include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )

add_subdirectory( main )
add_subdirectory( mcr )

