#ifndef ACTOR_H_
#define ACTOR_H_

#include <iostream>
#include "GraphObject.h"
#include "StudentWorld.h"
#include <queue>
using namespace std;

class Actor : public GraphObject
{
public:
	Actor(StudentWorld* sw, int imageID, int startX, int startY, Direction startDirection, float size, unsigned int depth);
	virtual void getAnnoyed(unsigned pts) =0 ;
	virtual void doSomething() = 0;
	void setStatus(bool b);
	bool isAlive() const;
	virtual unsigned returnIdentity() = 0;
	StudentWorld* getSW() const;

	bool isVisi(){ return visible; };
	void setVisi(bool b){ visible = b; };

	bool canMoveTo(Direction dir, int X, int Y);
	bool canMoveInDir(Direction dir);
	void moveInDir(Direction dir);
	virtual bool getleavetheOil(){ return false; };
private:
	bool visible;
	bool alive;
	StudentWorld* m_studentWorld;
};
//each actor has an identityNumber from Gameconstant.h. just use image id
//FRACKMAN///////////////////////////////////////////////////////////////////////////////////
class Frackman : public Actor
{
public:
	Frackman(StudentWorld* sw);
	virtual void getAnnoyed(unsigned pts);
	unsigned getHP(){ return hitPoints; };
	unsigned getunitWater(){ return unitWater; }; 
	unsigned getGold(){ return gold; };
	unsigned getSonar(){ return sonarCharge; };
	bool noBoulderBlock(Direction dir);
	void incSonarBy(int i){ sonarCharge += i; };
	void incWaterBy(int i){ unitWater += i; };
	void incGold(){ gold++; };
	virtual void doSomething();
	virtual unsigned returnIdentity() { return 0; }; // technically don't need this
	double distanceToBoulder(double x1, double y1, double x2, double y2);
private:
	unsigned gold;
	int hitPoints;
	unsigned unitWater;
	unsigned sonarCharge;

};

//DIRT////////////////////////////////////////////////////////////////////////////////////
class Dirt : public Actor
{
public:
	Dirt(StudentWorld* sw, int startX, int startY);
	virtual unsigned returnIdentity() { return 6; };
	virtual void getAnnoyed(unsigned pts){};
	virtual void doSomething(){};
};

//BOUDLER///////////////////////////////////////////////////////////////////////////////
class Boulder : public Actor
{
public:
	Boulder(StudentWorld* sw, int startX, int startY);
	virtual unsigned returnIdentity() {return 4;};
	virtual void getAnnoyed(unsigned pts){};
	virtual void doSomething();
private:
	bool stable;
	bool waitingState; 
	bool fallingState;
	int waitingSec;
};
//SQUIRT///////////////////////////////////////////////////////////////////////////////
class Squirt : public Actor
{
public:
	Squirt(StudentWorld* sw, int startX, int startY, Direction dir);
	virtual unsigned returnIdentity() { return 3; };
	virtual void getAnnoyed(unsigned pts) {};
	virtual void doSomething();
private:
	unsigned travelDistance;
};
//BARREL///////////////////////////////////////////////////////////////////////////////
class Barrel : public Actor
{
public:
	Barrel(StudentWorld* sw, int startX, int startY);
	virtual unsigned returnIdentity() { return 5; };
	virtual void getAnnoyed(unsigned pts) {};
	virtual void doSomething();
private:
	
};

//GOLD NUGGET///////////////////////////////////////////////////////////////////////////////
class Gold : public Actor
{
public:
	Gold(StudentWorld* sw, int startX, int startY, bool v, bool tempState, bool pickUpableF, unsigned ts);
	virtual unsigned returnIdentity() { return 7; };
	virtual void getAnnoyed(unsigned pts){};
	virtual void doSomething();
private:
	bool tempState;
	int tempSec;
	bool pickUpableF;
};

//SONAR KIT///////////////////////////////////////////////////////////////////////////////
class Sonar : public Actor
{
public:
	Sonar(StudentWorld* sw, int startX, int startY, unsigned ts);
	virtual void getAnnoyed(unsigned pts){};
	virtual unsigned returnIdentity() { return 8; };
	virtual void doSomething();
private:
	int tempSec;
};

//Water Pool///////////////////////////////////////////////////////////////////////////////
class Water : public Actor
{
public:
	Water(StudentWorld* sw, int startX, int startY, unsigned ts);
	virtual void getAnnoyed(unsigned pts){};
	virtual unsigned returnIdentity() { return 9; };
	virtual void doSomething();
private:
	int tempSec;
};


//REGULAR PROTESTER///////////////////////////////////////////////////////////////////////////////
class RegProtester : public Actor
{
public:
	struct Coord{
		Coord(int q, int w) : r(q), c(w){}
		int getR() const{ return r; };
		int getC() const{ return c; };
		bool operator<(const RegProtester::Coord &a) const
		{
			if (this->getC() != a.getC())
				return this->getC() < a.getC();
			return (this->getR() < a.getR());
		}
		
	private:
		int r;
		int c;
	};
	
	RegProtester(StudentWorld* sw, int startX, int startY, int id, int leftSquares, int tw, int hp);
	virtual unsigned returnIdentity() { return 1; };
	unsigned getHP(){ return hitPoints; };
	void setHP(int i){ hitPoints = i; };
	virtual void getAnnoyed(unsigned pts);
	virtual void doSomething();
	void returnArr(char (&arr)[64][64]);
	bool explore(queue<Coord>& qu, char(&arr)[64][64], int sr, int sc);
	bool checkSquare(int sr, int sc, char (&arr)[64][64]);
	bool facingFm();
	Direction lineOfSight();
	int getNumOfLeftSquares(){ return numSquaresToMoveInCurrentDirection; };
	void setNumOfLeftSquares(int i){ numSquaresToMoveInCurrentDirection = i; };
	unsigned getticksToWait(){ return ticksToWait; };
	void decticksW(){ ticksToWait--; };
	void setTicksToWait(int i){ ticksToWait = i; };
	void setleavetheOil(bool b){ leavetheOil = b; };
	virtual bool getleavetheOil(){ return leavetheOil; };
	void setperpSec(int i){ perpSec = i; };
	bool canPerp(Direction currDir);
	int getperpSec(){ return perpSec; };
	bool goBack();
	int getshoutSec(){ return shoutSec; };
	void decshoutSec(){ shoutSec--; };
	void resetshoutSec(){ shoutSec = 15; };
private:
	int perpSec;
	int shoutSec;
	int ticksToWait;
	int originalTicks;
	bool leavetheOil;
	int hitPoints;
	int numSquaresToMoveInCurrentDirection;
	
};

class HardProtester : public RegProtester{
public:
	HardProtester(StudentWorld* sw, int startX, int startY,int id, int leftSquares, int tw);
	//the getAnnoyed function works for both
	virtual unsigned returnIdentity(){ return 2; };
	virtual void doSomething();
private:
	int orgTicks;
};

#endif // ACTOR_H_
