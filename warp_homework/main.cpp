#include <iostream>
#include <string>
#include <random>
#include <Windows.h>
#include <time.h>

using namespace std;

bool playerTurn = true;
random_device rd;
uniform_int_distribution<int> ran(1, 2);
uniform_int_distribution<int> ranstatus(5, 30);

class Item;

class Object {
protected:
	int hp;
public:
	void hit(int atk) {
		hp = hp - atk;
	}

	bool died() const {
		return hp > 0 ? false : true;
	}

	virtual void printInfo(ostream& os) const = 0;

	friend ostream& operator<<(ostream& os, const Object& o) {
		o.printInfo(os);
		return os;
	}

	explicit operator bool() const {
		return !died();
	}
};

class Monster : public Object {
private:
	int num;
	int atk;
	int def;
public:
	Monster(int tnum) : num{ tnum }, atk{ ranstatus(rd) }, def{ ranstatus(rd) } {
		hp = 100;
	};

	void attack(Object* o) {
		o->hit(atk);
	}

	virtual void printInfo(ostream& os) const override {
		os << num << "번 몬스터" << endl;
		os << "체력 : " << hp << endl;
		os << "공격력 : " << atk << endl;
		os << "방어력 : " << def << endl;
		os << endl;
	}
};

class Player : public Object {
private:
	string id;
	int atk;
	int def;
	int exp;
	int level;
public:
	Player(string tid) : id{ tid }, atk{ 20 }, def{ 10 }, exp{ 0 }, level{ 1 } {
		hp = 200; 
	};
	
	void attack(Object* o) {
		o->hit(atk);
	}

	void checkLevel() {
		if (exp > 0 && exp % 20 == 0) level++;
	}

	Player& operator+=(int gainedExp) {
		exp += gainedExp;
		checkLevel();
		return *this;
	}

	virtual void printInfo(ostream& os) const override {
		os << "*** " << id << " ***" << endl;
		os << "레벨 : " << level << endl;
		os << "체력 : " << hp << endl;
		os << "공격력 : " << atk << endl;
		os << "방어력 : " << def << endl;
		os << "경험치 : " << exp << endl;
		os << endl;
	}
};

class Npc : public Object {
private:

public:
	Npc() { hp = 100; };

	virtual void printInfo(ostream& os) const override {
		os << "Npc 체력 : " << hp << endl;
		os << endl;
	}
};

class Item {
};

int main() {
	string pID;
	cout << "ID : ";
	cin >> pID;
	Player p{ pID };
	Monster m[3] {1, 2, 3};
	Npc n;
	static int monsterNum{ 0 };
	int order{ 0 };

	while (1) {
		system("cls");
		cout << p << endl;
		cout << m[0] << endl;

		cout << "1. 상점" << endl;
		cout << "2. 싸우러 가자" << endl;
		cout << "3. 인벤토리" << endl;
		if (p) cout << "아직 살아있음" << endl;
		cout << "명령어 : ";
		cin >> order;

		if (playerTurn) {
			int num;
			if (!m[0].died()) cout << "1. 1번 몬스터 공격" << endl;
			if (!m[1].died()) cout << "2. 2번 몬스터 공격" << endl;
			if (!m[2].died()) cout << "3. 3번 몬스터 공격" << endl;
			if (!n.died()) cout << "4. Npc 공격" << endl;
			cout << "명령어 : ";
			cin >> num;
			if (num != 4) {
				cout << num << "번 몬스터 공격" << endl;
			}
			else {
				cout << "npc 공격" << endl;
			}
			playerTurn = false;
		}
		else
		{
			int hitWho = ran(rd);
			if (n.died()) {
				hitWho = 1;
			}
			if (hitWho == 1) {	// 플레이어 때림

			}
			else {	// Npc 때림

			}
			playerTurn = true;
		}
		Sleep(1500);
	}
}