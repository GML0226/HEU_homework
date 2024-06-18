#include<iostream>
#include"queue.h"

using namespace std;

int main() {

	Queue<int, 5> queue;
	
	cout << "������5�������������:";
	for (int i = 0; i < 5; i++) {
		int num;
		cin >> num;
		queue.insert(num);
	}
	cout << endl;

	cout << "���������������:";
	while (!queue.isEmpty()) {
		cout << queue.getFront() << "  ";
		queue.remove();
	}
	cout << endl;

}