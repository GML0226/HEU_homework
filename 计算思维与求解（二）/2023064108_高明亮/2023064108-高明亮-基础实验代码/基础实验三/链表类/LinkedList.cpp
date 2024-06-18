//9_6.cpp
#include <iostream>
#include "LinkedList.h"
using namespace std;

int main() {
	LinkedList<int>A;

	cout << "请输入链表A的内容：";
	for (int i = 0; i < 5; i++) {
		int node;
		cin >> node;
		A.insertFront(node);
	}
	//输出链表
	cout << "LIST A: ";
	A.reset();

	//输出各节点
	while (!A.endOfList()) {
		cout << A.data() << " ";
		A.next();
	}
	cout << endl << endl;

	LinkedList<int>B;

	cout << "请输入链表B的内容：";
	for (int i = 0; i < 5; i++) {
		int node;
		cin >> node;
		B.insertFront(node);
	}
	//输出链表
	cout << "LIST B: ";
	B.reset();

	//输出各节点
	while (!B.endOfList()) {
		cout << B.data() << " ";
		B.next();
	}
	cout << endl << endl;


	// 遍历 B 并把 B 中元素加入 A 的尾部
	for (B.reset(); !B.endOfList(); B.next()) {
		A.insertRear(B.data());
	}

	// 验证
	cout << "添加过后的LIST A: ";
	for (A.reset(); !A.endOfList(); A.next()) {
		cout << A.data() << " ";
	}
	cout << endl;

	return 0;
}
