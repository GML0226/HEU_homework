#ifndef ARRAYCLASS_H
#define ARRAYCLASS_H

#include <iostream>
using namespace std;

template <typename T>
class ArrayClass {
private:
    T* arr;      // ����Ԫ��
    int size;    // ��ǰ����Ԫ������
    int capacity; // �����������

    // ˽�г�Ա������������������Ԫ��
    void swapElements(int index1, int index2) {
        T temp = arr[index1];
        arr[index1] = arr[index2];
        arr[index2] = temp;
    }

public:
    // ���캯��
    ArrayClass(int cap) : size(0), capacity(cap) {
        arr = new T[capacity];
    }

    // ��������
    ~ArrayClass() {
        delete[] arr;
    }

    // ��Ա������ֱ�Ӳ�������
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

    // ��Ա������ֱ��ѡ������
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

    // ��Ա������ð������
    void bubbleSort() {
        for (int i = 0; i < size - 1; i++) {
            for (int j = 0; j < size - i - 1; j++) {
                if (arr[j] > arr[j + 1]) {
                    swapElements(j, j + 1);
                }
            }
        }
    }

    // ��Ա������˳�����
    int linearSearch(const T& value) {
        for (int i = 0; i < size; i++) {
            if (arr[i] == value) {
                return i;
            }
        }
        return -1; // Ԫ��δ�ҵ�
    }

    // ��Ա���������Ԫ��
    bool addElement(const T& value) {
        if (size >= capacity) {
            return false; // ��������
        }
        arr[size++] = value;
        return true;
    }

    // ��Ա��������ӡ����
    void printArray() {
        for (int i = 0; i < size; i++) {
            cout << arr[i] << " ";
        }
        cout << endl;
    }

    // ��Ա��������ȡ�����С
    int getSize() const {
        return size;
    }
};

#endif // ARRAYCLASS_H