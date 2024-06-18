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

    Point& operator++() {         // 重载前置自增运算符 ++
        ++_x;
        ++_y;
        return *this;
    }
     
    Point& operator--() {         // 重载前置自减运算符 --
        --_x;
        --_y;
        return *this;
    }

    Point operator++(int) {       // 重载后置自减运算符 --
        Point old = *this;
        _x++;
        _y++;
        return old;
    }

    Point operator--(int) {        // 重载后置自减运算符 --
        Point old = *this;
        _x--;
        _y--;
        return old;
    }
};

int main() {
    int a, b;
    cout << "请输入初始坐标:";
    cin >> a >> b;

    Point p( a,b );
    cout << "初始坐标:";
    p.display();

    cout << "前置自增:";
    (++p).display();  // 前置自增
  
    cout << "前置自减:";
    (--p).display();  // 前置自减

    cout << "后置自增:";
    (p++).display();  // 后置自增
 
    cout << "后置自减:";
    (p--).display();  // 后置自减


    return 0;
}