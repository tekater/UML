#include <iostream>
#include <conio.h>
#include <thread>
#include <windows.h>

using namespace std;
using namespace std::chrono_literals;

#define Enter 13
#define Escape 27

#define MIN_TANK_VOLUME 20
#define MAX_TANK_VOLUME 120

class Tank {
	const int VOLUME;
	double fuel_level;

public:

	int get_VOLUME()const {
		return VOLUME;
	}
	double get_fuel_level()const {
		return fuel_level;
	}


	void fill(double fuel) {
		if (fuel < 0) {
			return;
		}
		if (fuel_level + fuel < VOLUME) {
			fuel_level += fuel;
		}
		else {
			fuel_level = VOLUME;
		}
	}


	double give_fuel(double amount) {
		fuel_level -= amount;
		if (fuel_level < 0) {
			fuel_level = 0;
		}
		return fuel_level;
	}


	Tank(int volume):VOLUME(
		volume < MIN_TANK_VOLUME ? MIN_TANK_VOLUME :
		volume > MAX_TANK_VOLUME ? MAX_TANK_VOLUME :
		volume             ) {
		this->fuel_level = 0;
		cout << "Tank is ready:\t" << this << endl;
	}
	~Tank() {
		cout << "Tank is over:\t" << this << endl;
	}


	void info()const {
		cout << "Volume:\t" << VOLUME << " liters\n";
		cout << "Fuel level:\t" << get_fuel_level() << " liters\n\n";
	}
};



#define MIN_ENGINE_CONSUMPTION 3
#define MAX_ENGINE_CONSUMPTION 30

class Engine {
	const double DEFAULT_CONSUMPTION;
	const double DEFAULT_CONSUMPTION_PER_SECOND;

	double consumption_per_second;

	bool is_started;

public:

	double get_DEFAULT_CONSUMPTION_PER_SECOND()const {
		return DEFAULT_CONSUMPTION_PER_SECOND;
	}
	double get_consumption_per_second()const {
		return consumption_per_second;
	}


	void set_consumption_per_second(int speed) {
		if (speed == 0) {
			consumption_per_second = DEFAULT_CONSUMPTION_PER_SECOND * 3e-5;
		}
		else if (speed < 60) {
			consumption_per_second = DEFAULT_CONSUMPTION_PER_SECOND * 20 / 3;
		}
		else if (speed < 100) {
			consumption_per_second = DEFAULT_CONSUMPTION_PER_SECOND * 14 / 3;
		}
		else if (speed < 140) {
			consumption_per_second = DEFAULT_CONSUMPTION_PER_SECOND * 20 / 3;
		}
		else if (speed < 200) {
			consumption_per_second = DEFAULT_CONSUMPTION_PER_SECOND * 25 / 3;
		}
		else if (speed < 250) {
			consumption_per_second = DEFAULT_CONSUMPTION_PER_SECOND * 10;
		}
	}


	Engine(double consumption)
		:DEFAULT_CONSUMPTION
		(
			consumption < MIN_ENGINE_CONSUMPTION ? MIN_ENGINE_CONSUMPTION :
			consumption > MAX_ENGINE_CONSUMPTION ? MAX_ENGINE_CONSUMPTION :
			consumption
		),
		DEFAULT_CONSUMPTION_PER_SECOND(DEFAULT_CONSUMPTION * 3e-5)
	{
		//this->DEFAULT_CONSUMPTION_PER_SECOND = DEFAULT_CONSUMPTION*3e-5;
		set_consumption_per_second(consumption);

		is_started = false;

		cout << "Engine is ready:\t" << this << endl;
	}
	~Engine() {
		cout << "Engine is over:\t" << this << endl;
	}


	void start() {
		is_started = true;
	}
	void stop() {
		is_started = false;
	}

	bool started()const {
		return is_started;
	}
	

	void info() {
		cout << "Consumption:\t" << DEFAULT_CONSUMPTION << " liters per 100km/h.\n";
		cout << "Consumption:\t" << consumption_per_second << " liters per 1 second.\t";
	}
};



#define MAX_SPEED_LOWER_LEVEL 30
#define MAX_SPEED_UPPER_LEVEL 400

class Car {
	Engine engine;
	Tank tank;

	const int MAX_SPEED;
	int speed;
	int accelleration;

	double time;
	double distance;

	bool driver_inside;

	struct Threads {
		std::thread panel_thread;
		std::thread engine_idle_thread;
		std::thread free_wheeling_thread;
		std::thread total_distance;
		std::thread timer;
	}threads;

	//Threads threads;

public:

	int get_speed()const {
		return speed;
	}
	int get_MAX_SPEED()const {
		return MAX_SPEED;
	}


	Car(double consumption, int volume, int max_speed, int acceleration = 10)
		:engine(consumption), tank(volume),
		MAX_SPEED
		(
			max_speed < MAX_SPEED_LOWER_LEVEL ? MAX_SPEED_LOWER_LEVEL :
			max_speed > MAX_SPEED_UPPER_LEVEL ? MAX_SPEED_UPPER_LEVEL :
			max_speed
		)
	{
		driver_inside = false;
		this->accelleration = acceleration;
		this->speed = 0;
		this->distance = 0;
		this->time = 0;
		cout << "\nYour car is ready to go " << this << endl << endl;
	}
	~Car() {
		cout << "\nCar is over\t" << this << endl << endl;
	}


	void get_in() {
		driver_inside = true;
		threads.panel_thread = std::thread(&Car::panel, this);
	}
	void get_out() {
		driver_inside = false;
		if (threads.panel_thread.joinable()) {
			threads.panel_thread.join();
		}
		system("cls");
		cout << "Outside" << endl;
	}


	void start() {
		if (driver_inside && tank.get_fuel_level()) {
			engine.start();
			threads.engine_idle_thread = std::thread(&Car::engine_idle, this);
			threads.timer = std::thread(&Car::driving_time, this);
			cout << "Двигатель запущен.\n";
		}
	}
	void stop() {
		if (driver_inside) {
			engine.stop();
			if (threads.engine_idle_thread.joinable()) {
				threads.engine_idle_thread.join();

				cout << "Двигатель заглушен.\n";
			}
		}
	}


	void control() {
		char key = 0;
		do {
			key = 0;
			if (_kbhit()) {  // Функция _kbhit() возвращает 
				key = _getch();
			}

			switch (key) {

			case Enter:
				if (driver_inside && speed == 0) {
					get_out();
				}else if(!driver_inside && speed == 0) {
					get_in();
				}
				break;


			case 'F': case'f':
			{
				if (driver_inside) {
					cout << "Для начала нужно выйти из машины." << endl;
					std::this_thread::sleep_for(2s);
					//Sleep(2000);
					break;
				}
				double fuel;
				cout << "Введите объём топлива: "; cin >> fuel;
				tank.fill(fuel);
				break;
			}

			case 'I': case 'i': // Ignition - зажигание
			{
				if (engine.started()) {
					stop();
				}
				else {
					start();
				}
				break;
			}
			case 'W':case'w':
				accellerate();
				break;
			case 'S':case's':
				slow_down();
				break;

			case Escape:
				if (speed != 0) {
					speed = 0;
				}
				stop();
				get_out();
				break;


			default:
				key = 0;
				break;
			}

			if (tank.get_fuel_level() == 0) {
				stop();
			}
			if (speed <= 0) {
				engine.set_consumption_per_second(speed = 0);
			}
			if (speed == 0 && threads.free_wheeling_thread.joinable()) {
				threads.free_wheeling_thread.join();
			}
			if (speed == 0 && threads.total_distance.joinable()) {
				threads.total_distance.join();
			}
			if (!engine.started() && threads.timer.joinable()) {
				threads.timer.join();
			}
		} while (key != Escape);
	}


	void engine_idle() {
		while (engine.started() && tank.give_fuel(engine.get_consumption_per_second())) {
			std::this_thread::sleep_for(1s);
			
		}
	}

	void free_wheeling() {
		while (--speed) {
			std::this_thread::sleep_for(1s);
			engine.set_consumption_per_second(speed);
		}
	}

	void accellerate() {
		if (engine.started() && driver_inside != false) {
			speed += accelleration;
			if (speed > MAX_SPEED) {
				speed = MAX_SPEED;
			}

			if (!threads.free_wheeling_thread.joinable()) {
				threads.free_wheeling_thread = std::thread(&Car::free_wheeling, this);
			}
			if (!threads.total_distance.joinable()) {
				threads.total_distance = std::thread(&Car::total_distance, this);
			}
			
			std::this_thread::sleep_for(1s);
		}
	}

	void slow_down() {
		if (driver_inside != false) {
			if (speed > accelleration) {
				speed -= accelleration;
			}
			std::this_thread::sleep_for(1s);
		}
	}

	void speedometr()const {
		for (int i = 0; i < (MAX_SPEED / 3) / 2; i++) { cout << " "; }
		cout << "Speedometr\n+";
		for (int i = 0; i < MAX_SPEED / 3; i++) { cout << "-"; } cout << "+\n";

		for (int i = 0; i < speed / 3; i++) {
			cout << "|";
		}
		cout << "\n+";
		for (int i = 0; i < MAX_SPEED / 3; i++) { cout << "-"; } cout << "+\n";
	}

	void Tanker()const {
		cout << "\n TANK\n";
		cout << "===-===\n";
		if (tank.get_fuel_level() >= tank.get_VOLUME() * 0.25) {
			cout << " =---= \n";
		}
		else {
			cout << " =   = \n";
		}
		if (tank.get_fuel_level() >= tank.get_VOLUME() * 0.5) {
			cout << " =---= \n";
		}
		else {
			cout << " =   = \n";
		}
		if (tank.get_fuel_level() >= tank.get_VOLUME() * 0.75) {
			cout << " =---= \n";
		}
		else {
			cout << " =   = \n";
		}
		if (tank.get_fuel_level() != 0) {
			cout << " =---= \n";
		}
		else {
			cout << " =   = \n";
		}
		cout << "===-===\n";
	}

	void total_distance() {
		while(speed != 0) {
			std::this_thread::sleep_for(1s);
			distance += (speed / 3.6);
		}
	}

	void driving_time() {
		while (engine.started() != 0) {
			std::this_thread::sleep_for(1s);
			time++;
		}
	}
	void panel()const {
		while (driver_inside) {
			system("cls");

			{ // Чисто визуал
				speedometr();
				Tanker();
			}cout << endl;

			cout << "Fuel level:\t\t\t" << tank.get_fuel_level() << " liters.\t";

			/*if (tank.get_fuel_level() < 5) {
				cout << "LOW FUEL" << endl;
			}*/
			cout << "\n\t\t\t\t";
			if (tank.get_fuel_level() < 5) {
				HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
				SetConsoleTextAttribute(hConsole, 0xCE);
				cout << " LOW FUEL ";
				SetConsoleTextAttribute(hConsole, 0x07);
			}cout << endl;

			cout << "Engine is " << (engine.started() ? "started" : "stoped") << endl;
			cout << "Speed:\t\t\t\t" << speed << " km/h.\n";
			cout << "Consumption per second:\t\t" << engine.get_consumption_per_second() << " liters.\n";
			cout << endl;
			cout << "Total distance:\t\t\t" << distance / 1000 << " km." << "\n\t\t\t\t" << (int)distance << " m." << endl;
			cout << "Total driving timer\t\t" << (time / 3600.0) << " hours." << "\n\t\t\t\t" << (int)time << " seconds." << endl;
			std::this_thread::sleep_for(500ms);
		}
	}
};



//#define TANK_CHECK
//#define ENGINE_CHECK
#define CAR_CHECK

int main()
{
	setlocale(0, "");

#ifdef TANK_CHECK

	Tank tank(150);
	do {
		int fuel;
		cout << "Введите уровень топлива: "; cin >> fuel;

		tank.fill(fuel);
		tank.info();
	} while (true);

#endif // TANK_CHECK


#ifdef ENGINE_CHECK

	Engine engine(10);
	engine.info();

#endif // ENGINE_CHECK


#ifdef CAR_CHECK

	Car bmw(10, 40, 250);
	bmw.control();

#endif // CAR_CHECK
}





