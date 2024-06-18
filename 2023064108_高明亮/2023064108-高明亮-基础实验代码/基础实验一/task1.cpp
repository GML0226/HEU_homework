#include<iostream>
using namespace std;

class Animal {
public:           //这里将age设置为私有成员时，派生类公有继承时不可见，将age改为公有成员即可
	int age;
};

class dog : public Animal {
public:
	void SetAge(int n) {
		cout << "请输入年龄:";
		cin >> age;
		cout << "年龄是:" << age;
	}
};

int main() {
	dog DOG;
	cout << "年龄默认初始设置为0" << endl;
	DOG.SetAge(0);

}