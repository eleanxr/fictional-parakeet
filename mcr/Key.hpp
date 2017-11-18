#ifndef passman_Key_h_
#define passman_Key_h_

#include <algorithm>
#include <array>
#include <ostream>

namespace mcr {

/// A key with a compile-time specified length.
template< int kKeyLength >
class Key
{
public:

  using KeyArray = std::array< unsigned char, kKeyLength >;

  Key();
  ~Key();

  const KeyArray& keyData() const;
  KeyArray& keyData();

private:
  KeyArray mKeyData;
};

template< int kKeyLength >
std::ostream& operator<<( std::ostream& out, const Key< kKeyLength >& key );

} // namespace mcr

// Implementation

#include <algorithm>
#include <iomanip>
#include <iterator>

namespace mcr {

namespace detail {

template< typename T >
struct WidthPrintWrapper
{
  WidthPrintWrapper( const T& t ) : value( t ) {}
  T value;
};

template< typename T >
std::ostream& operator<<( std::ostream& out, const WidthPrintWrapper< T >& wrapper )
{
  out << std::hex << std::setfill( '0' ) << std::setw( 2 ) << wrapper.value;
  return out;
}

} // namespace detail

template< int kKeyLength >
Key< kKeyLength >::Key()
: mKeyData{}
{
}

template< int kKeyLength >
Key< kKeyLength >::~Key()
{

}

template< int kKeyLength >
auto Key< kKeyLength >::keyData() const -> const KeyArray&
{
  return mKeyData;
}

template< int kKeyLength >
auto Key< kKeyLength >::keyData() -> KeyArray&
{
  return mKeyData;
}

template< int kKeyLength >
std::ostream& operator<<( std::ostream& out, const Key< kKeyLength >& key )
{
  std::copy( key.keyData().data(), key.keyData().data() + kKeyLength,
    std::ostream_iterator< detail::WidthPrintWrapper< unsigned int > >( out, " " )
  );
  return out;
}

} // namespace mcr

#endif // include guard
