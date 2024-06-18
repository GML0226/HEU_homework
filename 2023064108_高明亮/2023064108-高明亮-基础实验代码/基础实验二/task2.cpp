//#include <iostream>
//using namespace std;
//
//class vehicle {
//public:
//    int MaxSpeed;
//    float Weight;
//
//    vehicle(int maxSpeed, float weight) : MaxSpeed(maxSpeed), Weight(weight) {}
//
//    void Run() { cout << "Vehicle is running." << endl; }
//    void Stop() { cout << "Vehicle has stopped." << endl; }
//    void show(){ cout << MaxSpeed << "," << Weight << endl; }
//};
//
//class bicycle : virtual public vehicle {
//public:
//    float Height;
//
//    bicycle(int maxSpeed, float weight, float height) : vehicle(maxSpeed, weight), Height(height) {}
//
//    void Run() { cout << "bicycle is running." << endl; }
//    void Stop() { cout << "bicycle has stopped." << endl; }
//    void show() { cout << MaxSpeed << "," << Weight << ","<<Height << endl; }
//};
//
//class motorcar : virtual public vehicle {
//public:
//    int SeatNum;
//
//    motorcar(int maxSpeed, float weight, int seatNum) : vehicle(maxSpeed, weight), SeatNum(seatNum) {}
//
//    void Run() { cout << "motorcar is running." << endl; }
//    void Stop() { cout << "motorcar has stopped." << endl; }
//    void show() { cout << MaxSpeed << "," << Weight << ","<<SeatNum << endl; }
//};
//
//class motorcycle : public bicycle, public motorcar {
//public:
//    motorcycle(int maxSpeed, float weight, float height, int seatNum) : vehicle(maxSpeed, weight), bicycle(maxSpeed, weight, height), motorcar(maxSpeed, weight, seatNum) {}
//
//    void Run() { cout << "motorcycle is running." << endl; }
//    void Stop() { cout << "motorcycle has stopped." << endl; }
//    void show() { cout << MaxSpeed << "," << Weight << "," << Height<<","<<SeatNum<< endl; }
//};
//
//int main() {
//    vehicle vehicle(100, 100);
//    vehicle.Run();
//    vehicle.Stop();
//    vehicle.show();
//    cout << endl;
//
//    bicycle bicycle(40, 10, 10);
//    bicycle.Run();
//    bicycle.Stop();
//    bicycle.show();
//    cout << endl;
//
//
//    motorcar motorcar(150, 200, 5);
//    motorcar.Run();
//    motorcar.Stop();
//    motorcar.show();
//    cout << endl;
//
//
//    motorcycle motorcycle(120, 150, 20, 3);
//    motorcycle.Run();
//    motorcycle.Stop();
//    motorcycle.show();
//
//    return 0;
//}


//使用vehicle类型指针调用

//#include <iostream>
//using namespace std;
//
//class vehicle {
//public:
//    int MaxSpeed;
//    float Weight;
//
//    vehicle(int maxSpeed, float weight) : MaxSpeed(maxSpeed), Weight(weight) {}
//
//    virtual void Run() { cout << "Vehicle is running." << endl; }        //没有声明虚函数，无法运行
//    virtual void Stop() { cout << "Vehicle has stopped." << endl; }      //没有声明虚函数，无法运行
//};
//
//class bicycle : virtual public vehicle {
//public:
//    float Height;
//
//    bicycle(int maxSpeed, float weight, float height) : vehicle(maxSpeed, weight), Height(height) {}
//
//    void Run() override { cout << "Bicycle is running." << endl; }
//    void Stop() override { cout << "Bicycle has stopped." << endl; }
//};
//
//class motorcar : virtual public vehicle {
//public:
//    int SeatNum;
//
//    motorcar(int maxSpeed, float weight, int seatNum) : vehicle(maxSpeed, weight), SeatNum(seatNum) {}
//
//    void Run() override { cout << "Motorcar is running." << endl; }
//    void Stop() override { cout << "Motorcar has stopped." << endl; }
//};
//
//class motorcycle : public bicycle, public motorcar {
//public:
//    motorcycle(int maxSpeed, float weight, float height, int seatNum) : vehicle(maxSpeed, weight), bicycle(maxSpeed, weight, height), motorcar(maxSpeed, weight, seatNum) {}
//
//    void Run() override { cout << "Motorcycle is running." << endl; }
//    void Stop() override { cout << "Motorcycle has stopped." << endl; }
//};
//
//int main() {
//    vehicle* v;
//
//    vehicle vehicle(100, 100);
//    v = &vehicle;
//    v->Run();
//    v->Stop();
//    cout << endl;
//
//    bicycle bicycle(40, 10, 10);
//    v = &bicycle;
//    v->Run();
//    v->Stop();
//    cout << endl;
//
//    motorcar motorcar(150, 200, 5);
//    v = &motorcar;
//    v->Run();
//    v->Stop();
//    cout << endl;
//
//    motorcycle motorcycle(120, 150, 20, 3);
//    v = &motorcycle;
//    v->Run();
//    v->Stop();
//
//    return 0;
//}



#include <iostream>
using namespace std;

class vehicle {
public:
    int MaxSpeed;
    float Weight;

    vehicle(int maxSpeed, float weight) : MaxSpeed(maxSpeed), Weight(weight) {}

    virtual void Run() { cout << "Vehicle is running." << endl; }                  //若没有声明虚函数，则通过vehicle指针无法运行
    virtual void Stop() { cout << "Vehicle has stopped." << endl; }                //若没有声明虚函数，则通过vehicle指针无法运行
    virtual void show() { cout << MaxSpeed << "," << Weight << endl; }             //若没有声明虚函数，则通过vehicle指针无法运行
};

class bicycle : virtual public vehicle {
public:
    float Height;

    bicycle(int maxSpeed, float weight, float height) : vehicle(maxSpeed, weight), Height(height) {}

    void Run() override { cout << "bicycle is running." << endl; }
    void Stop() override { cout << "bicycle has stopped." << endl; }
    void show() override { cout << MaxSpeed << "," << Weight << "," << Height << endl; }
};

class motorcar : virtual public vehicle {
public:
    int SeatNum;

    motorcar(int maxSpeed, float weight, int seatNum) : vehicle(maxSpeed, weight), SeatNum(seatNum) {}

    void Run() override { cout << "motorcar is running." << endl; }
    void Stop() override { cout << "motorcar has stopped." << endl; }
    void show() override { cout << MaxSpeed << "," << Weight << "," << SeatNum << endl; }
};

class motorcycle : public bicycle, public motorcar {
public:
    motorcycle(int maxSpeed, float weight, float height, int seatNum) : vehicle(maxSpeed, weight), bicycle(maxSpeed, weight, height), motorcar(maxSpeed, weight, seatNum) {}

    void Run() override { cout << "motorcycle is running." << endl; }
    void Stop() override { cout << "motorcycle has stopped." << endl; }
    void show() override { cout << MaxSpeed << "," << Weight << "," << Height << "," << SeatNum << endl; }
};

int main() {
    vehicle v(100, 100);
    v.Run();
    v.Stop();
    v.show();
    cout << endl;

    //正常直接调用
    bicycle b(40, 10, 10);
    b.Run();
    b.Stop();
    b.show();
    cout << endl;

    motorcar m(150, 200, 5);
    m.Run();
    m.Stop();
    m.show();
    cout << endl;

    motorcycle mc(120, 150, 20, 3);
    mc.Run();
    mc.Stop();
    mc.show();
    cout << endl;


    //使用vehicle类型指针调用
    vehicle* vPtr;

    vPtr = &b;
    vPtr->Run();
    vPtr->Stop();
    vPtr->show();
    cout << endl;

    vPtr = &m;
    vPtr->Run();
    vPtr->Stop();
    vPtr->show();
    cout << endl;

    vPtr = &mc;
    vPtr->Run();
    vPtr->Stop();
    vPtr->show();
    cout << endl;

    return 0;
}
