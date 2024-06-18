#ifndef ARRAYCLASS_H
#define ARRAYCLASS_H

#include <iostream>
using namespace std;

template <typename T>
class ArrayClass {
private:
    T* arr;      // 数组元素
    int size;    // 当前数组元素数量
    int capacity; // 数组最大容量

    // 私有成员函数：交换两个数组元素
    void swapElements(int index1, int index2) {
        T temp = arr[index1];
        arr[index1] = arr[index2];
        arr[index2] = temp;
    }

public:
    // 构造函数
    ArrayClass(int cap) : size(0), capacity(cap) {
        arr = new T[capacity];
    }

    // 析构函数
    ~ArrayClass() {
        delete[] arr;
    }

    // 成员函数：直接插入排序
    void insertionSort() {
        for (int i = 1; i < size; i++) {
            T key = arr[i];
            int j = i - 1;
            while (j >= 0 && arr[j] > key) {
                arr[j + 1] = arr[j];
                j--;
            }
            arr[j + 1] = key;
        }
    }

    // 成员函数：直接选择排序
    void selectionSort() {
        for (int i = 0; i < size - 1; i++) {
            int minIndex = i;
            for (int j = i + 1; j < size; j++) {
                if (arr[j] < arr[minIndex]) {
                    minIndex = j;
                }
            }
            if (minIndex != i) {
                swapElements(i, minIndex);
            }
        }
    }

    // 成员函数：冒泡排序
    void bubbleSort() {
        for (int i = 0; i < size - 1; i++) {
            for (int j = 0; j < size - i - 1; j++) {
                if (arr[j] > arr[j + 1]) {
                    swapElements(j, j + 1);
                }
            }
        }
    }

    // 成员函数：顺序查找
    int linearSearch(const T& value) {
        for (int i = 0; i < size; i++) {
            if (arr[i] == value) {
                return i;
            }
        }
        return -1; // 元素未找到
    }

    // 成员函数：添加元素
    bool addElement(const T& value) {
        if (size >= capacity) {
            return false; // 数组已满
        }
        arr[size++] = value;
        return true;
    }

    // 成员函数：打印数组
    void printArray() {
        for (int i = 0; i < size; i++) {
            cout << arr[i] << " ";
        }
        cout << endl;
    }

    // 成员函数：获取数组大小
    int getSize() const {
        return size;
    }
};

#endif // ARRAYCLASS_H