#ifndef __OFFSET_PTR_H__
#define __OFFSET_PTR_H__

#include <iostream>

//------------------------------------------------------------------------------
template <typename T_pointed_type>
class OffsetPtr
{
public:
  using element_type  = T_pointed_type;
  using offset_ptr    = OffsetPtr<element_type>;


  OffsetPtr() = default;
  OffsetPtr(T_pointed_type*);
  OffsetPtr(const offset_ptr& );
  OffsetPtr(offset_ptr&& );

  std::size_t offset() const { return _offset; }

  T_pointed_type*       operator->();
  const T_pointed_type* operator->() const;

  T_pointed_type&       operator*();
  const T_pointed_type& operator*() const;


  offset_ptr& operator=( T_pointed_type* ptr );
  offset_ptr& operator=( const offset_ptr& ptr );
  offset_ptr& operator=( offset_ptr&& ptr );


private:
  std::ptrdiff_t  _offset{};

  const T_pointed_type* to_address() const;
  std::ptrdiff_t to_offset(const T_pointed_type* elt) const;
};


//------------------------------------------------------------------------------
template <typename T_pointed_type>
OffsetPtr<T_pointed_type>::OffsetPtr(T_pointed_type* elt)
{
  _offset = to_offset(elt);
}


//------------------------------------------------------------------------------
template <typename T_pointed_type>
OffsetPtr<T_pointed_type>::OffsetPtr(const offset_ptr& copy)
{
  _offset = to_offset(copy.to_address());
}


//------------------------------------------------------------------------------
template <typename T_pointed_type>
OffsetPtr<T_pointed_type>::OffsetPtr(offset_ptr&& move)
{
  _offset = to_offset(move.to_address());
}


//------------------------------------------------------------------------------
template <typename T_pointed_type>
T_pointed_type*
OffsetPtr<T_pointed_type>::operator->()
{
  return const_cast<T_pointed_type*>(
      reinterpret_cast<const OffsetPtr<T_pointed_type>*>(this)->operator->());
}


//------------------------------------------------------------------------------
template <typename T_pointed_type>
const T_pointed_type*
OffsetPtr<T_pointed_type>::operator->() const
{
  return to_address();
}


//------------------------------------------------------------------------------
template <typename T_pointed_type>
T_pointed_type&
OffsetPtr<T_pointed_type>::operator*()
{
  return const_cast<T_pointed_type&>(
      reinterpret_cast<const OffsetPtr<T_pointed_type>*>(this)->operator*()
      );
}


//------------------------------------------------------------------------------
template <typename T_pointed_type>
const T_pointed_type&
OffsetPtr<T_pointed_type>::operator*() const
{
  return *to_address();
}


//------------------------------------------------------------------------------
template <typename T_pointed_type>
auto
OffsetPtr<T_pointed_type>::operator=(T_pointed_type* ptr) -> offset_ptr&
{
  _offset = to_offset(ptr);
  return *this;
}


//------------------------------------------------------------------------------
template <typename T_pointed_type>
auto
OffsetPtr<T_pointed_type>::operator=(const offset_ptr& from) -> offset_ptr&
{
  _offset = to_offset(from.to_address());
  return *this;
}


//------------------------------------------------------------------------------
template <typename T_pointed_type>
auto
OffsetPtr<T_pointed_type>::operator=(offset_ptr&& from) -> offset_ptr&
{
  _offset = to_offset(from.to_address());
  return *this;
}



//------------------------------------------------------------------------------
template <typename T_pointed_type>
const T_pointed_type*
OffsetPtr<T_pointed_type>::to_address() const
{
  if (_offset == 0)
    return nullptr;
  return reinterpret_cast<const T_pointed_type*>(
      reinterpret_cast<const std::byte*>(this) + _offset
      );
}



//------------------------------------------------------------------------------
template <typename T_pointed_type>
std::ptrdiff_t
OffsetPtr<T_pointed_type>::to_offset(const T_pointed_type* elt) const
{
  if (elt == nullptr)
    return 0;
  return 
    reinterpret_cast<const std::byte*>(elt)
    - reinterpret_cast<const std::byte*>(this);
}



//------------------------------------------------------------------------------
template <typename T>
bool
operator==( const OffsetPtr<T>& lhs, const void* ptr )
{
  return std::to_address(lhs) == ptr;
}



//------------------------------------------------------------------------------
template <typename T>
bool
operator!=( const OffsetPtr<T>& lhs, const void* ptr )
{
  return !(lhs == ptr);
}



//------------------------------------------------------------------------------
template <typename T>
std::ostream& operator<<(std::ostream& os, const OffsetPtr<T>& ptr )
{
  os << "@[" << ptr.offset() << ']';
  return os;
}


#endif // #ifndef __OFFSET_PTR_H__
