# Find the GCrypt library

include( FindPackageHandleStandardArgs )

find_file( Gcrypt_INCLUDE_DIR NAMES gcrypt.h )
set( Gcrypt_INCLUDE_DIRS "${Gcrypt_INCLUDE_DIR}" )

find_library( Gcrypt_gcrypt_LIBRARY NAMES gcrypt )

set( Gcrypt_LIBRARIES "${Gcrypt_gcrypt_LIBRARY}" )

find_package_handle_standard_args( Gcrypt DEFAULT_MSG
  Gcrypt_LIBRARIES Gcrypt_INCLUDE_DIRS )

