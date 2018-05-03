#include <assert.h>
#include <iostream>
#include <ctime>
#include <string>
#include <queue>

#include "HelperChar.h"
#include "HelperRandom.h"
#include "QuickSort.h"
#include "BinaryTree.h"
#include "BinaryTreeSort.h"

// Testing Binary Trees
template <class T> void test_BinaryTree(const std::vector<T> list)
{
	BinaryTree<T> t;

	// insert list values in the tree
	for (int i = 0; i < list.size(); i++)
		t.insert(list[i]);

	// print out tree state
	std::cout << "Printing ( h = " << t.height() << ", l = " << t.maxValueLength() << "): \n";
	t.print();
}

// Testing Sort Methods
template <class T> bool test_Sort(const Sort<T>& S, std::vector<T>& list)
{
	std::clock_t start, end;

	// sort the list using the specified sorting algorithm
	start = std::clock();
	S.sort(list, 0, list.size());
	end = std::clock();
	std::cout << "took " << (end - start) / (double)CLOCKS_PER_SEC << "s" << std::endl;

	// check that sorting was successfull
	bool success = true;
	//std::cout << "Sorted: ";
	for (int i = 0; i < list.size(); i++)
	{
		if (i > 0 && list[i - 1] > list[i])
			success = false;
		//std::cout << list[i] << " ";
	}
	//std::cout << std::endl;

	return success;
}

// Comparing Sorting Methods
template <class T> bool test_compareSort(const Sort<T>& S1, const Sort<T>& S2, int size)
{
	// insert random values in the list to be sorted
	std::vector<T> list = randomList<T>(size);

	//std::cout << "Inserting (" << size << " items): ";
	//for (int i = 0; i < size; i++)
	//	std::cout << list[i] << " ";
	//std::cout << std::endl;

	std::cout << "Sorting using " << S1.name() << " : ";
	std::vector<T> list1 = list;
	const bool s1 = test_Sort(S1,list1);

	std::cout << "Sorting using " << S2.name() << " : ";
	std::vector<T> list2 = list;
	const bool s2 = test_Sort(S2, list2);

	return s1 && s2;
}

int main()
{
	srand(time(0)); // for random generation of values to be sorted

	// compare sorting methods
	QuickSelectiveSort<int> S1;
	QuickIterativeSort<int> S2;

	if (test_compareSort(S1, S2, 100000))
		std::cout << "Comparing Sorting methods '" << S1.name() << "'/'" << S2.name() << "' succeeded!\n";
	else
		std::cout << "Warning: Comparing Sorting methods '" << S1.name() << "'/'" << S2.name() << "' failed!\n";

	return 0;
}