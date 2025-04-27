#include <iostream>
#include <list>
#include "ChineseCars.h"

using namespace std;

enum class Source_power : int
{
    Low,
    Average,
    High,
    Unknown
};

enum class car_launch_method : int
{
    remote_app,
    kay_command,
    website,

    None
};

class CarStartStrategy {
public:
    virtual ~CarStartStrategy() {}
    virtual void start() = 0;
};

class RemoteAppCarStartStrategy : public CarStartStrategy
{
    void start() { cout << "Запускаю машину с мобильного приложения"; }
};

class KayCommandCarStartStrategy : public CarStartStrategy
{
    void start() { cout << "Запускаю машину ключом зажигания"; }
};

class WebsiteCarStartStrategy : public CarStartStrategy
{
    void start() { cout << "Запускаю машину с сайта"; }
};

CarStartStrategy* CreateCarStartStrategy(car_launch_method manner) {
    switch (manner) {
    case car_launch_method::remote_app: return new RemoteAppCarStartStrategy;
    case car_launch_method::kay_command: return new KayCommandCarStartStrategy;
    case car_launch_method::website: return new WebsiteCarStartStrategy;
    default:return nullptr;
    }
}

class SmartCar {
private:
    bool has_subscription;
    Source_power power;

    CarStartStrategy* __CarStartStrategy;

    void DoCarStartStrategy() {
        if (__CarStartStrategy != nullptr) {
            __CarStartStrategy->start();
        }
        else
        {
            cout << "Ничего не делать! ";
        }
    }

    void DetectSubscription() {
        if (IsSubscription())
        {
            cout << "Есть оплата каршеринга ";
        }
        else
        {
            cout << "Нет оплаты каршеринга! ";
        }
    }

protected:
    string car_brand;
    string protocols;
    string OS;

public:
    SmartCar(Source_power power) : power(power), has_subscription(false), __CarStartStrategy(nullptr) {
        has_subscription = static_cast<bool>(rand() % 2);
    };

    virtual ~SmartCar() {
        if (__CarStartStrategy != nullptr) delete __CarStartStrategy;
    }

    string get_car_brand() const { return car_brand; }
    bool get_has_subscription() const { return has_subscription; }
    string getOS() const { return OS; }

    bool IsSubscription() const { return has_subscription; }
    Source_power GetPower() const { return power; }

    void SetCarStartStrategy(CarStartStrategy* carStartStrategy)
    {
        __CarStartStrategy = carStartStrategy;
    }

    virtual void PrintType() = 0;
    virtual void ChooseCar() = 0;

    void start()
    {
        PrintType();
        cout << " : ";

        DetectSubscription();
        cout << " : ";

        ChooseCar();
        cout << " : ";

        DoCarStartStrategy();

        cout << endl;
    }
};

class Geely : public SmartCar {
public:
    Geely();
    ~Geely();

    void PrintType() { cout << "Geely"; };
    void ChooseCar() { cout << "Выбираю модель Geely"; };
};

Geely::Geely() : SmartCar(Source_power::High)
{
    SetCarStartStrategy(CreateCarStartStrategy(car_launch_method::kay_command));
    car_brand = "Geely";
    protocols = "Wi-Fi, Bluetooth, 4G";
}

Geely::~Geely() {}

class BYD : public SmartCar {
public:
    BYD();
    ~BYD();

    void PrintType() { cout << "BYD"; };
    void ChooseCar() { cout << "Выбираю модель BYD"; };
};

BYD::BYD() : SmartCar(Source_power::Low) {
    SetCarStartStrategy(CreateCarStartStrategy(car_launch_method::website));
    car_brand = "BYD";
    protocols = "Wi-Fi, Bluetooth";
}

BYD::~BYD() {}

class NIO : public SmartCar {
public:
    NIO();
    ~NIO();

    void PrintType() { cout << "NIO"; };
    void ChooseCar() { cout << "Выбираю модель NIO"; };
};

NIO::NIO() : SmartCar(Source_power::Average) {
    SetCarStartStrategy(CreateCarStartStrategy(car_launch_method::remote_app));
    car_brand = "NIO";
    protocols = "Wi-Fi, Bluetooth, 5G";
}

NIO::~NIO() {}

// Фабричный метод
enum class SmartCar_type : int
{
    Geely = 1,
    BYD = 2,
    NIO = 3,

    Undefined = 0
};

SmartCar* CreateSmartCar(SmartCar_type type)
{
    SmartCar* new_smart_car = nullptr;

    if (type == SmartCar_type::Geely)
    {
        new_smart_car = new Geely;
    }
    else if (type == SmartCar_type::BYD)
    {
        new_smart_car = new BYD;
    }
    else if (type == SmartCar_type::NIO)
    {
        new_smart_car = new NIO;
    }

    return new_smart_car;
}

// Декоратор итератора для выделения машин по мощности
class PowerDecorator : public IteratorDecorator<class SmartCar*>
{
private:
    Source_power TargetPower;

public:
    PowerDecorator(Iterator<SmartCar*>* it, Source_power power)
        : IteratorDecorator<SmartCar*>(it), TargetPower(power) {}

    void First()
    {
        It->First();
        while (!It->IsDone() && It->GetCurrent()->GetPower() != TargetPower)
        {
            It->Next();
        }
    }

    void Next()
    {
        do
        {
            It->Next();

        } while (!It->IsDone() && It->GetCurrent()->GetPower() != TargetPower);
    }
};

// Декоратор итератора для выделения машин по наличию оплаты каршеринга
class SubscriptionDecorator : public IteratorDecorator<class SmartCar*>
{
private:
    bool TargetSubscription;

public:
    SubscriptionDecorator(Iterator<SmartCar*>* it, bool isSubscription)
        : IteratorDecorator<SmartCar*>(it), TargetSubscription(isSubscription) {}

    void First()
    {
        It->First();
        while (!It->IsDone() && It->GetCurrent()->IsSubscription() != TargetSubscription)
        {
            It->Next();
        }
    }

    void Next()
    {
        do
        {
            It->Next();

        } while (!It->IsDone() && It->GetCurrent()->IsSubscription() != TargetSubscription);
    }
};

void StartAll(Iterator<SmartCar*>* it)
{
    for (it->First(); !it->IsDone(); it->Next())
    {
        SmartCar* currentSmartCar = it->GetCurrent();
        currentSmartCar->start();
    }
}

int main()
{
    setlocale(LC_ALL, "Russian");

    size_t N = 30;

    ArrayClass<SmartCar*> smartcarArray;
    for (size_t i = 0; i < N; i++)
    {
        int smartcar_num = rand() % 3 + 1;
        SmartCar_type smart_car_type = static_cast<SmartCar_type>(smartcar_num);
        SmartCar* newSmartCar = CreateSmartCar(smart_car_type);
        smartcarArray.Add(newSmartCar);
    }

    wcout << L"Размер массива машин: " << smartcarArray.Size() << endl;

    list<SmartCar*> smartcarList;
    for (size_t i = 0; i < N; i++)
    {
        int smartcar_num = rand() % 3 + 1;
        SmartCar_type smart_car_type = static_cast<SmartCar_type>(smartcar_num);
        SmartCar* newSmartCar = CreateSmartCar(smart_car_type);
        smartcarList.push_back(newSmartCar);
    }

    wcout << L"Размер списка машин: " << smartcarList.size() << endl;

    // Обход всех элементов при помощи итератора
    cout << endl << "Попробовать запустить всё: " << endl;
    Iterator<SmartCar*>* allIt = smartcarArray.GetIterator();
    StartAll(allIt);
    delete allIt;

    // Обход всех машин с оплатой каршеринга
    cout << endl << "Запустить только если есть оплата каршеринга:" << endl;
    Iterator<SmartCar*>* goodIt = new SubscriptionDecorator(smartcarArray.GetIterator(), true);
    StartAll(goodIt);
    delete goodIt;

    // Обход всех мощных машин
    cout << endl << "Все мощные машины:" << endl;
    Iterator<SmartCar*>* orangeIt = new PowerDecorator(smartcarArray.GetIterator(), Source_power::High);
    StartAll(orangeIt);
    delete orangeIt;

    // Обход всех мощных машин с оплатой каршеринга
    cout << endl << "Все мощные машины с оплатой каршеринга:" << endl;
    Iterator<SmartCar*>* goodOrangeIt = new SubscriptionDecorator(new PowerDecorator(smartcarArray.GetIterator(), Source_power::High), true);
    StartAll(goodOrangeIt);
    delete goodOrangeIt;

    // Демонстрация работы адаптера
    cout << endl << "Все мощные машины с оплатой каршеринга (другой контейнер):" << endl;
    Iterator<SmartCar*>* adaptedIt = new ConstIteratorAdapter<std::list<SmartCar*>, SmartCar*>(&smartcarList);
    Iterator<SmartCar*>* adaptedOrangeIt = new SubscriptionDecorator(new PowerDecorator(adaptedIt, Source_power::High), true);
    StartAll(adaptedOrangeIt);
    delete adaptedOrangeIt;

    for (size_t i = 0; i < smartcarArray.Size(); i++) {
        delete smartcarArray.GetElement(i);
    }

    for (auto car : smartcarList) {
        delete car;
    }

    return 0;
}