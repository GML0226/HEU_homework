#include <iostream>
using namespace std;

class vehicle {
public:
    int MaxSpeed;
    float Weight;

    vehicle(int maxSpeed, float weight) : MaxSpeed(maxSpeed), Weight(weight) {}

    void Run() { cout << "Vehicle is running." << endl; }
    void Stop() { cout << "Vehicle has stopped." << endl; }
};

class bicycle : virtual public vehicle {
public:
    float Height;

    bicycle(int maxSpeed, float weight, float height) : vehicle(maxSpeed, weight), Height(height) {}
};

class motorcar : virtual public vehicle {
public:
    int SeatNum;

    motorcar(int maxSpeed, float weight, int seatNum) : vehicle(maxSpeed, weight), SeatNum(seatNum) {}
};

class motorcycle : public bicycle, public motorcar {
public:
    motorcycle(int maxSpeed, float weight, float height, int seatNum) : vehicle(maxSpeed, weight), bicycle(maxSpeed, weight, height), motorcar(maxSpeed, weight, seatNum) {}
};

int main() {
    motorcycle moto(120, 200.5, 1.2, 2);

    // ·ÃÎÊ³µÁ¾ÊôÐÔ
    cout << "Max Speed: " << moto.MaxSpeed << endl;
    moto.Run();
    return 0;
}
