#ifndef SORT_H
#define SORT_H

template <class T> class Sort
{
public:

	virtual std::string name() const = 0;
	virtual void sort(std::vector<T>& list, int from, int to) const = 0;
};

#endif