#include<iostream>
#include<string>
using namespace std;

// date�࣬��ʾ����
class date {
public:
    int year, month, day;
    date(int y = 0, int m = 0, int d = 0) :year(y), month(m), day(d) {}
};

// people�࣬�̳���date�࣬��ʾ��
class people : public date {
public:
    string name;
    int number;
    string gender;
    int id;

    // ���캯��
    people(string name = "0", int number = 0, string gender = "0", int id = 0)
        : name(name), number(number), gender(gender), id(id) {}

    // ����==��������Ƚ�����people�����id����
    bool operator==(const people& other) const {
        return this->id == other.id;
    }

    // ���ظ�ֵ�������ʵ��people�����ĸ�ֵ����
    people& operator=(const people& other) {
        if (this != &other) {
            this->name = other.name;
            this->number = other.number;
            this->gender = other.gender;
            this->id = other.id;
            this->year = other.year;
            this->month = other.month;
            this->day = other.day;
        }
        return *this;
    }
};

// student�࣬��̳���people�࣬��ʾѧ��
class student : virtual public people {
public:
    int classnumber;
    student(string name = "1", int number = 1, string gender = "1", int id = 1, int classnumber = 1)
        : people(name, number, gender, id), classnumber(classnumber) {}
};

// teacher�࣬��̳���people�࣬��ʾ��ʦ
class teacher : virtual public people {
public:
    string department;
    teacher(string name = "2", int number = 2, string gender = "2", int id = 2, string department = "2")
        : people(name, number, gender, id), department(department) {}

    // ʾ����Ա����
    void run() {
        cout << "teacher class running" << endl;
    }
};

// researchstudent�࣬��̳���student�࣬��ʾ�о���
class researchstudent : virtual public student {
public:
    teacher a = teacher("0", 1, "0", 0, "0");
    researchstudent(string name = "3", int number = 3, string gender = "3", int id = 3, int classnumber = 3)
        : student(name, number, gender, id, classnumber) {}
};

// doctor�࣬�̳���researchstudent��teacher�࣬��ʾ��ʿ��
class doctor : public researchstudent, public teacher {
public:
    doctor(string name = "4", int number = 4, string gender = "4", int id = 4, int classnumber = 4, string department = "4")
        : people(name, number, gender, id), researchstudent(name, number, gender, id, classnumber), teacher(name, number, gender, id, department) {}
};

int main() {
    // ����doctor�����
    doctor def("HEU", 123, "Male", 211, 789, "Physics");
    doctor abc("HIT", 456, "Female", 985, 101, "Math");

    cout << "HEU's ID before assignment: " << def.id << endl;
    cout << "HIT's ID before assignment: " << abc.id << endl;

    if (abc == def) {
        cout << "HEU and HIT have the same ID." << endl;
    }
    else {
        cout << "HEU and HIT have different ID." << endl;
    }

    // ���Ը�ֵ�����
    def = abc;
    cout << "HEU's ID after assignment: " << def.id << endl;

    // ���Ե��������
    if (abc == def) {
        cout << "HEU and HIT have the same ID." << endl;
    }
    else {
        cout << "HEU and HIT have different ID." << endl;
    }

}
