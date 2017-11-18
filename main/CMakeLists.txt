project( Main )

find_package( Gcrypt REQUIRED )

include_directories( "${Gcrypt_INCLUDE_DIRS}" )

file( GLOB headers *.h )
file( GLOB sources *.cpp )

add_executable( ${PROJECT_NAME} "${headers}" "${sources}" )

target_link_libraries( ${PROJECT_NAME} ${Gcrypt_LIBRARIES} )
