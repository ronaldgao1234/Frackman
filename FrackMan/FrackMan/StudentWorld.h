#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include <iostream>
#include "GameWorld.h"
#include "GameConstants.h"
#include <string>
#include <vector>

using namespace std;

class Actor;
class Frackman;
class Dirt;


class StudentWorld : public GameWorld
{
public:
	StudentWorld(string assetDir) : GameWorld(assetDir){};
	~StudentWorld();
//
//	/*responsible for creating the current level’s oil field and populating
//		it with Dirt, Boulders, Barrels of Oil, and Gold Nuggets(we’ll show you how below),
//		and constructing a virtual representation of the current level in your StudentWorld class,
//		using one or more data structures that you come up with.*/
	virtual int init();
//
	virtual int move();

	virtual void cleanUp();

	void setDisplayText();
	string setDigits(int item, int length);
	string makeDisplayNeat(unsigned score, unsigned level, unsigned lives, unsigned health, unsigned squirts, 
		unsigned gold, unsigned sonar, unsigned barrelsLeft);
	
	bool checkforDirt(int lx, int ly);
	bool checkforBoulder(unsigned lx, unsigned ly);
	void destroyDirt(int i, int j);

	void getActors(unsigned identity, vector <Actor*> &temp);
	void addActors(int count, char c, int sr = 0, int sc = 0);
	Frackman* getFm()const{ return fm; };
	//returns the number of Barrels you set it to

	void decBarrels(){numberOfBarrels--;};
	unsigned getnumberOfBarrels(){return numberOfBarrels;};

	double distance(int x1, int y1, int x2, int y2);

private:
	Frackman* fm;
	vector <Actor*> v;
	Dirt* dirtArray[VIEW_WIDTH][VIEW_HEIGHT];
	unsigned ticksSinceLastProtester;
	int numberOfBarrels;
	int numberOfProtesters;
};

#endif // STUDENTWORLD_H_
