#ifndef SORT_QUICK_H
#define SORT_QUICK_H

#include <queue>
#include "Sort.h"

// basic version of recursive QuickSort
template <class T> class QuickSort : public Sort<T>
{
public:

	void sort(std::vector<T>& list, int from, int to) const
	{
		assert(!list.empty());

		if (from < to)
		{
			const int pivot = partition(list, from, to);

			QuickSort<T>::sort(list, from, pivot);
			QuickSort<T>::sort(list, pivot + 1, to);
		}
	}

	std::string name() const
	{
		return std::string("Quick Sort (basic)");
	}

protected:

	void swap(std::vector<T>& list, int i, int j) const
	{
		assert(i >= 0 && i < list.size());
		assert(j >= 0 && j < list.size());

		if (i != j)
		{
			const T temp = list[i];
			list[i] = list[j];
			list[j] = temp;
		}
	}

	int partition(std::vector<T>& list, int from, int to) const
	{
		// partition list[from] until list[to-1]

		// select a pivot (always last element)
		int pivot = to - 1;

		// push bigger elements on the right and smaller on the left
		int i = from, j = to - 2; // skipping pivot at the end

		while (i < j && i < to - 2 && j >= from)
		{
			if (list[i] >= list[pivot])
				if (list[j] < list[pivot])
					swap(list, i++, j--);
				else
					j--;
			else if (list[j] < list[pivot])
				i++;
			else
			{
				i++;
				j--;
			}
		}

		// put pivot at its final location
		if (list[i] < list[pivot] && i + 1 < to)
		{
			swap(list, i + 1, pivot);
			pivot = i + 1;
		}
		else
		{
			swap(list, i, pivot);
			pivot = i;
		}

		return pivot;
	}

};

// iterative version of QuickSort with the aid of queue
template <class T> class QuickIterativeSort : public QuickSort<T>
{
private:

	typedef std::pair<int, int> Interval;

public:

	void sort(std::vector<T>& list, int from, int to) const
	{
		std::queue<Interval> sublist;
		sublist.push(Interval(from, to));

		while (!sublist.empty())
		{
			const Interval i = sublist.front();
			sublist.pop();

			if (i.first < i.second)
			{
				const int pivot = partition(list, i.first, i.second);

				sublist.push(Interval(i.first, pivot));
				sublist.push(Interval(pivot + 1, i.second));
			}
		}
	}

	std::string name() const
	{
		return std::string("Quick Sort (iterative)");
	}
};

// QuickSort using QuickSelect for finding optimal pivot
template <class T> class QuickSelectiveSort : public QuickSort<T>
{

public:

	void sort(std::vector<T>& list, int from, int to) const
	{
		assert(!list.empty());

		// optimally select the pivot, by partially sorting the list
		const int pivot = (to - from) / 2;
		quickselect(list, from, to, pivot);

		// put back at the end of the list to be picked first
		swap(list, pivot, to - 1);

		QuickSort<T>::sort(list, from, to);
	}

	std::string name() const
	{
		return std::string("Quick Sort (with Quick Select)");
	}

protected:

	template <class T> void quickselect(std::vector<T>& list, int from, int to, int pivot) const 
	{
		// find the pivot element in the list

		const int p1 = partition(list, from, to);

		if (p1 < pivot)
			quickselect(list, p1 + 1, to, pivot);
		else if (p1 > pivot)
			quickselect(list, from, p1, pivot);
	}

};

#endif