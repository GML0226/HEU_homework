//9_6.cpp
#include <iostream>
#include "LinkedList.h"
using namespace std;

int main() {
	LinkedList<int>A;

	cout << "����������A�����ݣ�";
	for (int i = 0; i < 5; i++) {
		int node;
		cin >> node;
		A.insertFront(node);
	}
	//�������
	cout << "LIST A: ";
	A.reset();

	//������ڵ�
	while (!A.endOfList()) {
		cout << A.data() << " ";
		A.next();
	}
	cout << endl << endl;

	LinkedList<int>B;

	cout << "����������B�����ݣ�";
	for (int i = 0; i < 5; i++) {
		int node;
		cin >> node;
		B.insertFront(node);
	}
	//�������
	cout << "LIST B: ";
	B.reset();

	//������ڵ�
	while (!B.endOfList()) {
		cout << B.data() << " ";
		B.next();
	}
	cout << endl << endl;


	// ���� B ���� B ��Ԫ�ؼ��� A ��β��
	for (B.reset(); !B.endOfList(); B.next()) {
		A.insertRear(B.data());
	}

	// ��֤
	cout << "��ӹ����LIST A: ";
	for (A.reset(); !A.endOfList(); A.next()) {
		cout << A.data() << " ";
	}
	cout << endl;

	return 0;
}
