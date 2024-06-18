#include<iostream>
#include"queue.h"

using namespace std;

int main() {

	Queue<int, 5> queue;
	
	cout << "请输入5个整数进入队列:";
	for (int i = 0; i < 5; i++) {
		int num;
		cin >> num;
		queue.insert(num);
	}
	cout << endl;

	cout << "队列依次输出的数:";
	while (!queue.isEmpty()) {
		cout << queue.getFront() << "  ";
		queue.remove();
	}
	cout << endl;

}