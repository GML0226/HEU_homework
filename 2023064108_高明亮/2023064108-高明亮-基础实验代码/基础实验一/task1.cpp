#include<iostream>
using namespace std;

class Animal {
public:           //���ｫage����Ϊ˽�г�Աʱ�������๫�м̳�ʱ���ɼ�����age��Ϊ���г�Ա����
	int age;
};

class dog : public Animal {
public:
	void SetAge(int n) {
		cout << "����������:";
		cin >> age;
		cout << "������:" << age;
	}
};

int main() {
	dog DOG;
	cout << "����Ĭ�ϳ�ʼ����Ϊ0" << endl;
	DOG.SetAge(0);

}