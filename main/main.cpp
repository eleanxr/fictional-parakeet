
#include "Key.h"

#include <iostream>

#include <gcrypt.h>

#include <iostream>

int main( int argc, char * argv [] )
{
  std::string password{ "notASecret" };
  Key< 32 > key; // 256 bits.
  gcry_kdf_derive(
    password.c_str(),
    password.size(),
    GCRY_KDF_SIMPLE_S2K,
    GCRY_MD_SHA256,
    nullptr,
    0,
    16,
    32,
    key.keyData().data() );
  std::cout << key << std::endl;
  return 0;
}
