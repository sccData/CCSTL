#include <iostream>
#include "list.h"

int main() {
	CCSTL::list<int> l = {1, 2, 3, 4, 5, 6};
	for(CCSTL::list<int>::iterator it = l.begin(); it != l.end(); ++it) {
		std::cout << *it << " ";
	}
}