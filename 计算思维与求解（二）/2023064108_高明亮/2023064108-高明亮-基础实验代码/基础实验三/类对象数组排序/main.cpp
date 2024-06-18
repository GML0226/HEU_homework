#include <iostream>
#include <string>
#include <algorithm>

using namespace std;

// ����������
class date {
public:
    int year, month, day;
    date(int y = 0, int m = 0, int d = 0) : year(y), month(m), day(d) {}
};

// ������Ա�࣬�������ڶ���
class people {
public:
    string name;
    int number;
    string gender;
    int id;
    date birthday;  // �������ڶ���

    people(string name = "0", int number = 0, string gender = "0", int id = 0, date birthday = date())
        : name(name), number(number), gender(gender), id(id), birthday(birthday) {}
};

// ����ѧ���࣬�̳���Ա��
class student : public people {
public:
    int classnumber;
    student(string name = "1", int number = 1, string gender = "1", int id = 1, int classnumber = 1, date birthday = date())
        : people(name, number, gender, id, birthday), classnumber(classnumber) {}
};

// �����ʦ�࣬�̳���Ա��
class teacher : public people {
public:
    string department;
    teacher(string name = "2", int number = 2, string gender = "2", int id = 2, string department = "2", date birthday = date())
        : people(name, number, gender, id, birthday), department(department) {}
};

// �� number ���Զ���Ա���������������ĺ���
void sortPeopleArray(people* peopleArray[], int size) {
    sort(peopleArray, peopleArray + size, [](people* p1, people* p2) {
        return p1->number < p2->number;
        });
}

// �� number ���Բ�����Ա����ĺ���
people* searchPeopleByNumber(people* peopleArray[], int size, int number) {
    for (int i = 0; i < size; ++i) {
        if (peopleArray[i]->number == number) {
            return peopleArray[i];
        }
    }
    return nullptr;
}

int main() {
    // ������ʦ����
    teacher* teachers[] = {
        new teacher("A", 3, "Female", 1001, "Math", date(1980, 5, 1)),
        new teacher("B", 1, "Male", 1002, "Science", date(1975, 8, 23)),
        new teacher("C", 2, "Male", 1003, "History", date(1982, 11, 15))
    };
    const int teachersSize = sizeof(teachers) / sizeof(teachers[0]);

    // ����ѧ������
    student* students[] = {
        new student("D", 6, "Male", 2001, 101, date(2000, 2, 14)),
        new student("E", 5, "Female", 2002, 102, date(2001, 7, 30)),
        new student("F", 4, "Male", 2003, 103, date(1999, 12, 5))
    };
    const int studentsSize = sizeof(students) / sizeof(students[0]);

    // �� number �Խ�ʦ��ѧ����������
    sortPeopleArray(reinterpret_cast<people**>(teachers), teachersSize);
    sortPeopleArray(reinterpret_cast<people**>(students), studentsSize);

    // ��ʾ�����Ľ�ʦ
    cout << "�����Ľ�ʦ:" << endl;
    for (int i = 0; i < teachersSize; ++i) {
        cout << teachers[i]->name << " (number: " << teachers[i]->number << ", birthday: " << teachers[i]->birthday.year << "-" << teachers[i]->birthday.month << "-" << teachers[i]->birthday.day << ")" << endl;
    }

    // ��ʾ������ѧ��
    cout << "������ѧ��:" << endl;
    for (int i = 0; i < studentsSize; ++i) {
        cout << students[i]->name << " (number: " << students[i]->number << ", birthday: " << students[i]->birthday.year << "-" << students[i]->birthday.month << "-" << students[i]->birthday.day << ")" << endl;
    }

    // �� number ���ҽ�ʦ��ѧ��
    cout << "�� number ���ҽ�ʦ:";
    int searchNumber1;
    cin >> searchNumber1;
    people* foundTeacher = searchPeopleByNumber(reinterpret_cast<people**>(teachers), teachersSize, searchNumber1);

    if (foundTeacher) {
        cout << "�ҵ���ʦ: " << foundTeacher->name << " (number: " << foundTeacher->number << ")" << endl;
    }
    else {
        cout << "δ�ҵ����Ϊ " << searchNumber1 << " �Ľ�ʦ" << endl;
    }

    cout << "�� number ����ѧ��:";
    int searchNumber2;
    cin >> searchNumber2;
    people* foundStudent = searchPeopleByNumber(reinterpret_cast<people**>(students), studentsSize, searchNumber2);

    if (foundStudent) {
        cout << "�ҵ�ѧ��: " << foundStudent->name << " (number: " << foundStudent->number << ")" << endl;
    }
    else {
        cout << "δ�ҵ����Ϊ " << searchNumber2 << " ��ѧ��" << endl;
    }

    // ����̬������ڴ�
    for (int i = 0; i < teachersSize; ++i) {
        delete teachers[i];
    }
    for (int i = 0; i < studentsSize; ++i) {
        delete students[i];
    }

    return 0;
}