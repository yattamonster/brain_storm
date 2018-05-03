#ifndef HELPER_RANDOM_H
#define HELPER_RANDOM_H

template <class T> T random()
{

}

template <> int random<int>()
{
	return rand() % 100;
}

template <> float random<float>()
{
	return static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
}

template <> char random<char>()
{
	// generates random alpha character

	const int lower = 'z' - 'a' + 1;
	const int upper = 'Z' - 'A' + 1;

	return (rand() % 2 == 0) ? ('a' + rand() % lower) : ('A' + rand() % upper);
}

template <> char* random<char*>()
{
	const int length = rand() % 10;
	char* value = new char(length);

	for (int l = 0; l < length - 1; l++)
		value[l] = random<char>();
	value[length - 1] = '\0'; // termination character

	return value;
}

template <> std::string random<std::string>()
{
	return std::string(random<char*>());
}

class Test_Person
{
public:
	std::string name;
	int age;

	Test_Person() { init(); }
	Test_Person(std::string n, int a) : name(n), age(a) {}
	void init()
	{
		// randomly initialize
		name = random<std::string>();
		age = rand() % 100;
	}

	friend std::ostream& operator<<(std::ostream& os, const Test_Person& p)
	{
		os << p.name << " (" << p.age << ")";
		return os;
	}

	bool operator==(const Test_Person& a)
	{
		if (age == a.age && name == a.name)
			return true;
		else
			return false;
	}

	bool operator>(const Test_Person& a)
	{
		if (age > a.age)
			return true;
		else
			return false;
	}

	bool operator>=(const Test_Person& a)
	{
		if (age >= a.age)
			return true;
		else
			return false;
	}

	bool operator<(const Test_Person& a)
	{
		if (age < a.age)
			return true;
		else
			return false;
	}

	bool operator<=(const Test_Person& a)
	{
		if (age <= a.age)
			return true;
		else
			return false;
	}
};

template <> Test_Person random<Test_Person>()
{
	Test_Person o; o.init();
	return o;
}

template <class T> std::vector<T> randomList(int size)
{
	std::vector<T> list(size);
	for (int i = 0; i < size; i++)
		list[i] = random<T>();
	return list;
}

#endif