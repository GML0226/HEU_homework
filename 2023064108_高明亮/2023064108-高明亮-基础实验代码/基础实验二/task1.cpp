#include <iostream>
using namespace std;

class Point {
private:
    int _x;
    int _y;

public:
    Point(int x, int y) : _x(x), _y(y) {}

    void display() const {
        cout << "(" << _x << ", " << _y << ")" << endl;
    }

    Point& operator++() {         // ����ǰ����������� ++
        ++_x;
        ++_y;
        return *this;
    }
     
    Point& operator--() {         // ����ǰ���Լ������ --
        --_x;
        --_y;
        return *this;
    }

    Point operator++(int) {       // ���غ����Լ������ --
        Point old = *this;
        _x++;
        _y++;
        return old;
    }

    Point operator--(int) {        // ���غ����Լ������ --
        Point old = *this;
        _x--;
        _y--;
        return old;
    }
};

int main() {
    int a, b;
    cout << "�������ʼ����:";
    cin >> a >> b;

    Point p( a,b );
    cout << "��ʼ����:";
    p.display();

    cout << "ǰ������:";
    (++p).display();  // ǰ������
  
    cout << "ǰ���Լ�:";
    (--p).display();  // ǰ���Լ�

    cout << "��������:";
    (p++).display();  // ��������
 
    cout << "�����Լ�:";
    (p--).display();  // �����Լ�


    return 0;
}