
#include "mcr/Key.hpp"

#include <iostream>

#include <gcrypt.h>

#include <functional>
#include <iostream>
#include <sstream>
#include <vector>

gcry_cipher_hd_t initAes256Ecb( const mcr::Key< 32 >& key ) {
  gcry_cipher_hd_t handle;
  gcry_cipher_open( &handle, GCRY_CIPHER_AES256, GCRY_CIPHER_MODE_ECB, 0 );
  gcry_cipher_setkey( handle, key.keyData().data(), 32 );
  return handle;
}

template< int N, typename T >
using BlockFunction = std::function< void( const std::array< T, N >&, std::array< T, N >& ) >;

template< int N, typename T >
void forBlocks(
  std::istream& input,
  std::ostream& output,
  const BlockFunction< N, T >& f )
{
  std::array< T, N > inBuffer;
  std::array< T, N > outBuffer;
  while( input ) {
    std::fill( inBuffer.begin(), inBuffer.end(), 0 );
    input.read( inBuffer.data(), N );
    f( inBuffer, outBuffer );
    output.write( outBuffer.data(), N );
  }
}

void encrypt( const mcr::Key< 32 >& key, std::istream& input, std::ostream& output )
{
  gcry_cipher_hd_t handle = initAes256Ecb( key );
  const int kBlockSize = 16;

  forBlocks< kBlockSize, char >( input, output,
    [&handle](
      const std::array< char, kBlockSize >& inBlock,
      std::array< char, kBlockSize >& outBlock )
    {
    gcry_cipher_encrypt(
      handle,
      outBlock.data(),
      kBlockSize,
      inBlock.data(),
      kBlockSize );
    } );
}

void decrypt( const mcr::Key< 32 >& key, std::istream& input, std::ostream& output )
{
  gcry_cipher_hd_t handle = initAes256Ecb( key );
  const int kBlockSize = 16;

  forBlocks< kBlockSize, char >( input, output,
    [&handle](
      const std::array< char, kBlockSize >& inBlock,
      std::array< char, kBlockSize >& outBlock )
    {
    gcry_cipher_decrypt(
      handle,
      outBlock.data(),
      kBlockSize,
      inBlock.data(),
      kBlockSize );
    } );
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

  std::cout << "=====" << std::endl;
  std::istringstream ciphertextInput( printCiphertext );
  decrypt( key, ciphertextInput, std::cout );
  std::cout << std::endl;
  return 0;
}
