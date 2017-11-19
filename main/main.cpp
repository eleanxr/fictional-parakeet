
#include "mcr/Key.hpp"

#include <iostream>

#include <gcrypt.h>

#include <functional>
#include <iostream>
#include <iterator>
#include <sstream>
#include <vector>

gcry_cipher_hd_t initAes256Ecb( const mcr::Key< 32 >& key ) {
  gcry_cipher_hd_t handle;
  gcry_cipher_open( &handle, GCRY_CIPHER_AES256, GCRY_CIPHER_MODE_ECB, 0 );
  gcry_cipher_setkey( handle, key.keyData().data(), 32 );
  return handle;
}

template< int N, typename T >
using BlockFunction =
    std::function<void(const std::array<T, N> &, std::array<T, N> &)>;

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

template< std::size_t kBlockSize >
void encryptAndOutputBlock(
  const std::array< char, kBlockSize >& inBuffer,
  const std::array< char, kBlockSize >& outBuffer,
  std::ostream& output )
{

}

void encrypt( const mcr::Key< 32 >& key, std::istream& input, std::ostream& output )
{
  gcry_cipher_hd_t handle = initAes256Ecb( key );
  constexpr std::size_t kBlockSize = 16;
  std::array< char, kBlockSize > inBuffer;
  std::array< char, kBlockSize > outBuffer;
  std::streamsize streamBytes = 0;
  constexpr auto streamsizeLength = sizeof( std::streamsize );
  static_assert( streamsizeLength <= kBlockSize );
  bool lengthBlockRequired = true;
  while ( input ) {
    std::fill( inBuffer.begin(), inBuffer.end(), 0 );
    const auto charactersRead = input.read( inBuffer.data(), inBuffer.size() ).gcount();
    streamBytes += charactersRead;
    if ( input.eof() ) {
      // end of input, if there's room, write the length at the end of the block. Otherwise, add an
      // extra block.
      if ( inBuffer.size() - charactersRead > streamsizeLength ) {
        // There's enough space to write the length of the message in the final block
        std::copy(
          &streamBytes,
          &streamBytes + streamsizeLength,
          inBuffer.end() - streamsizeLength );
        lengthBlockRequired = false;
      }
    }
    gcry_cipher_encrypt(
      handle,
      outBuffer.data(),
      kBlockSize,
      inBuffer.data(),
      kBlockSize );
    output.write( outBuffer.data(), outBuffer.size() );
  }

  // Write the message length if necessary.
  if ( lengthBlockRequired ) {
    std::fill( inBuffer.begin(), inBuffer.end(), 0 );
    std::copy(
      &streamBytes,
      &streamBytes + streamsizeLength,
      inBuffer.end() - streamsizeLength );
    gcry_cipher_encrypt(
      handle,
      outBuffer.data(),
      kBlockSize,
      inBuffer.data(),
      kBlockSize );
    output.write( outBuffer.data(), outBuffer.size() );
  }
}

void decrypt( const mcr::Key< 32 >& key, std::istream& input, std::ostream& output )
{
  gcry_cipher_hd_t handle = initAes256Ecb( key );
  constexpr int kBlockSize = 16;
  std::array< char, kBlockSize > inBuffer;
  std::array< char, kBlockSize > outBuffer;

  // forBlocks< kBlockSize, char >( input, output,
  //   [&handle](
  //     const std::array< char, kBlockSize >& inBlock,
  //     std::array< char, kBlockSize >& outBlock )
  //   {
  //     gcry_cipher_decrypt(
  //       handle,
  //       outBlock.data(),
  //       kBlockSize,
  //       inBlock.data(),
  //       kBlockSize );
  //     } );

  while ( input ) {
    input.read( inBuffer.data(), inBuffer.size() );
    std::fill( outBuffer.begin(), outBuffer.end(), 0 );
    gcry_cipher_decrypt(
      handle,
      outBuffer.data(),
      kBlockSize,
      inBuffer.data(),
      kBlockSize );
    output.write( outBuffer.data(), outBuffer.size() );
  }
}

void outputBlocksInColumns( const int blockSize, std::istream& input, std::ostream& output )
{
  std::vector< unsigned char > printBuffer( blockSize );
  while ( input ) {
    std::fill( printBuffer.begin(), printBuffer.end(), 0 );
    if ( input.read( reinterpret_cast< char * >( printBuffer.data() ), printBuffer.size() ).gcount() > 0 ) {
      std::copy( printBuffer.begin(), printBuffer.end(),
        std::ostream_iterator< mcr::detail::WidthPrintWrapper< unsigned int > >( output, " " ) );
      output << std::endl;
    }
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
  std::ostringstream ciphertext;
  encrypt( key, std::cin, ciphertext );
  std::cout << std::endl;
  const auto printCiphertext = ciphertext.str();
  std::istringstream ciphertextPrintStream( printCiphertext );
  outputBlocksInColumns( 16, ciphertextPrintStream, std::cout );
  std::cout << std::endl;

  std::cout << "=====" << std::endl;
  std::istringstream ciphertextInput( printCiphertext );
  decrypt( key, ciphertextInput, std::cout );
  std::cout << std::endl;
  return 0;
}
