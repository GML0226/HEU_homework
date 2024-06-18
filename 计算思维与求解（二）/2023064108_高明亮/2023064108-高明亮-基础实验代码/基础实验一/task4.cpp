#include<iostream>
#include<string>
using namespace std;

class date {
public:
	int year, month, day;
	date(int y = 0, int m = 0, int d = 0) :year(y), month(m), day(d) {}

};

class people :public date {
public:
	string name;
	int number;
	string gender;
	int id;
	people(string name = "0", int number = 0, string gender = "0", int id = 0) :name(name), number(number), gender(gender), id(id) {}

	virtual void run() {
		cout << "people class running";
	}

};

class student :virtual public people {
public:
	int classnumber;
	student(string name = "1", int number = 1, string gender = "1", int id = 1, int classnumber = 1) :people(name, number, gender, id), classnumber(classnumber) {}
};

class teacher :virtual public people {
public:
	string department;
	teacher(string name = "2", int number = 2, string gender = "2", int id = 2, string department = "2") :people(name, number, gender, id), department(department) {}

	void run() {
		cout << "teacher class running";    //重载run函数
	}
};

class researchstudent :virtual public student {
public:
	teacher a = teacher("0", 1, "0", 0, "0");
	researchstudent(string name = "3", int number = 3, string gender = "3", int id = 3, int classnumer = 3) :student(name, number, gender, id, classnumber) {}
};

class doctor :public researchstudent, public teacher {

};

int main() {
	doctor abc;
	cout << abc.id << endl;   //显示的是people类的成员值
	abc.run();  //调用距离最近的函数，在teacher类而并非people类
	return 0;
}