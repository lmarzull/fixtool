#ifndef __AVLTree_H__
#define __AVLTree_H__


#include <memory>
#include <ManagedMemory.h>
#include <OffsetPtr.h>
#include <functional>
#include <iostream>



namespace detail
{
//  template <typename, typename = void>
//  struct has_to_address : std::false_type
//  {};
  template <typename, typename = void>
  struct has_allocator : std::false_type
  {
    using type = std::false_type;
  };


//  template <typename T>
//  struct has_to_address<
//    T,
//    std::void_t<decltype(T::to_address())>> : std::true_type
//  {
//    using type = std::true_type;
//  };
//
//
  template <typename T>
  struct has_allocator<
    T,
    std::void_t<decltype(std::declval<T>().get_allocator())>> : std::true_type
  {
    using type = std::true_type;
  };
}




template <typename T_value,
          typename T_allocator = std::allocator<T_value>>
class AVLTree
{
public:
  struct TreeNode;

  using value_alloc_traits  = typename std::allocator_traits<T_allocator>;
  using node_alloc_traits   = typename value_alloc_traits::template rebind_traits<TreeNode>;
  using node_allocator      = typename value_alloc_traits::template rebind_alloc<TreeNode>;
  using node_ptr = node_alloc_traits::pointer;
  using const_node_ptr = node_alloc_traits::const_pointer;
  using node_pointer_traits = std::pointer_traits<node_ptr>;

  struct TreeNode
  {
    T_value   value;
    node_ptr  left;
    node_ptr  right;
    std::int8_t balance_factor;
    friend std::ostream& operator<<( std::ostream& os, const TreeNode& node)
    {
      os << '(' << node.value << "[" << static_cast<int16_t>(node.balance_factor) << "])";
      return os;
    }
  };


  AVLTree();
  AVLTree(const T_allocator& allocator);


  const TreeNode*  root() const;
  const TreeNode*  insert(const T_value&);

  const TreeNode*  find(const T_value& value) const
  {
    return std::to_address(_find(_root, value));
  }


private:
  node_ptr _root;
  T_allocator _allocator;


//  static const TreeNode* to_address(const node_ptr ptr, std::true_type)
//  {
//    return node_pointer_traits::to_address(ptr);
//  }
//
//  static const TreeNode* to_address(const node_ptr ptr, std::false_type)
//  {
//    return ptr;
//  }
//
//  static const TreeNode* to_address(const node_ptr ptr)
//  {
//    return std::to_address(ptr);
//  }


  node_allocator get_node_allocator(std::true_type)
  {
    typename std::allocator_traits<T_allocator>::template rebind_alloc<TreeNode> alloc(_allocator.resource());
    return alloc;
  }

  node_allocator get_node_allocator(std::false_type)
  {
    typename std::allocator_traits<T_allocator>::template rebind_alloc<TreeNode> alloc;
    return alloc;
  }

  TreeNode* _allocate_nodes(std::size_t nbr)
  {
    return get_node_allocator(
      std::integral_constant<bool,
        std::is_base_of<std::pmr::polymorphic_allocator<T_value>,
                        T_allocator>::value>())
        .allocate(nbr);
  }

  T_value _copy(const T_value& value, std::true_type)
  {
    return T_value(value, _allocator);
  }

  T_value _copy(const T_value& value, std::false_type)
  {
    return T_value(value);
  }

  bool _do_insert(node_ptr& root, TreeNode* val);
  node_ptr _rotate_right(node_ptr) const;
  node_ptr _rotate_left(node_ptr) const;

  node_ptr _find(node_ptr, const T_value&) const;
};



//------------------------------------------------------------------------------
template <typename T_value, typename T_allocator>
AVLTree<T_value, T_allocator>::AVLTree()
  : _root(nullptr)
  , _allocator()
{
}



//------------------------------------------------------------------------------
template <typename T_value, typename T_allocator>
AVLTree<T_value, T_allocator>::AVLTree( const T_allocator& allocator )
  : _root(nullptr)
  , _allocator(allocator)
{
}



//------------------------------------------------------------------------------
template <typename T_value, typename T_allocator>
auto
AVLTree<T_value, T_allocator>::root() const -> const TreeNode*
{
  return std::to_address(_root);
}



//------------------------------------------------------------------------------
template <typename T_value, typename T_allocator>
auto
AVLTree<T_value, T_allocator>::insert(const T_value& val) -> const TreeNode*
{
  TreeNode* new_node = _allocate_nodes(1);
  ::new (new_node) TreeNode;
  new_node->value = std::move(_copy(val, detail::has_allocator<T_value>()));
  new_node->balance_factor = 0;
  new_node->left = nullptr;
  new_node->right = nullptr;
  if (_root == nullptr)
    _root = new_node;
  else
    _do_insert(_root, new_node);
  return new_node;
}



//------------------------------------------------------------------------------
template <typename T_value, typename T_allocator>
bool
AVLTree<T_value, T_allocator>::_do_insert(
    node_ptr& node,
    TreeNode* val)
{
  auto bf = node->balance_factor;
  std::uint8_t height_change = 0;
  if (val->value < node->value)
  {
    if (node->left == nullptr) {
      node->left = val;
      height_change = -1;
    }
    else {
      if (_do_insert(node->left, val))
        height_change = -1;
    }
  }
  else
  {
    if (node->right == nullptr) {
      node->right = val;
      height_change = 1;
    }
    else {
      if(_do_insert(node->right, val)){
        height_change= 1;
      }
    }
  }
  node->balance_factor += height_change;

  if (node->balance_factor == -2 && node->left != nullptr && node->left->balance_factor == -1) {
    node = _rotate_right(node);
  } else if (node->balance_factor == 2 && node->right != nullptr && node->right->balance_factor == 1) {
    node = _rotate_left(node);
  } else if (node->balance_factor == -2 && node->left != nullptr && node->left->balance_factor == 1) {
    node->left = _rotate_left(node->left);
    node = _rotate_right(node);
  } else if (node->balance_factor == 2 && node->right != nullptr && node->right->balance_factor == -1) {
    node->right = _rotate_right(node->right);
    node = _rotate_left(node);
  }

  return node->balance_factor != 0 && node->balance_factor != bf;
}


//------------------------------------------------------------------------------
template <typename T_value, typename T_allocator>
auto
AVLTree<T_value, T_allocator>::_rotate_right(node_ptr pivot) const -> node_ptr
{
  node_ptr new_node = pivot->left;
  node_ptr right = new_node->right;
  new_node->right = pivot;
  pivot->left = right;

  if (pivot->right == nullptr && pivot->left != nullptr) {
    pivot->balance_factor = -1;
  }
  else if (pivot->left == nullptr && pivot->right != nullptr) {
    pivot->balance_factor = +1;
  }
  else {
    pivot->balance_factor = 0;
  }
  new_node->balance_factor = 0;
  return new_node;
}


//------------------------------------------------------------------------------
template <typename T_value, typename T_allocator>
auto
AVLTree<T_value, T_allocator>::_rotate_left(node_ptr pivot) const -> node_ptr
{
  node_ptr new_node = pivot->right;
  node_ptr left = new_node->left;
  new_node->left = pivot;
  pivot->right = left;

  if (pivot->right == nullptr && pivot->left != nullptr) {
    pivot->balance_factor = -1;
  }
  else if (pivot->left == nullptr && pivot->right != nullptr) {
    pivot->balance_factor = +1;
  }
  else {
    pivot->balance_factor = 0;
  }
  new_node->balance_factor = 0;
  return new_node;
}


//------------------------------------------------------------------------------
template <typename T_value, typename T_allocator>
auto
AVLTree<T_value, T_allocator>::_find(
    node_ptr starting_node,
    const T_value& value) const -> node_ptr
{
  if (starting_node==nullptr)
    return nullptr;

  if (value < starting_node->value)
    return _find(starting_node->left, value);
  else if (value > starting_node->value)
    return _find(starting_node->right, value);

  return starting_node;
}


#endif  // #ifndef __AVLTree_H__
