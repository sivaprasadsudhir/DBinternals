#include <math.h>
#include <iostream>
#include <stdlib.h>
using namespace std;
int main(int argc, char **argv)
{
	int a = atoi(argv[1]);
	for (int i = 0; i < a; ++i)
	{
		cout << i << endl;
	}
	return 0;
}