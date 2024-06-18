#include<iostream>
using namespace std;

class BaseClass {
public:
	int Number;
	BaseClass() {
		cout << "BaseClass created" << endl;
	}
	~BaseClass() {
		cout << "BaseClass destoryed" << endl;
	}
};

class DerivedClass:public BaseClass{
public:
	DerivedClass() {
		cout << "DerivedClass created" << endl;
	}
	~DerivedClass() {
		cout << "DerivedClass destoryed" << endl;
	}
};

int main() {
	//BaseClass base;
	DerivedClass derived;
}