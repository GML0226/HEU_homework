#include <iostream>
#include "Stack.h" // 包含头文件

using namespace std;

int main() {
    Stack<int, 5> intStack; // 声明一个大小为5的整型栈对象

    // 将五个整数压入栈
    cout << "请输入五个数压入栈:";
    for (int i = 0; i < 5; i++) {
        int num;
        cin >> num;
        intStack.push(num);
    }
    cout << endl;


    // 依次弹出并显示栈中的整数
    cout << "弹出栈的数:";
    while (!intStack.isEmpty()) {
        cout << intStack.pop() << "  ";
    }
    cout << endl;

    return 0;
}