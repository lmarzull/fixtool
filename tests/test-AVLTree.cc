#include "doctest.h"
#include <AVLTree.h>
#include <SharedMemory.h>
#include <iostream>
#include <cstring>


template <typename T>
bool
operator==( const T& lhs, const T& rhs )
{
  return lhs.value == rhs.value
    && lhs.balance_factor == rhs.balance_factor;
}


struct AVL_Std
{
  using tree_t = AVLTree<int>;
  using node_t = tree_t::TreeNode;

  AVLTree<int> tree;

  node_t make_node(int val, std::int8_t bf)
  {
    return node_t{.value = val, .balance_factor = bf };
  }
};


//------------------------------------------------------------------------------
struct AVL_Shmem
{
  using tree_t = AVLTree<int, SharedMemoryAllocator<int>>;
  using node_t = tree_t::TreeNode;

  SharedMemory shmem;
  SharedMemoryAllocator<int> allocator;
  tree_t tree;

  AVL_Shmem()
      : shmem("TestUnit-AVL", 1024)
      , allocator(&shmem)
      , tree(allocator)
  {}

  node_t make_node(int val, std::int8_t bf)
  {
    return node_t{.value = val, .balance_factor = bf };
  }
};




//------------------------------------------------------------------------------
SCENARIO_TEMPLATE("Building Balanced AVL Tree - int",
  T_AVLStruct,
  AVL_Std,
  AVL_Shmem)
{
  GIVEN("An empty AVL tree")
  {
    T_AVLStruct avl;

    THEN("The root of the tree is null")
    {
      CHECK(avl.tree.root() == nullptr);
    }

    WHEN("Inserting first element")
    {
      avl.tree.insert(13);
      THEN("Balance factor is 0")
      {
        CHECK(*avl.tree.root() == avl.make_node(13,0));
      }
    }

    WHEN("Inserting root->left")
    {
      avl.tree.insert(13);
      avl.tree.insert(10);

      THEN("Balance factor of root node is -1")
      {
        CHECK(*avl.tree.root() == avl.make_node(13,-1));
      }

      THEN("Balance factor of inserted node is 0")
      {
        CHECK(*avl.tree.root()->left == avl.make_node(10,0));
      }
    }

    WHEN("Inserting root->right")
    {
      avl.tree.insert(13);
      avl.tree.insert(10);
      avl.tree.insert(15);

      THEN("Balance factor of root node is 0")
      {
        CHECK(*avl.tree.root() == avl.make_node(13,0));
      }

      THEN("Balance factor of root->left is 0")
      {
        CHECK(*avl.tree.root()->left == avl.make_node(10,0));
      }

      THEN("Balance factor of inserted node is 0")
      {
        CHECK(*avl.tree.root()->right == avl.make_node(15,0));
      }
    }

    WHEN("Inserting root->right->right")
    {
      avl.tree.insert(13);
      avl.tree.insert(10);
      avl.tree.insert(15);
      avl.tree.insert(16);

      THEN("Balance factor of root node is +1")
      {
        CHECK(*avl.tree.root() == avl.make_node(13,+1));
      }

      THEN("Balance factor of root->left is 0")
      {
        CHECK(*avl.tree.root()->left == avl.make_node(10,0));
      }

      THEN("Balance factor of root->right is +1")
      {
        CHECK(*avl.tree.root()->right == avl.make_node(15,+1));
      }

      THEN("Balance factor of inserted node is 0")
      {
        CHECK(*avl.tree.root()->right->right == avl.make_node(16,0));
      }
    }

    WHEN("Inserting root->left->left")
    {
      avl.tree.insert(13);
      avl.tree.insert(10);
      avl.tree.insert(15);
      avl.tree.insert(16);
      avl.tree.insert(5);

      THEN("Balance factor of root node is 0")
      {
        CHECK(*avl.tree.root() == avl.make_node(13,0));
      }

      THEN("Balance factor of root->left is -1")
      {
        CHECK(*avl.tree.root()->left == avl.make_node(10,-1));
      }

      THEN("Balance factor of root->left->left is 0")
      {
        CHECK(*avl.tree.root()->left->left == avl.make_node(5,0));
      }
    }

    WHEN("Inserting root->left->right")
    {
      avl.tree.insert(13);
      avl.tree.insert(10);
      avl.tree.insert(15);
      avl.tree.insert(16);
      avl.tree.insert(5);
      avl.tree.insert(11);

      THEN("Balance factor of root node is 0")
      {
        CHECK(*avl.tree.root() == avl.make_node(13,0));
      }

      THEN("Balance factor of root->left is 0")
      {
        CHECK(*avl.tree.root()->left == avl.make_node(10,0));
      }

      THEN("Balance factor of root->left->left is 0")
      {
        CHECK(*avl.tree.root()->left->left == avl.make_node(5,0));
      }

      THEN("Balance factor of root->left->right is 0")
      {
        CHECK(*avl.tree.root()->left->right == avl.make_node(11,0));
      }
    }

    WHEN("Inserting root->left->left->left")
    {
      avl.tree.insert(13);
      avl.tree.insert(10);
      avl.tree.insert(15);
      avl.tree.insert(16);
      avl.tree.insert(5);
      avl.tree.insert(11);
      avl.tree.insert(4);

      THEN("Balance factor of root node is -1")
      {
        CHECK(*avl.tree.root() == avl.make_node(13,-1));
      }

      THEN("Balance factor of root->left is -1")
      {
        CHECK(*avl.tree.root()->left == avl.make_node(10,-1));
      }

      THEN("Balance factor of root->left->left is -1")
      {
        CHECK(*avl.tree.root()->left->left == avl.make_node(5,-1));
      }

      THEN("Balance factor of root->left->right is 0")
      {
        CHECK(*avl.tree.root()->left->right == avl.make_node(11,0));
      }

      THEN("Balance factor of root->left->left->left is 0")
      {
        CHECK(*avl.tree.root()->left->left->left == avl.make_node(4,0));
      }
    }

    WHEN("Inserting root->left->left->right")
    {
      avl.tree.insert(13);
      avl.tree.insert(10);
      avl.tree.insert(15);
      avl.tree.insert(16);
      avl.tree.insert(5);
      avl.tree.insert(11);
      avl.tree.insert(4);
      avl.tree.insert(8);

      THEN("Balance factor of root node is -1")
      {
        CHECK(*avl.tree.root() == avl.make_node(13,-1));
      }

      THEN("Balance factor of root->left is -1")
      {
        CHECK(*avl.tree.root()->left == avl.make_node(10,-1));
      }

      THEN("Balance factor of root->left->left is 0")
      {
        CHECK(*avl.tree.root()->left->left == avl.make_node(5,0));
      }

      THEN("Balance factor of root->left->right is 0")
      {
        CHECK(*avl.tree.root()->left->right == avl.make_node(11,0));
      }

      THEN("Balance factor of root->left->left->left is 0")
      {
        CHECK(*avl.tree.root()->left->left->left == avl.make_node(4,0));
      }

      THEN("Balance factor of root->left->left->right is 0")
      {
        CHECK(*avl.tree.root()->left->left->right == avl.make_node(8,0));
      }
    }
  }
}



SCENARIO_TEMPLATE("Rotate right minimal unbalanced Tree with root as pivot",
  T_AVLStruct,
  AVL_Std,
  AVL_Shmem)
{
  GIVEN("A balanced AVL tree - left/left")
  {
    //            (13[-1])
    //             /
    //         (10[0])
    //
    T_AVLStruct avl;
    avl.tree.insert(13);
    avl.tree.insert(10);

    WHEN("Inserting left->left->lef")
    {
      avl.tree.insert(5);
      //
      //            (13[-2])
      //             /
      //         (10[-1])
      //          /
      //       (5[0])
      //
      THEN("Rotate right based at root")
      {
        //
        //            (10[0])
        //             /    \
        //        (5[0])   (13[0])
        //
        CHECK(*avl.tree.root() == avl.make_node(10,0));
        CHECK(*avl.tree.root()->left == avl.make_node(5,0));
        CHECK(*avl.tree.root()->right == avl.make_node(13,0));
      }
    }
  }
}


SCENARIO_TEMPLATE("Rotate right unbalanced Tree with root as pivot",
  T_AVLStruct,
  AVL_Std,
  AVL_Shmem)
{
  GIVEN("A balanced AVL tree with root balancing factor -1")
  {
    //              (13[-1])
    //              /      \
    //             /        \
    //         (10[0])    (20[0])
    //           /  \
    //      (5[0])  (11[0])
    //
    T_AVLStruct avl;
    avl.tree.insert(13);
    avl.tree.insert(10);
    avl.tree.insert(20);
    avl.tree.insert(5);
    avl.tree.insert(11);

    WHEN("Decreasing root balancing factor to -2")
    {
      avl.tree.insert(1);
      //              (13[-2])
      //              /      \
      //             /        \
      //         (10[-1])    (20[0])
      //          /    \
      //      (5[-1])  (11[0])
      //       /
      //    (1[0])

      THEN("Rotate right with root as pivot")
      {
        //              (10[0])
        //              /      \
        //             /        \
        //         (5[-1])     (13[0])
        //          /          /    \
        //      (1[0])     (11[0])  (20[0])
        //
        CHECK(*avl.tree.root() == avl.make_node(10,0));

        CHECK(*avl.tree.root()->left == avl.make_node(5,-1));
        CHECK(*avl.tree.root()->left->left == avl.make_node(1,0));

        CHECK(*avl.tree.root()->right == avl.make_node(13,0));
        CHECK(*avl.tree.root()->right->left == avl.make_node(11,0));
        CHECK(*avl.tree.root()->right->right == avl.make_node(20,0));
      }
    }
  }
}



SCENARIO_TEMPLATE("Rotate left minimal unbalanced Tree with root as pivot",
  T_AVLStruct,
  AVL_Std,
  AVL_Shmem)
{
  GIVEN("A balanced AVL tree - right/right")
  {
    //      (13[+1])
    //         \
    //          \
    //         (15[0])
    //
    T_AVLStruct avl;
    avl.tree.insert(13);
    avl.tree.insert(15);

    WHEN("Inserting right->right->right")
    {
      avl.tree.insert(20);
      //
      //      (13[+2])
      //          \
      //         (15[+1])
      //            \
      //           (20[0])
      //

      THEN("Rotate left with root as pivot")
      {
        //
        //      (15[0])
        //      /     \
        //  (13[0])   (20[0])
        //
        CHECK(*avl.tree.root() == avl.make_node(15,0));
        CHECK(*avl.tree.root()->left == avl.make_node(13,0));
        CHECK(*avl.tree.root()->right == avl.make_node(20,0));
      }
    }
  }
}


SCENARIO_TEMPLATE("Rotate left unbalanced Tree with root as pivot",
  T_AVLStruct,
  AVL_Std,
  AVL_Shmem)
{
  GIVEN("A balanced AVL tree with root balancing factor +1")
  {
    //              (13[+1])
    //              /      \
    //             /        \
    //         (10[0])     (20[0])
    //                     /     \
    //                 (15[0])  (30[0])
    //
    T_AVLStruct avl;
    avl.tree.insert(13);
    avl.tree.insert(10);
    avl.tree.insert(20);
    avl.tree.insert(15);
    avl.tree.insert(30);

    WHEN("Increasing root balancing factor to +2")
    {
      avl.tree.insert(40);
      //              (13[+2])
      //              /      \
      //             /        \
      //         (10[0])     (20[1])
      //                     /     \
      //                 (15[0])  (30[1])
      //                             \
      //                             (40[0])

      THEN("Rotate left with root as pivot")
      {
        //              (20[0])
        //              /      \
        //             /        \
        //         (13[0])      (30[1])
        //          /    \          \
        //      (10[0]) (15[0])      (40[0])
        //
        CHECK(*avl.tree.root() == avl.make_node(20,0));

        CHECK(*avl.tree.root()->left == avl.make_node(13,0));
        CHECK(*avl.tree.root()->left->left == avl.make_node(10,0));
        CHECK(*avl.tree.root()->left->right == avl.make_node(15,0));

        CHECK(*avl.tree.root()->right == avl.make_node(30,1));
        CHECK(*avl.tree.root()->right->right == avl.make_node(40,0));
      }
    }
  }
}


SCENARIO_TEMPLATE("Rotate right unbalanced Tree with not root as pivot ",
  T_AVLStruct,
  AVL_Std,
  AVL_Shmem)
{
  GIVEN("A balanced AVL tree with a node balancing factor at -1")
  {
    //              (13[+1])
    //              /      \
    //             /        \
    //         (10[-1])     (15[1])
    //           /  \          \
    //      (5[0])  (11[0])   (16[0])
    //       /  \
    //  (4[0]) (8[0])
    //
    T_AVLStruct avl;
    avl.tree.insert(13);
    avl.tree.insert(10);
    avl.tree.insert(15);
    avl.tree.insert(5);
    avl.tree.insert(11);
    avl.tree.insert(16);
    avl.tree.insert(4);
    avl.tree.insert(8);

    WHEN("Decreasing node balancing factor to -2")
    {
      avl.tree.insert(3);
      //
      //              (13[+1])
      //              /      \
      //             /        \
      //         (10[-2])     (15[1])
      //          /    \          \
      //      (5[-1])  (11[0])   (16[0])
      //       /  \
      //  (4[0]) (8[0])
      //    /
      // (3[0])
      //

      THEN("Rotate right with node where balancing factor is -2 as pivot")
      {
        //              (13[-1])
        //              /      \
        //             /        \
        //         (5[0])     (15[1])
        //          /   \          \
        //      (4[-1]) (10[0])   (16[0])
        //       /      /   \
        //  (3[0]) (8[0]) (11[0])
        //
        CHECK(*avl.tree.root() == avl.make_node(13,-1));

        CHECK(*avl.tree.root()->left == avl.make_node(5,0));
        CHECK(*avl.tree.root()->left->left == avl.make_node(4,-1));
        CHECK(*avl.tree.root()->left->left->left == avl.make_node(3,0));

        CHECK(*avl.tree.root()->left->right == avl.make_node(10,0));
        CHECK(*avl.tree.root()->left->right->left == avl.make_node(8,0));
        CHECK(*avl.tree.root()->left->right->right == avl.make_node(11,0));

        CHECK(*avl.tree.root()->right == avl.make_node(15,1));
        CHECK(*avl.tree.root()->right->right == avl.make_node(16,0));
      }
    }
  }
}


SCENARIO_TEMPLATE("Rotate left unbalanced Tree with not root as pivot ",
  T_AVLStruct,
  AVL_Std,
  AVL_Shmem)
{
  GIVEN("A balanced AVL tree with a node balancing factor at +1")
  {
    //              (13[+1])
    //              /      \
    //             /        \
    //         (10[-1])     (15[+1])
    //          /            /    \
    //       (8[0])      (14[0])   (20[0])
    //                              /    \
    //                           (17[0])  (30[0])
    //
    T_AVLStruct avl;
    avl.tree.insert(13);
    avl.tree.insert(10);
    avl.tree.insert(15);
    avl.tree.insert(8);
    avl.tree.insert(14);
    avl.tree.insert(20);
    avl.tree.insert(17);
    avl.tree.insert(30);

    WHEN("Increase node balancing factor to +2")
    {
      avl.tree.insert(40);
      //              (13[+1])
      //              /      \
      //             /        \
      //         (10[-1])     (15[+2])
      //          /            /    \
      //       (8[0])      (14[0])   (20[1])
      //                              /    \
      //                           (17[0])  (30[1])
      //                                      \
      //                                      (40[0])

      THEN("Rotate left with node where balancing factor is +2 as pivot")
      {
        //              (13[+1])
        //              /      \
        //             /        \
        //         (10[-1])     (20[0])
        //           /           /    \
        //        (8[0])     (15[0])  (30)[+1])
        //                    /   \       \
        //                (14[0])  (17[0]) (40[0])
        //
        CHECK(*avl.tree.root() == avl.make_node(13,1));

        CHECK(*avl.tree.root()->left == avl.make_node(10,-1));
        CHECK(*avl.tree.root()->left->left == avl.make_node(8,0));

        CHECK(*avl.tree.root()->right == avl.make_node(20,0));
        CHECK(*avl.tree.root()->right->left == avl.make_node(15,0));
        CHECK(*avl.tree.root()->right->left->left == avl.make_node(14,0));
        CHECK(*avl.tree.root()->right->left->right == avl.make_node(17,0));

        CHECK(*avl.tree.root()->right->right == avl.make_node(30,1));
        CHECK(*avl.tree.root()->right->right->right == avl.make_node(40,0));
      }
    }
  }
}


SCENARIO_TEMPLATE("Rotate left/right minimal unbalanced Tree with root as pivot",
  T_AVLStruct,
  AVL_Std,
  AVL_Shmem)
{
  GIVEN("A balanced AVL tree - left/left")
  {
    //              (13[-1])
    //              /
    //             /
    //         (10[0])
    //
    T_AVLStruct avl;
    avl.tree.insert(13);
    avl.tree.insert(10);

    WHEN("Decrease balancing factor of root to -2 and root->left to +1")
    {
      avl.tree.insert(11);
      //              (13[-2])
      //              /
      //         (10[+1])
      //              \
      //             (11[0])

      THEN("Rotate left with root->left as pivot and rotate right with root as pivot")
      {
        //              (11[0])
        //              /     \
        //         (10[0])   (13[0])
        //
        CHECK(*avl.tree.root() == avl.make_node(11,0));
        CHECK(*avl.tree.root()->left == avl.make_node(10,0));
        CHECK(*avl.tree.root()->right == avl.make_node(13,0));
      }
    }
  }
}



SCENARIO_TEMPLATE("Rotate left/right unbalanced Tree with not root as pivot ",
  T_AVLStruct,
  AVL_Std,
  AVL_Shmem)
{
  GIVEN("A balanced AVL tree with a node balancing factor at -1")
  {
    //              (13[+1])
    //              /      \
    //             /        \
    //         (10[-1])     (15[1])
    //           /  \          \
    //      (5[0])  (11[0])   (16[0])
    //       /  \
    //  (4[0]) (8[0])
    //
    T_AVLStruct avl;
    avl.tree.insert(13);
    avl.tree.insert(10);
    avl.tree.insert(15);
    avl.tree.insert(5);
    avl.tree.insert(11);
    avl.tree.insert(16);
    avl.tree.insert(4);
    avl.tree.insert(8);

    WHEN("Decreasing node balancing factor to -2 and node->left to +1")
    {
      avl.tree.insert(9);
      //
      //              (13[+1])
      //              /      \
      //             /        \
      //         (10[-2])     (15[1])
      //          /    \          \
      //      (5[+1])  (11[0])   (16[0])
      //       /  \
      //  (4[0]) (8[+1])
      //             \
      //             (9[0])
      //

      THEN("Rotate left with root->left->left as pivot and rotate right root->left")
      {
        //             (13[-1])
        //             /      \
        //            /        \
        //        (8[0])     (15[1])
        //        /   \          \
        //    (5[-1)  (10[0])    (16[0])
        //     /       /   \
        //  (4[0])  (9[0]) (11[0])
        //
        CHECK(*avl.tree.root() == avl.make_node(13,-1));

        CHECK(*avl.tree.root()->left == avl.make_node(8,0));
        CHECK(*avl.tree.root()->left->left == avl.make_node(5,-1));
        CHECK(*avl.tree.root()->left->left->left == avl.make_node(4,0));

        CHECK(*avl.tree.root()->left->right == avl.make_node(10,0));
        CHECK(*avl.tree.root()->left->right->left == avl.make_node(9,0));
        CHECK(*avl.tree.root()->left->right->right == avl.make_node(11,0));

        CHECK(*avl.tree.root()->right == avl.make_node(15,1));
        CHECK(*avl.tree.root()->right->right == avl.make_node(16,0));
      }
    }
  }
}


SCENARIO_TEMPLATE("Rotate right/left unbalanced Tree with not root as pivot ",
  T_AVLStruct,
  AVL_Std,
  AVL_Shmem)
{
  GIVEN("A balanced AVL tree with a node balancing factor at +1")
  {
    //              (13[+1])
    //              /      \
    //             /        \
    //         (10[-1])     (15[+1])
    //          /            /    \
    //       (8[0])      (14[0])   (20[0])
    //                              /    \
    //                           (17[0])  (30[0])
    //
    T_AVLStruct avl;
    avl.tree.insert(13);
    avl.tree.insert(10);
    avl.tree.insert(15);
    avl.tree.insert(8);
    avl.tree.insert(14);
    avl.tree.insert(20);
    avl.tree.insert(17);
    avl.tree.insert(30);

    WHEN("Increase node balancing factor to +2 and node->right to -1")
    {
      avl.tree.insert(16);
      //              (13[+1])
      //              /      \
      //             /        \
      //         (10[-1])     (15[+2])
      //          /            /    \
      //       (8[0])      (14[0])   (20[-1])
      //                              /    \
      //                           (17[0])  (30[1])
      //                            /
      //                         (16[0])

      THEN("Rotate right with node->right->right as pivot and rotate left node->right")
      {
        //              (13[+1])
        //              /      \
        //             /        \
        //         (10[-1])     (17[0])
        //           /           /    \
        //       (8[0])      (15[0])  (20[+1])
        //                   /   \         \
        //             (14[0])  (16[0])   (30[0])
        //
        CHECK(*avl.tree.root() == avl.make_node(13,1));

        CHECK(*avl.tree.root()->left == avl.make_node(10,-1));
        CHECK(*avl.tree.root()->left->left == avl.make_node(8,0));

        CHECK(*avl.tree.root()->right == avl.make_node(17,0));
        CHECK(*avl.tree.root()->right->left == avl.make_node(15,0));
        CHECK(*avl.tree.root()->right->left->left == avl.make_node(14,0));
        CHECK(*avl.tree.root()->right->left->right == avl.make_node(16,0));

        CHECK(*avl.tree.root()->right->right == avl.make_node(20,1));
        CHECK(*avl.tree.root()->right->right->right == avl.make_node(30,0));
      }
    }
  }
}



SCENARIO_TEMPLATE("find element",
  T_AVLStruct,
  AVL_Std,
  AVL_Shmem)
{
  GIVEN("An AVL")
  {
    //              (13[+1])
    //              /      \
    //             /        \
    //         (10[-1])     (15[+1])
    //          /            /    \
    //       (8[0])      (14[0])   (20[0])
    //                              /    \
    //                           (17[0])  (30[0])
    //
    T_AVLStruct avl;
    avl.tree.insert(13);
    avl.tree.insert(10);
    avl.tree.insert(15);
    avl.tree.insert(8);
    avl.tree.insert(14);
    avl.tree.insert(20);
    avl.tree.insert(17);
    avl.tree.insert(30);

    WHEN("Looking for 13")
    {
      const auto* elt = avl.tree.find(13);
      THEN("Return the root of the tree")
      {
        REQUIRE(elt == avl.tree.root());
      }
    }

    WHEN("Looking for 30")
    {
      const auto* elt = avl.tree.find(30);
      THEN("Return the node with value of 30")
      {
        REQUIRE(elt->value == 30);
      }
    }
  }
}



SCENARIO("AVL Tree allocated in SharedMemory")
{
  GIVEN("An AVL inside SharedMemory")
  {
    using tree_t = AVLTree<std::pmr::string, SharedMemoryAllocator<std::pmr::string>>;

    SharedMemory memory("UnitTest-AVL_InsideMemory", 1024);
    SharedMemoryAllocator<std::pmr::string> allocator{&memory};
    tree_t* tree = reinterpret_cast<tree_t*>(memory.allocate(sizeof(tree_t)));
    :: new (tree) tree_t(allocator);
    
    WHEN("Inserting a new string")
    {
      const char* str = "This is a new string allocated inside shared memory";
      tree->insert(str);

      THEN("Then root value is equal to the string")
      {
        CHECK(strcmp(tree->root()->value.c_str(), str) == 0);
      }
    }

    WHEN("Mapping the SharedMemory in another memory space")
    {
      const char* str = "This is a new string allocated inside shared memory";
      tree->insert(str);

      SharedMemory memory2("UnitTest-AVL_InsideMemory", 1024);
      tree_t* tree2 = reinterpret_cast<tree_t*>(std::begin(memory2));

      THEN("Both AVL are the same")
      {
        CHECK(tree->root() != tree2->root());
        CHECK(tree->root()->value == tree2->root()->value);
      }
    }
  }
}
