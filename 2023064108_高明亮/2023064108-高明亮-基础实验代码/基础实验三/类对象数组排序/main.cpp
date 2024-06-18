#include <iostream>
#include <string>
#include <algorithm>

using namespace std;

// 定义日期类
class date {
public:
    int year, month, day;
    date(int y = 0, int m = 0, int d = 0) : year(y), month(m), day(d) {}
};

// 定义人员类，包含日期对象
class people {
public:
    string name;
    int number;
    string gender;
    int id;
    date birthday;  // 包含日期对象

    people(string name = "0", int number = 0, string gender = "0", int id = 0, date birthday = date())
        : name(name), number(number), gender(gender), id(id), birthday(birthday) {}
};

// 定义学生类，继承人员类
class student : public people {
public:
    int classnumber;
    student(string name = "1", int number = 1, string gender = "1", int id = 1, int classnumber = 1, date birthday = date())
        : people(name, number, gender, id, birthday), classnumber(classnumber) {}
};

// 定义教师类，继承人员类
class teacher : public people {
public:
    string department;
    teacher(string name = "2", int number = 2, string gender = "2", int id = 2, string department = "2", date birthday = date())
        : people(name, number, gender, id, birthday), department(department) {}
};

// 按 number 属性对人员对象数组进行排序的函数
void sortPeopleArray(people* peopleArray[], int size) {
    sort(peopleArray, peopleArray + size, [](people* p1, people* p2) {
        return p1->number < p2->number;
        });
}

// 按 number 属性查找人员对象的函数
people* searchPeopleByNumber(people* peopleArray[], int size, int number) {
    for (int i = 0; i < size; ++i) {
        if (peopleArray[i]->number == number) {
            return peopleArray[i];
        }
    }
    return nullptr;
}

int main() {
    // 创建教师数组
    teacher* teachers[] = {
        new teacher("A", 3, "Female", 1001, "Math", date(1980, 5, 1)),
        new teacher("B", 1, "Male", 1002, "Science", date(1975, 8, 23)),
        new teacher("C", 2, "Male", 1003, "History", date(1982, 11, 15))
    };
    const int teachersSize = sizeof(teachers) / sizeof(teachers[0]);

    // 创建学生数组
    student* students[] = {
        new student("D", 6, "Male", 2001, 101, date(2000, 2, 14)),
        new student("E", 5, "Female", 2002, 102, date(2001, 7, 30)),
        new student("F", 4, "Male", 2003, 103, date(1999, 12, 5))
    };
    const int studentsSize = sizeof(students) / sizeof(students[0]);

    // 按 number 对教师和学生进行排序
    sortPeopleArray(reinterpret_cast<people**>(teachers), teachersSize);
    sortPeopleArray(reinterpret_cast<people**>(students), studentsSize);

    // 显示排序后的教师
    cout << "排序后的教师:" << endl;
    for (int i = 0; i < teachersSize; ++i) {
        cout << teachers[i]->name << " (number: " << teachers[i]->number << ", birthday: " << teachers[i]->birthday.year << "-" << teachers[i]->birthday.month << "-" << teachers[i]->birthday.day << ")" << endl;
    }

    // 显示排序后的学生
    cout << "排序后的学生:" << endl;
    for (int i = 0; i < studentsSize; ++i) {
        cout << students[i]->name << " (number: " << students[i]->number << ", birthday: " << students[i]->birthday.year << "-" << students[i]->birthday.month << "-" << students[i]->birthday.day << ")" << endl;
    }

    // 按 number 查找教师和学生
    cout << "按 number 查找教师:";
    int searchNumber1;
    cin >> searchNumber1;
    people* foundTeacher = searchPeopleByNumber(reinterpret_cast<people**>(teachers), teachersSize, searchNumber1);

    if (foundTeacher) {
        cout << "找到教师: " << foundTeacher->name << " (number: " << foundTeacher->number << ")" << endl;
    }
    else {
        cout << "未找到编号为 " << searchNumber1 << " 的教师" << endl;
    }

    cout << "按 number 查找学生:";
    int searchNumber2;
    cin >> searchNumber2;
    people* foundStudent = searchPeopleByNumber(reinterpret_cast<people**>(students), studentsSize, searchNumber2);

    if (foundStudent) {
        cout << "找到学生: " << foundStudent->name << " (number: " << foundStudent->number << ")" << endl;
    }
    else {
        cout << "未找到编号为 " << searchNumber2 << " 的学生" << endl;
    }

    // 清理动态分配的内存
    for (int i = 0; i < teachersSize; ++i) {
        delete teachers[i];
    }
    for (int i = 0; i < studentsSize; ++i) {
        delete students[i];
    }

    return 0;
}