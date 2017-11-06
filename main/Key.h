#ifndef passman_Key_h_
#define passman_Key_h_

#include <array>
#include <ostream>

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

// Implementation

#include <algorithm>
#include <iomanip>
#include <iterator>

namespace detail {

} // namespace detail

template< int kKeyLength >
Key< kKeyLength >::Key()
: mKeyData{}
{
}

template< int kKeyLength >
Key< kKeyLength >::~Key() = default;

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
  out << std::setfill( '0' ) << std::setw( 2 ) << std::hex;
  std::copy( key.keyData().data(), key.keyData().data() + kKeyLength,
    std::ostream_iterator< unsigned int >( out, " " )
  );
  return out;
}

#endif // include guard
