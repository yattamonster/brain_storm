#include "stdafx.h"
#include <iostream>
#include <ctime>
#include <string>
#include <queue>

#include "HelperChar.h"
#include "BinaryTree.h"

void test_int()
{
	std::cout << "----------------- Testing INT -----------------\n";
	BinaryTree<int> t;

	std::cout << "Inserting: ";
	srand(time(0));
	for (int i = 0; i < 10; i++)
	{
		const int value = rand() % 10;
		t.insert(value);
		std::cout << value << " ";
	}
	std::cout << std::endl;

	std::cout << "Printing (h=" << t.height() << ", l=" << t.maxValueLength() << "):\n";
	t.print();

	std::vector<int> sortedList;
	t.getSortedList(sortedList);
	std::cout << "Sorted: ";
	for (int i = 0; i < sortedList.size(); i++)
		std::cout << sortedList[i] << " ";
	std::cout << std::endl;
}

void test_float()
{
	std::cout << "----------------- Testing FLOAT -----------------\n";
	BinaryTree<float> t;

	std::cout << "Inserting: ";
	srand(time(0));
	for (int i = 0; i < 10; i++)
	{
		const float value = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		t.insert(value);
		std::cout << value << " ";
	}
	std::cout << std::endl;

	std::cout << "Printing (h=" << t.height() << ", l=" << t.maxValueLength() << "):\n";
	t.print();

	std::vector<float> sortedList;
	t.getSortedList(sortedList);
	std::cout << "Sorted: ";
	for (int i = 0; i < sortedList.size(); i++)
		std::cout << sortedList[i] << " ";
	std::cout << std::endl;
}

void test_char()
{
	std::cout << "----------------- Testing CHAR -----------------\n";
	BinaryTree<char> t;

	std::cout << "Inserting: ";
	srand(time(0));

	for (int i = 0; i < 10; i++)
	{
		char value = randomAlphaCharacter();
		t.insert(value);
		std::cout << value << " ";
	}
	std::cout << std::endl;

	std::cout << "Printing (h=" << t.height() << ", l=" << t.maxValueLength() << "):\n";
	t.print();

	std::vector<char> sortedList;
	t.getSortedList(sortedList);
	std::cout << "Sorted: ";
	for (int i = 0; i < sortedList.size(); i++)
		std::cout << sortedList[i] << " ";
	std::cout << std::endl;
}

void test_charp()
{
	std::cout << "----------------- Testing CHAR* -----------------\n";
	std::cout << "-----This does not work as is, since only--------\n";
	std::cout << "------pointers are compared and not their--------\n";
	std::cout << "--------value: use std::string instead!----------\n";
	BinaryTree<char*> t;

	std::cout << "Inserting: ";
	srand(time(0));

	for (int i = 0; i < 10; i++)
	{
		char* value = randomAlphaString(rand() % 10);
		t.insert(value);
		std::cout << value << " ";
	}
	std::cout << std::endl;

	std::vector<char*> sortedList;
	t.getSortedList(sortedList);
	std::cout << "Sorted: ";
	for (int i = 0; i < sortedList.size(); i++)
		std::cout << sortedList[i] << " ";
	std::cout << std::endl;
}

void test_string()
{
	std::cout << "----------------- Testing STRING -----------------\n";
	BinaryTree<std::string> t;

	std::cout << "Inserting: ";
	srand(time(0));

	for (int i = 0; i < 10; i++)
	{
		char* value = randomAlphaString(rand() % 10);
		t.insert(std::string(value));
		std::cout << value << " ";
	}
	std::cout << std::endl;

	std::vector<std::string> sortedList;
	t.getSortedList(sortedList);
	std::cout << "Sorted: ";
	for (int i = 0; i < sortedList.size(); i++)
		std::cout << sortedList[i] << " ";
	std::cout << std::endl;
}

void test_object()
{
	std::cout << "----------------- Testing OBJECT -----------------\n";
	class Person
	{
	public:
		std::string name;
		int age;

		Person() : name("undefined"), age(0) {}
		Person(std::string n, int a) : name(n), age(a) {}

		bool operator==(const Person& a)
		{
			if (age == a.age && name == a.name)
				return true;
			else
				return false;
		}

		bool operator>(const Person& a)
		{
			if (age > a.age)
				return true;
			else
				return false;
		}

		bool operator<(const Person& a)
		{
			if (age < a.age)
				return true;
			else
				return false;
		}
	};

	BinaryTree<Person> t;

	t.insert(Person("mike", 30));
	t.insert(Person("susanne", 31));
	t.insert(Person("paul", 2));
	t.insert(Person("jack", 59));
	t.insert(Person("george", 59));
	t.insert(Person("matt", 61));
	t.insert(Person("sean", 60));

	std::vector<Person> sortedList;
	t.getSortedList(sortedList);
	std::cout << "Sorted: ";
	for (int i = 0; i < sortedList.size(); i++)
		std::cout << sortedList[i].name << " ";
	std::cout << std::endl;
}

int _tmain(int argc, _TCHAR* argv[])
{
	test_int();
	test_float();
	test_char();
	test_charp();
	test_string();
	test_object();

	return 0;
}

