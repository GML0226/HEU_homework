#include <iostream>
#include "Stack.h" // ����ͷ�ļ�

using namespace std;

int main() {
    Stack<int, 5> intStack; // ����һ����СΪ5������ջ����

    // ���������ѹ��ջ
    cout << "�����������ѹ��ջ:";
    for (int i = 0; i < 5; i++) {
        int num;
        cin >> num;
        intStack.push(num);
    }
    cout << endl;


    // ���ε�������ʾջ�е�����
    cout << "����ջ����:";
    while (!intStack.isEmpty()) {
        cout << intStack.pop() << "  ";
    }
    cout << endl;

    return 0;
}