#ifndef BINARY_TREE_H
#define BINARY_TREE_H

#include <sstream>

template <class T> struct Node
{
	T value;
	size_t count;

	Node* parent;
	Node* left;
	Node* right;

	Node(T v, Node* p, Node* l, Node* r)
	{
		value = v;
		count = 1;

		parent = p;
		left = l;
		right = r;
	}
};

template <class T> class BinaryTree
{
	// list of nodes with connections
	Node<T> * _root;

public:

	BinaryTree() : _root(0) {}
	~BinaryTree() { delete_recursive(_root); }

	void insert(T value)
	{
		if (_root == 0)
			_root = new Node<T>(value, 0, 0, 0);
		else
			insert_recursive(value, _root);
	}

	bool find(T value)
	{
		if (_root == 0)
			return false;
		else
			return find_recursive(value, _root);
	}

	int height()
	{
		return height_recursive(_root);
	}

	int height(T value)
	{
		return height_recursive(value, _root, 1).second;
	}

	int maxValueLength()
	{
		return maxValueLength_recursive(_root);
	}

	void getSortedList(std::vector<T>& sorted)
	{
		sorted.clear();
		getSortedList_recursive(_root, sorted);
	}

	void print()
	{
		// implements breadth-first tree traversal
		const int h = height();
		const int l = maxValueLength();
		int count = 0;
		int height = 0;

		std::queue<Node<T>*> q;

		if (_root != 0)
			q.push(_root);

		while (!q.empty() && height <= h)
		{
			Node<T>* node = q.front(); q.pop();
			count++;

			// new set of children on the same line
			if ((count & (count - 1)) == 0) // power of 2 reached
			{
				height++;
				printf("\n");
			}

			// print value and spaces after
			if (node != 0)
			{
				std::cout << "(" << node->value << "," << node->count << ")";
				for (int s = 0; s < ((std::pow(2, (h - height)) - 1) * l); s++)
					std::cout << " ";
			}
			else
			{
				for (int s = 0; s < ((std::pow(2, (h - height))) * l); s++)
					std::cout << " ";
			}

			// put children in queue
			if (node != 0)
			{
				q.push(node->left);
				q.push(node->right);
			}
			else
			{
				q.push(0);
				q.push(0);
			}
		}
	}

private:

	bool find_recursive(T value, Node<T>* node)
	{
		if (value == node->value)
			return true;
		else if (value > node->value)
			if (node->right != 0)
				return find_recursive(value, node->right);
			else
				return false;
		else
			if (node->left != 0)
				return find_recursive(value, node->left);
			else
				return false;
	}

	void insert_recursive(T value, Node<T>* node)
	{
		if (value == node->value)
			node->count++;
		else if (value > node->value)
			if (node->right != 0)
				insert_recursive(value, node->right);
			else
				node->right = new Node<T>(value, node, 0, 0);
		else
			if (node->left != 0)
				insert_recursive(value, node->left);
			else
				node->left = new Node<T>(value, node, 0, 0);
	}

	void delete_recursive(Node<T>* node)
	{
		if (node != 0)
		{
			delete_recursive(node->left);
			delete_recursive(node->right);
			delete node;
		}
	}

	int height_recursive(Node<T>* node)
	{
		if (node == 0)
			return 0;

		return 1 + std::max(height_recursive(node->left), height_recursive(node->right));
	}

	std::pair<bool, int> height_recursive(T value, Node<T>* node, int height)
	{
		if (node == 0)
			return std::pair<bool, int>(false, -1);

		if (value == node->value)
			return std::pair<bool, int>(true, height);
		else if (value > node->value)
			if (node->right != 0)
				return height_recursive(value, node->right, height + 1);
			else
				return std::pair<bool, int>(false, -1);
		else
			if (node->left != 0)
				return height_recursive(value, node->left, height + 1);
			else
				return std::pair<bool, int>(false, -1);
	}

	int maxValueLength_recursive(Node<T>* node)
	{
		if (node == 0)
			return 0;

		std::ostringstream o; o << node->value;
		const int length = (o.str()).length();
		return std::max(length, std::max(maxValueLength_recursive(node->left), maxValueLength_recursive(node->right)));
	}

	void getSortedList_recursive(Node<T>* node, std::vector<T>& sorted)
	{
		if (node != 0)
		{
			getSortedList_recursive(node->left, sorted);
			for (int c = 0; c < node->count; c++)
				sorted.push_back(node->value);
			getSortedList_recursive(node->right, sorted);
		}
	}
};

#endif
