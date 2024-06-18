#include "ArrayClass.h"

int main() {
    using namespace std;

    // ����һ���������͵�������ʵ��������Ϊ10
    ArrayClass<int> myArray(10);

    // �����������һЩԪ��
    cout << "���������ԭʼ���ݣ�" << endl;
    for (int i = 0; i < 5; i++) {
        int num;
        cin >> num;
        myArray.addElement(num);
    }


    // ��ӡԭʼ����
    cout << "ԭʼ����: ";
    myArray.printArray();

    // ִ��ֱ�Ӳ�������
    myArray.insertionSort();
    cout << "���������: ";
    myArray.printArray();

    // ִ��ֱ��ѡ������
    myArray.selectionSort();
    cout << "ѡ�������: ";
    myArray.printArray();

    // ִ��ð������
    myArray.bubbleSort();
    cout << "ð�������: ";
    myArray.printArray();

    // ����˳�����
    cout << "����Ҫ���ҵ����ݣ�" << endl;
    int valueToFind;
    cin >> valueToFind;
    int foundIndex = myArray.linearSearch(valueToFind);
    if (foundIndex != -1) {
        cout << "Ԫ�� " << valueToFind << " �ҵ���λ�������±� " << foundIndex << endl;
    }
    else {
        cout << "Ԫ�� " << valueToFind << " δ�ҵ���" << endl;
    }

    return 0;
}