#include "ArrayClass.h"

int main() {
    using namespace std;

    // 创建一个整数类型的数组类实例，容量为10
    ArrayClass<int> myArray(10);

    // 向数组中添加一些元素
    cout << "请输入五个原始数据：" << endl;
    for (int i = 0; i < 5; i++) {
        int num;
        cin >> num;
        myArray.addElement(num);
    }


    // 打印原始数组
    cout << "原始数组: ";
    myArray.printArray();

    // 执行直接插入排序
    myArray.insertionSort();
    cout << "插入排序后: ";
    myArray.printArray();

    // 执行直接选择排序
    myArray.selectionSort();
    cout << "选择排序后: ";
    myArray.printArray();

    // 执行冒泡排序
    myArray.bubbleSort();
    cout << "冒泡排序后: ";
    myArray.printArray();

    // 测试顺序查找
    cout << "输入要查找的数据：" << endl;
    int valueToFind;
    cin >> valueToFind;
    int foundIndex = myArray.linearSearch(valueToFind);
    if (foundIndex != -1) {
        cout << "元素 " << valueToFind << " 找到，位于数组下标 " << foundIndex << endl;
    }
    else {
        cout << "元素 " << valueToFind << " 未找到。" << endl;
    }

    return 0;
}