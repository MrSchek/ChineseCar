#include <iostream>
#include <list>
#include "ChineseCars.h"

using namespace std;

// Мощность автомобиля
enum class Car_power : int
{
    Low,
    Average,
    High,
    Unknown
};

class Car {
private:
    bool electric;
    Car_power Power;

protected:
    string brand;
    string fuel_type;
    string OS;

    bool subscription;

public:
    Car(Car_power power) : Power(power), subscription(false) {
        subscription = static_cast<bool>(rand() % 2);
    };
    virtual ~Car();

    string get_brand() const { return brand; }
    bool isElectric() const { return electric; }
    string getOS() const { return OS; }

    bool IsSubscription() const { return subscription; }

    Car_power GetPower() const { return Power; }

    virtual void drive() {
        if (IsSubscription()) {
            cout << "Запускаю автомобиль " << brand << endl;
        }
        else {
            cout << "Нет оплаты! Невозможно запустить " << brand << endl;
        }
    }
};

Car::~Car() {}

class BYD : public Car {
public:
    BYD();
    ~BYD();

    string get_brand() const;

    void drive();
};

BYD::BYD() : Car(Car_power::High) {
    brand = "BYD";
    fuel_type = "Электрический";
}

BYD::~BYD() {}

string BYD::get_brand() const { return Car::get_brand(); }

void BYD::drive() {
    Car::drive();
    cout << "Автомобиль " << brand << " на ходу!" << endl;
}

class Geely : public Car {
public:
    Geely();
    ~Geely();

    void drive();
};

Geely::Geely() : Car(Car_power::Low) {
    brand = "Geely";
    fuel_type = "Бензин";
}

Geely::~Geely() {}

void Geely::drive() {
    Car::drive();
    cout << "Автомобиль " << brand << " на ходу!" << endl;
}

class NIO : public Car {
public:
    NIO();
    ~NIO();

    void drive();
};

NIO::NIO() : Car(Car_power::Average) {
    brand = "NIO";
    fuel_type = "Электрический";
}

NIO::~NIO() {}

void NIO::drive() {
    Car::drive();
    cout << "Автомобиль " << brand << " на ходу!" << endl;
}

// Фабричный метод

enum class Car_type : int
{
    BYD = 1,
    Geely = 2,
    NIO = 3,

    Undefined = 0
};

Car* Create_car(Car_type type) {
    Car* new_car = nullptr;

    if (type == Car_type::BYD) {
        new_car = new BYD;
    }
    else if (type == Car_type::Geely) {
        new_car = new Geely;
    }
    else if (type == Car_type::NIO) {
        new_car = new NIO;
    }

    return new_car;
}

// Декоратор итератора для выделения автомобилей по мощности

class PowerDecorator : public IteratorDecorator<class Car*> {
private:
    Car_power TargetPower;

public:
    PowerDecorator(Iterator<Car*>* it, Car_power power)
        : IteratorDecorator<Car*>(it), TargetPower(power) {}

    void First() {
        It->First();
        while (!It->IsDone() && It->GetCurrent()->GetPower() != TargetPower) {
            It->Next();
        }
    }

    void Next() {
        do {
            It->Next();
        } while (!It->IsDone() && It->GetCurrent()->GetPower() != TargetPower);
    }
};

// Декоратор итератора для выделения автомобилей по наличию оплаты

class SubscriptionDecorator : public IteratorDecorator<class Car*> {
private:
    bool TargetSubscription;

public:
    SubscriptionDecorator(Iterator<Car*>* it, bool isSubscription)
        : IteratorDecorator<Car*>(it), TargetSubscription(isSubscription) {}

    void First() {
        It->First();
        while (!It->IsDone() && It->GetCurrent()->IsSubscription() != TargetSubscription) {
            It->Next();
        }
    }

    void Next() {
        do {
            It->Next();
        } while (!It->IsDone() && It->GetCurrent()->IsSubscription() != TargetSubscription);
    }
};

void Drive_all(Iterator<Car*>* it) {
    for (it->First(); !it->IsDone(); it->Next()) {
        Car* currentCar = it->GetCurrent();
        currentCar->drive();
    }
}

int main() {
    setlocale(LC_ALL, "Russian");

    size_t N = 30;


    ArrayClass<Car*> carArray;
    for (size_t i = 0; i < N; i++) {
        int car_num = rand() % 3 + 1;
        Car_type car_type = static_cast<Car_type>(car_num);
        Car* newCar = Create_car(car_type);
        carArray.Add(newCar);
    }

    wcout << L"Размер массива автомобилей: " << carArray.Size() << endl;

    list<Car*> carList;
    for (size_t i = 0; i < N; i++) {
        int car_num = rand() % 3 + 1;
        Car_type car_type = static_cast<Car_type>(car_num);
        Car* newCar = Create_car(car_type);
        carList.push_back(newCar);
    }

    wcout << L"Размер списка автомобилей: " << carList.size() << endl;

    // Обход всех автомобилей
    cout << endl << "Попробовать запустить все автомобили: " << endl;
    Iterator<Car*>* allIt = carArray.GetIterator();
    Drive_all(allIt);
    delete allIt;

    // Обход всех автомобилей с оплатой каршеринга
    cout << endl << "Запустить только если есть оплата каршеринга:" << endl;
    Iterator<Car*>* goodIt = new SubscriptionDecorator(carArray.GetIterator(), true);
    Drive_all(goodIt);
    delete goodIt;

    // Обход всех мощных автомобилей
    cout << endl << "Все мощные автомобили:" << endl;
    Iterator<Car*>* powerIt = new PowerDecorator(carArray.GetIterator(), Car_power::High);
    Drive_all(powerIt);
    delete powerIt;

    // Обход всех мощных автомобилей с оплатой каршеринга
    cout << endl << "Все мощные автомобили с оплатой каршеринга:" << endl;
    Iterator<Car*>* goodPowerIt =
        new SubscriptionDecorator(new PowerDecorator(carArray.GetIterator(), Car_power::High), true);
    Drive_all(goodPowerIt);
    delete goodPowerIt;

    // Демонстрация работы адаптера
    cout << endl << "Все мощные автомобили с оплатой (другой контейнер):" << endl;
    Iterator<Car*>* adaptedIt = new ConstIteratorAdapter<std::list<Car*>, Car*>(&carList);
    Iterator<Car*>* adaptedPowerIt = new SubscriptionDecorator(new PowerDecorator(adaptedIt, Car_power::High), true);
    Drive_all(adaptedPowerIt);
    delete adaptedPowerIt;

    return 0;
}

