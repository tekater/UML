#include <iostream>
#include <thread>

using std::cin;
using std::cout;
using std::endl;

using namespace std::chrono_literals;

bool finish = false;


void Plus() {
    while (!finish) {
        cout << "+ ";
        std::this_thread::sleep_for(1s);
    }
}
void Minus() {
    while (!finish) {
        cout << "- ";
        std::this_thread::sleep_for(1s);
    }
}

int main()
{
    setlocale(0, "");

    //Plus();
    //Minus();

    std::thread plus_thread = std::thread(Plus);
    std::thread minus_thread = std::thread(Minus);

    cin.get(); // Функция get() ожидает нажатие 'Enter'
    finish = true;

    if (minus_thread.joinable()) {
        minus_thread.join();
    }
    if (plus_thread.joinable()) {
        plus_thread.join();
    }
}
