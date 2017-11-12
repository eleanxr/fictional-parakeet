
#include "mcr/Key.hpp"

#include <iostream>

#include <gcrypt.h>

#include <iostream>
#include <sstream>
#include <vector>

void encrypt( const mcr::Key< 32 >& key, std::istream& input, std::ostream& output )
{
  gcry_cipher_hd_t handle;
  gcry_cipher_open( &handle, GCRY_CIPHER_AES256, GCRY_CIPHER_MODE_ECB, 0 );
  gcry_cipher_setkey( handle, key.keyData().data(), 32 );

  const int kBlockSize = 16;

  std::array< char, kBlockSize > inBuffer;
  std::array< char, kBlockSize > outBuffer;
  while( input ) {
    std::fill( inBuffer.begin(), inBuffer.end(), 0 );
    input.read( inBuffer.data(), kBlockSize );
    gcry_cipher_encrypt(
      handle,
      outBuffer.data(),
      kBlockSize,
      inBuffer.data(),
      kBlockSize );
    output.write( outBuffer.data(), kBlockSize );
  }
}

int main( int argc, char * argv [] )
{
  std::string password{ "notASecret" };
  mcr::Key< 32 > key; // 256 bits.
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
  std::cout << "=====" << std::endl;
  std::istringstream message( "This is a test" );
  std::ostringstream ciphertext;
  encrypt( key, message, ciphertext );
  const auto printCiphertext = ciphertext.str();
  std::vector< unsigned char > printBuffer( printCiphertext.size() );
  std::copy( printCiphertext.begin(), printCiphertext.end(), printBuffer.begin() );
  std::copy( printBuffer.cbegin(), printBuffer.cend(),
    std::ostream_iterator< mcr::detail::WidthPrintWrapper< unsigned int > >( std::cout, " " ) );
  std::cout << std::endl;
  return 0;
}
