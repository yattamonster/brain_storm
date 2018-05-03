#ifndef HELPER_CHAR_H
#define HELPER_CHAR_H

int compareCharPointer(const char* a, const char* b)
{
	if (a == 0 || b == 0)
		return 0; // undefined behaviour

	int length = 0;
	while (a[length] != '\0' && b[length] != '\0' && a[length] == b[length])
		length++;

	if (a[length] == '\0' && b[length] == '\0')
		return 0;
	else if (a[length] < b[length])
		return -1;
	else
		return 1;
}

int lengthCharPointer(const char* a)
{
	if (a == 0)
		return 0;

	int length = 0;
	while (a[length] != '\0')
		length++;

	return length;
}

#endif
