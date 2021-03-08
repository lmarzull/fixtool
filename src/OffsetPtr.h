#ifndef __OFFSET_PTR_H__
#define __OFFSET_PTR_H__

#include <iostream>

//------------------------------------------------------------------------------
template <typename T_pointed_type>
class OffsetPtr final
{
public:
  using element_type    = T_pointed_type;
  using pointer         = T_pointed_type*;
  using const_pointer   = const T_pointed_type*;
  using difference_type = std::ptrdiff_t;
  using offset_ptr_type = OffsetPtr<element_type>;


  OffsetPtr() = default;
  OffsetPtr(pointer);
  OffsetPtr(const offset_ptr_type& );


  difference_type offset() const noexcept { return _offset; }


  pointer operator->() noexcept;
  const_pointer operator->() const noexcept;

  element_type&       operator*() noexcept;
  const element_type& operator*() const noexcept;


  offset_ptr_type& operator=( pointer ptr ) noexcept;
  offset_ptr_type& operator=( const offset_ptr_type& ptr ) noexcept;


private:
  difference_type  _offset{};

  const_pointer to_address() const noexcept;
  difference_type to_offset(const_pointer elt) const noexcept;
};


//------------------------------------------------------------------------------
template <typename T_pointed_type>
OffsetPtr<T_pointed_type>::OffsetPtr(pointer elt)
  : _offset(to_offset(elt))
{}


//------------------------------------------------------------------------------
template <typename T_pointed_type>
OffsetPtr<T_pointed_type>::OffsetPtr(const offset_ptr_type& copy)
  : _offset(to_offset(copy.to_address()))
{}


//------------------------------------------------------------------------------
template <typename T_pointed_type>
auto
OffsetPtr<T_pointed_type>::operator->() noexcept -> pointer
{
  return const_cast<pointer>(
      reinterpret_cast<const OffsetPtr<element_type>*>(this)->operator->());
}


//------------------------------------------------------------------------------
template <typename T_pointed_type>
auto
OffsetPtr<T_pointed_type>::operator->() const noexcept -> const_pointer
{
  return to_address();
}


//------------------------------------------------------------------------------
template <typename T_pointed_type>
auto
OffsetPtr<T_pointed_type>::operator*() noexcept -> element_type&
{
  return const_cast<element_type&>(
      reinterpret_cast<const offset_ptr_type*>(this)->operator*()
      );
}


//------------------------------------------------------------------------------
template <typename T_pointed_type>
auto
OffsetPtr<T_pointed_type>::operator*() const noexcept -> const element_type&
{
  return *to_address();
}


//------------------------------------------------------------------------------
template <typename T_pointed_type>
auto
OffsetPtr<T_pointed_type>::operator=(pointer ptr) noexcept -> offset_ptr_type&
{
  _offset = to_offset(ptr);
  return *this;
}


//------------------------------------------------------------------------------
template <typename T_pointed_type>
auto
OffsetPtr<T_pointed_type>::operator=(const offset_ptr_type& from) noexcept -> offset_ptr_type&
{
  _offset = to_offset(from.to_address());
  return *this;
}


//------------------------------------------------------------------------------
template <typename T_pointed_type>
auto
OffsetPtr<T_pointed_type>::to_address() const noexcept -> const_pointer
{
  if (_offset == 0)
    return nullptr;
  return reinterpret_cast<const_pointer>(
      reinterpret_cast<const std::byte*>(this) + _offset
      );
}



//------------------------------------------------------------------------------
template <typename T_pointed_type>
auto
OffsetPtr<T_pointed_type>::to_offset(const_pointer elt) const noexcept -> difference_type
{
  if (elt == nullptr)
    return static_cast<difference_type>(0);
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
