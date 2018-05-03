#ifndef SORT_BINARY_TREE_H
#define SORT_BINARY_TREE_H

#include "BinaryTree.h"
#include "Sort.h"

template <class T> class BinaryTreeSort : public Sort<T>
{
public:

	void sort(std::vector<T>& list, int from, int to) const
	{
		// insert all list elements in a binary tree
		BinaryTree<T> BT;
		for (int i = from; i < to; i++)
			BT.insert(list[i]);
		
		BT.getSortedList(list);
	}

	std::string name() const
	{
		return std::string("Binary Tree Sort");
	}
};


#endif