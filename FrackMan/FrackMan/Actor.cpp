#include "Actor.h"
#include "GraphObject.h"
#include "StudentWorld.h"
#include "GameWorld.h"
#include "GameConstants.h"
#include <queue>
#include <map>
#include <iostream>
#include <algorithm>
#include <iomanip>
using namespace std;
//ACTOR IMPLEMENTATION////////////////////////////////////////////////

Actor::Actor(StudentWorld* sw, int imageID, int startX, int startY, Direction startDirection, float size, unsigned int depth) :
GraphObject(imageID, startX, startY, startDirection, size, depth), m_studentWorld(sw), alive(true)
{
	setVisible(true);
}

void Actor::setStatus(bool b){
	if (b)
		alive = true;
	alive = false;
}
bool Actor::isAlive() const{
	if (alive)
		return true;
	return false;
}

StudentWorld* Actor::getSW() const{
	return m_studentWorld;
}

//in this direction dir can you move to coordinate x,y
bool Actor::canMoveTo(Direction dir, int X, int Y){
	vector<Actor*>boulder;
	getSW()->getActors(4, boulder);
	//right
	if (dir == right){
		if (X > 60)
			return false;
		int i = 0;
		while (getX() + i != X + 1){
			for (int s = 0; s < 4; s++){
				for (int o = 0; o < 4; o++){
					//check for dirt
					if (getSW()->checkforDirt(getX() + i + s, getY() + o))
						return false;

					//check for boulders
					if (getSW()->checkforBoulder(getX() + i + s, getY() + o))
						return false;
				}
			}
			i++;
		}
		return true;
	}
	else if (dir == left){
		if (X < 0)
			return false;
		int i = 0;
		while (getX() - i != X - 1){
			for (int l = 0; l < 4; l++){
				for (int p = 0; p < 4; p++){
					if (getSW()->checkforDirt(getX() - i + l, getY() + p))
						return false;
					if (getSW()->checkforBoulder(getX() - i + l, getY() + p))
						return false;
				}
			}
			i++;
		}
		return true;
	}
	else if (dir == up){
		if (Y > 60)
			return false;
		int i = 0;
		while (getY() + i != Y + 1){
			for (int l = 0; l < 4; l++){
				for (int p = 0; p < 4; p++){
					if (getSW()->checkforDirt(getX() + l, getY() + i + p))
						return false;
					if (getSW()->checkforBoulder(getX() + l, getY() + i + p))
						return false;
				}
			}
			i++;
		}
		return true;
	}
	else if (dir == down){
		if (Y < 0)
			return false;
		int i = 0;
		while (getY() - i != Y - 1){
			for (int l = 0; l < 4; l++){
				for (int p = 0; p < 4; p++){
					if (getSW()->checkforDirt(getX() + l, getY() - i + p))
						return false;
					if (getSW()->checkforBoulder(getX() + l, getY() - i + p))
						return false;
				}
			}
			i++;
		}
		return true;
	}
	return false; // if dir == none
}

bool Actor::canMoveInDir(Direction dir){
	if (dir == right){
		if (canMoveTo(right, getX() + 1, getY()))
			return true;
	}
	else if (dir == left){
		if (canMoveTo(left, getX() - 1, getY()))
			return true;
	}
	else if (dir == up){
		if (canMoveTo(up, getX(), getY() + 1))
			return true;
	}
	else if (dir == down){
		if (canMoveTo(down, getX(), getY() - 1))
			return true;
	}
	return false;
}

void Actor::moveInDir(Direction dir){
	switch (dir){
	case right:
		moveTo(getX() + 1, getY());
		break;
	case left:
		moveTo(getX() - 1, getY());
		break;
	case up:
		moveTo(getX(), getY() + 1);
		break;
	case down:
		moveTo(getX(), getY() - 1);
		break;
	default:
		break;
	}
}

////Frackman//////////////////////////////////////////////////////////////

Frackman::Frackman(StudentWorld* sw): Actor(sw, IID_PLAYER, 30, 60, right, 1, 0), hitPoints(10), unitWater(5), 
sonarCharge(1), gold(0)
{}


void Frackman::doSomething(){
	if (isAlive() == false)
		return;
	//if the image currently overlaps with any dirt
	for (int i = getX(); i < getX() + 4 && i < 63 + 1 && i >= 0; i++){
		for (int j = getY(); j < getY() + 4 && j < 60 && j >= 0; j++){
			if (getSW()->checkforDirt(i, j)){
				getSW()->destroyDirt(i, j);
				getSW()->playSound(SOUND_DIG);
			}

		}
	}
	//how frackman reacts to key press
	int ch = 0;
	int x = getX();
	int y = getY();
	bool dontMove = false;
	
	if (getSW()->getKey(ch))
	{
		switch (ch){
		case KEY_PRESS_ESCAPE:
			setStatus(false);
			dontMove = true;
			break;
		case KEY_PRESS_SPACE:
			if (unitWater <= 0){
				dontMove = true;
				break;
			}
			unitWater--;
			//checking four squares ahead
			if (getDirection() == right){
				if (canMoveTo(right, getX() + 4, getY())){
					getSW()->addActors(1, 's', getX() + 4, getY());
				}

			}
			else if(getDirection() == left){
				if (canMoveTo(left, getX() - 4, getY())){
					getSW()->addActors(1, 's', getX() - 4, getY());
				}
			} 
			else if(getDirection() == up){
				if (canMoveTo(up, getX(), getY() + 4)){
					getSW()->addActors(1, 's', getX(), getY() + 4);
				}
			}
			else if(getDirection() == down){
				if (canMoveTo(down, getX(), getY() - 4)){
					getSW()->addActors(1, 's', getX(), getY() - 4);
				}
			}
			//either way we are going to make this sound
			getSW()->playSound(SOUND_PLAYER_SQUIRT);
			dontMove = true;
			break;
		case 'z':
		case 'Z':
			if (sonarCharge >= 1){
				getSW()->playSound(SOUND_SONAR);
				vector<Actor*> vec;
				getSW()->getActors(5, vec);
				getSW()->getActors(7, vec);
				for (int i = 0; i < static_cast<int>(vec.size()); i++){
					if (getSW()->distance(vec[i]->getX(), vec[i]->getY(), getX(), getY()) <= 12){
						vec[i]->setVisible(true);
						vec[i]->setVisi(true);
					}
				}
				sonarCharge--;
				dontMove = true;
			}
			break;
		case KEY_PRESS_TAB:
			if (gold >= 1){
				getSW()->addActors(1, 'g', 99 ,99); //99 so that it knows that it came from frackman
				gold--;
			}
			dontMove = true;
			break;
		case KEY_PRESS_LEFT:
			if (getDirection() == left){
				if (noBoulderBlock(left)){
					x = getX() - 1;
					y = getY();
				}
				else
					dontMove = true;
			}
			else
				setDirection(left);
			break;
		case KEY_PRESS_RIGHT:
			if (getDirection() == right){
				if (noBoulderBlock(right)){
					x = getX() + 1;
					y = getY();
				}
				else dontMove = true;
			}
			else
				setDirection(right);
			break;
		case KEY_PRESS_UP:
			if (getDirection() == up){
				if (noBoulderBlock(up)){
					x = getX();
					y = getY() + 1;
				}
				else
					dontMove = true;
			}
			else
				setDirection(up);
			break;
		case KEY_PRESS_DOWN:
			if (getDirection() == down){
				if (noBoulderBlock(down)){
					x = getX();
					y = getY() - 1;
				}
				else
					dontMove = true;
			}
			else
				setDirection(down);
			break;
		default:
			x = getX();
			y = getY();
			break;
		}

		if (x >= 0 && y >= 0 && x <= 60 && y <= 60 && dontMove == false)
			this->moveTo(x, y);
		
	}
}
//frackman specify squirt direction
void Frackman::getAnnoyed(unsigned pts){
	hitPoints -= pts;
	if (hitPoints <= 0){
		setStatus(false);
		getSW()->playSound(SOUND_PLAYER_GIVE_UP);
	}
}

bool Frackman::noBoulderBlock(Direction dir){
	vector<Actor*>boulder;
	getSW()->getActors(4, boulder);
	for (int ind = 0; ind < static_cast<int>(boulder.size()); ind++){
		if (dir == right){
			for (int i = 0; i < 4; i++){
				for (int j = 0; j < 4; j++){
					if (distanceToBoulder(boulder[ind]->getX() + 1.5, boulder[ind]->getY() + 1.5, getX() + i + 1, getY() + j) <= 1.5)
						return false;
				/*	cout << boulder[ind]->getX() + 1.5 << " " << boulder[ind]->getY() + 1.5 << endl;*/
				}
			}
		}
		else if (dir == left){
			for (int i = 0; i < 4; i++){
				for (int j = 0; j < 4; j++){
					if (distanceToBoulder(boulder[ind]->getX() + 1.5, boulder[ind]->getY() + 1.5, getX() + i - 1, getY() + j) <= 1.5)
						return false;
				}
			}
		}
		else if (dir == up){
			for (int i = 0; i < 4; i++){
				for (int j = 0; j < 4; j++){
					if (distanceToBoulder(boulder[ind]->getX() + 1.5, boulder[ind]->getY() + 1.5, getX() + i, getY() + j + 1) <= 1.5)
						return false;
				}
			}
		}
		else if (dir == down){
			for (int i = 0; i < 4; i++){
				for (int j = 0; j < 4; j++){
					if (distanceToBoulder(boulder[ind]->getX() + 1.5, boulder[ind]->getY() + 1.5, getX() + i, getY() + j - 1) <= 1.5)
						return false;
				}
			}
		}		
	}
	return true;
}

double Frackman::distanceToBoulder(double x1, double y1, double x2, double y2){
	return sqrt(((x1 - x2)*(x1 - x2)) + (y1 - y2)*(y1 - y2));
}

//Dirt////////////////////////////////////////////////////////////////////////////

Dirt::Dirt(StudentWorld* sw, int startX, int startY) :
Actor(sw,IID_DIRT, startX, startY, right, .25, 3)
{}

//Boulder///////////////////////////////////////////////////////////////////////////

Boulder::Boulder(StudentWorld* sw, int startX, int startY):Actor(sw, IID_BOULDER, startX, startY,down,1,1), 
stable(true), waitingState(false),fallingState(false), waitingSec(0)
{}

void Boulder::doSomething(){
	if (!isAlive())
		return;

	if (stable){
		unsigned c = 0;
		//check for stability
		for (size_t i = 0; i < 4; i++){
			if (getSW()->checkforDirt((getX() + i), getY() - 1))
				c++;
		}
		if (c == 0){
			stable = false;
			waitingState = true;
		}
	}
	
	if (waitingState)
		waitingSec++;
	//31 because the first one will be immediately after the first time it goes into waiting state
	if (waitingSec == 31){
		waitingState = false;
		fallingState = true;
		waitingSec = 0;
		getSW()->playSound(SOUND_FALLING_ROCK);
	}
	if (fallingState)
	{
		bool conditionMet = false;
		//checks if it went below the screen
		if (getY() - 1 >= 0){
			moveTo(getX(), getY() - 1);
		}
		else conditionMet = true;
		if (!conditionMet){
			//store the boulder pointers into b
			vector<Actor*> b;
			getSW()->getActors(4,b);
			for (vector<Actor*>::iterator it = b.begin(); it != b.end(); it++){
				//check if this boulder meets the top of another boulder. even on the side
				if (this->getY() == (*it)->getY() + 3){
					if (this->getX() <= (*it)->getX() && this->getX() + 3 >= (*it)->getX())
						conditionMet = true;
					if (this->getX() <= (*it)->getX() + 3 && this->getX() + 3 >= (*it)->getX() + 3)
						conditionMet = true;
				}
			}
		}
		if (!conditionMet){
			for (size_t i = 0; i < 4; i++){
				if (getSW()->checkforDirt(getX() + i, getY()-1))
					conditionMet = true;
			}
		}
		if (conditionMet){
			this->setStatus(false);
			fallingState = false;
		}

		//get Protesters 
		vector<Actor*>p;
		getSW()->getActors(1, p);
		getSW()->getActors(2, p);

		//annoy the frackman if boulder falls on him
		if (getSW()->distance(getSW()->getFm()->getX(), getSW()->getFm()->getY(), getX(), getY()) <= 3){
			getSW()->getFm()->getAnnoyed(10);
		}

		//annoy the protesters if the boulders fall on them
		for (vector < Actor*>::iterator ii = p.begin(); ii != p.end(); ii++){
			if (getSW()->distance((*ii)->getX(), (*ii)->getY(), getX(), getY()) <= 3){
				(*ii)->getAnnoyed(10);
				getSW()->increaseScore(500);
			}
		}
	}
}

//SQUIRT/////////////////////////////////////////////////////////////////////////////
Squirt::Squirt(StudentWorld* sw, int startX, int startY, Direction dir) : Actor(sw, IID_WATER_SPURT, startX, startY, dir, 1,1),
travelDistance(4)
{}

void Squirt::doSomething(){
	//check if it hit any protesters
	vector<Actor*> protesters;
	getSW()->getActors(1, protesters);
	getSW()->getActors(2, protesters);
	for (size_t i = 0; i < protesters.size(); i++){
		if (getSW()->distance(protesters[i]->getX(), protesters[i]->getY(), getX(), getY()) <= 3 && !protesters[i]->getleavetheOil()){
			protesters[i]->getAnnoyed(2);
			setStatus(false);
		}
	}
	//check if it traveled full travel distance
	if (travelDistance == 0){
		setStatus(false);
	}
	else{
		if (canMoveInDir(getDirection())){
			moveInDir(getDirection());
			travelDistance--;
		}
		else
			setStatus(false);
	}

}

//BARREL////////////////////////////////////////////////////////////////////////

//constructor
Barrel::Barrel(StudentWorld* sw, int startX, int startY) : Actor(sw, IID_BARREL, startX, startY, right, 1, 2)
{
	setVisible(false);
	setVisi(false);
}

void Barrel::doSomething(){
	if (!isAlive())
		return;
	if (isVisi() == false && getSW()->distance(getSW()->getFm()->getX(), getSW()->getFm()->getY(), getX(), getY()) <= 4){
		setVisible(true);
		setVisi(true);
		return;
	}
	if (getSW()->distance(getSW()->getFm()->getX(), getSW()->getFm()->getY(), getX(), getY()) <= 3){
		setStatus(false);
		getSW()->playSound(SOUND_FOUND_OIL);
		getSW()->increaseScore(1000);
		getSW()->decBarrels();
	}
}

//GOLD////////////////////////////////////////////////////////////////////////


Gold::Gold(StudentWorld* sw, int startX, int startY, bool v, bool tempState, bool pickUpableF, unsigned ts) : Actor(sw, IID_GOLD, startX, startY, right, 1, 2), tempState(tempState), pickUpableF(pickUpableF), tempSec(ts)
{
	setVisible(v);
	setVisi(v);
}


void Gold::doSomething()
{
	//get protestors
	vector<Actor*> vec;
	getSW()->getActors(1, vec);
	getSW()->getActors(2, vec);
 //don't forget to decrement tempsec
	if (!isAlive())
		return;
	//near frackman
	else if (!isVisi() && getSW()->distance(getSW()->getFm()->getX(), getSW()->getFm()->getY(), getX(), getY()) <= 4){
		setVisible(true);
		setVisi(true);
		return;
	}
	//close to frackman to be picked up
	else if (pickUpableF && getSW()->distance(getSW()->getFm()->getX(), getSW()->getFm()->getY(), getX(), getY()) <= 3){
		setStatus(false);
		getSW()->playSound(SOUND_GOT_GOODIE);
		getSW()->increaseScore(10);
		getSW()->getFm()->incGold();  
	}
	else{
		//picked up by protesters
		for (int i = 0; i < static_cast<int>(vec.size()); i++){
			if (!pickUpableF && getSW()->distance(vec[i]->getX(), vec[i]->getY(), getX(), getY()) <= 3){
				setStatus(false);
				tempSec = 0;
				getSW()->playSound(SOUND_PROTESTER_FOUND_GOLD);

				int N = max(50, 100 - static_cast<int>(getSW()->getLevel() * 10));
				if (vec[i]->returnIdentity() == 2){ 
					getSW()->increaseScore(50);
					HardProtester* hp = dynamic_cast<HardProtester*>(vec[i]);
					hp->setTicksToWait(hp->getticksToWait() + N);
					// protester
				
				}
				else{
					RegProtester* rp = dynamic_cast<RegProtester*>(vec[i]);
					getSW()->increaseScore(25);
					rp->getAnnoyed(99); // this will be just for gold nuggets for regular protesters
				}
				
			}
		}
	}
	if (tempState){
		if (tempSec == 0)
			setStatus(false);
		tempSec--;
	}

}

//SONAR KIT////////////////////////////////////////////////////////////////////////
Sonar::Sonar(StudentWorld* sw, int startX, int startY, unsigned ts) : Actor(sw, IID_SONAR, startX, startY, right, 1, 2),
tempSec(ts)
{}

void Sonar::doSomething(){
	if (!isAlive())
		return;
	if (getSW()->distance(getSW()->getFm()->getX(), getSW()->getFm()->getY(), getX(), getY()) <= 3){
		setStatus(false);
		getSW()->playSound(SOUND_GOT_GOODIE);
		getSW()->getFm()->incSonarBy(2);
		getSW()->increaseScore(75);
	}
	if (tempSec == 0){
		setStatus(false);
	}
	else
	{
		tempSec--;
	};
}
//WATER POOL////////////////////////////////////////////////////////////////////////
Water::Water(StudentWorld* sw, int startX, int startY, unsigned ts) : Actor(sw, IID_WATER_POOL, startX, startY, right, 1, 2),
tempSec(ts)
{}

void Water::doSomething(){
	if (!isAlive())
		return;
	if (getSW()->distance(getSW()->getFm()->getX(), getSW()->getFm()->getY(), getX(), getY()) <= 3){
		setStatus(false);
		getSW()->playSound(SOUND_GOT_GOODIE);
		getSW()->getFm()->incWaterBy(5);
		getSW()->increaseScore(100);
	}
	if (tempSec == 0)
		setStatus(false);
	else 
		tempSec--;
}

//REGULAR PROTESTER////////////////////////////////////////////////////////////////////////
	
RegProtester::RegProtester(StudentWorld* sw, int startX, int startY, int id, int leftSquares, int tw, int hp) : Actor(sw, id, startX, startY, left, 1, 0),
numSquaresToMoveInCurrentDirection(leftSquares), hitPoints(hp), leavetheOil(false), ticksToWait(tw), originalTicks(tw), shoutSec(0), perpSec(200)
{}

void RegProtester::doSomething(){
	if (!isAlive())
		return;
	//waiting period

	bool skiptoEight = false;
	if (ticksToWait != 0){
		ticksToWait--;
		return;
	}
	else{
		//wait is over
		ticksToWait = originalTicks;

		//waiting period for perpSec. decrement no matter what happens. that's why it starts at 201
		perpSec--;
		if (perpSec < 0)
			perpSec = 0;
		//walk back to 60, 60
		if (leavetheOil){
			if (goBack())
				return;
		}

		//4
		else if (getSW()->distance(getSW()->getFm()->getX(), getSW()->getFm()->getY(), getX(), getY()) <= 4 && facingFm()){
			//done waiting for 15 shoutseconds
			if (shoutSec == 0){
				getSW()->playSound(SOUND_PROTESTER_YELL);
				getSW()->getFm()->getAnnoyed(2);
				shoutSec = 15;
			}
			else shoutSec--;
			return;
		}
		//5
		else if (lineOfSight() != none && getSW()->distance(getSW()->getFm()->getX(), getSW()->getFm()->getY(), getX(), getY()) > 4 && canMoveTo(lineOfSight(), getSW()->getFm()->getX(), getSW()->getFm()->getY())){
				Direction ls = lineOfSight(); // returns direction to go
				setDirection(ls); //set direction then take a step
				if (ls == right)
					moveTo(getX() + 1, getY());
				else if (ls == left)
					moveTo(getX() - 1, getY());
				else if (ls == up)
					moveTo(getX(), getY() + 1);
				else if (ls == down)
					moveTo(getX(), getY() - 1);
				if (lineOfSight() != ls) //doesn't see the frackman after moving
					numSquaresToMoveInCurrentDirection = 0;
				return;
				
		}
		//6 Cant directly see the frackman 
		else{
			numSquaresToMoveInCurrentDirection--;
			if (numSquaresToMoveInCurrentDirection <= 0){

				unsigned r = rand() % 4;
				size_t c = 0;
				while (c == 0){
					//right
					if (r == 0){
						if (canMoveInDir(right))
						{
							setDirection(right);
							c++;
						}
						else r++;
					}
					if (r == 1){
						if (canMoveInDir(left))
						{
							setDirection(left);
							c++;
						}
						else r++;
					}
					if (r == 2){
						if (canMoveInDir(up))
						{
							setDirection(up);
							c++;
						}
						else r++;
					}
					if (r == 3){
						if (canMoveInDir(down))
						{
							setDirection(down);
							c++;
						}
						else r = 0;
					}
				}
				numSquaresToMoveInCurrentDirection = (rand() % (60 - 8)) + 8;
				skiptoEight = true;
			}
		}
		//7
		//can go in at least one of the perp. directions
		if (!skiptoEight){
			bool first = false;
			bool sec = false;
			if (canPerp(getDirection())){
				if (perpSec <= 0){
					if (getDirection() == right || getDirection() == left){
						//check up and down
						first = canMoveTo(up, getX(), getY() + 1);
						sec = canMoveTo(down, getX(), getY() - 1);
						if (first == true && sec == true){
							size_t r = rand() % 2;
							if (r == 1)
								setDirection(up);
							else
								setDirection(down);
						}
						else if (first == true)
							setDirection(up);
						else{
							setDirection(down);
						}
					}
					else if (getDirection() == up || getDirection() == down){
						//check left and right
						first = canMoveTo(right, getX() + 1, getY());
						sec = canMoveTo(left, getX() - 1, getY());
						if (first == true && sec == true){
							size_t r = rand() % 2;
							if (r == 1)
								setDirection(right);
							else
								setDirection(down);
						}
						else if (first == true)
							setDirection(right);
						else
							setDirection(left);
					}
					numSquaresToMoveInCurrentDirection = (rand() % (60 - 8)) + 8;
					perpSec = 200; //reset perpsec

				}
			}
		}
		//	//8 + 9
		if (canMoveInDir(getDirection()))
			moveInDir(getDirection());
		else
			numSquaresToMoveInCurrentDirection = 0;
	}
}

void RegProtester::getAnnoyed(unsigned pts){
	if (!leavetheOil)
		hitPoints -= pts;
	
	//99 is just to turn on leave the oil and nothing else
	if (hitPoints <= 0){
		leavetheOil = true;
		ticksToWait = 0;
		if (pts == 99){
			return;
		}
		else {
			//for causing a regular protester to give up u inc score by 100 and 250 for hardcore
			if (returnIdentity() == 1){
				getSW()->increaseScore(100);
			}
			else
				getSW()->increaseScore(250);

			getSW()->playSound(SOUND_PROTESTER_GIVE_UP);
		}
		
		//if annoyed by boulder increase by 500 pts
		//if annoyed by squirt increase by 100
	}
	else
	{
		getSW()->playSound(SOUND_PROTESTER_ANNOYED);
		int N = max(50, 100 - static_cast<int>(getSW()->getLevel() * 10));
		ticksToWait += N;
	}
}

void RegProtester::returnArr(char (&ar)[64][64]){
	for (int e = 0; e < 64; e++){
		for (int f = 0; f < 64; f++){
			ar[e][f] = '.';
		}
	}
	int c = 60;
	int d = 60;
	for (int a = 0; a < 64; a++) {
		for (int b = 0; b < 64; b++){
				if (getSW()->checkforDirt(a, b) ){
					ar[a][b] = 'X';
					}
		}

	}

	vector <Actor*> v;
	getSW()->getActors(4, v);
	for (int o = 0; o < static_cast<int>(v.size()); o++)
	{
		for (int q = 0; q < 4; q++){
			for (int r = 0; r < 4; r++){
				ar[v[o]->getX() + q][v[o]->getY() + r] = 'X';
			}
		}
	}
}

bool RegProtester::explore(queue<Coord>& q, char(&arr)[64][64], int sr, int sc){
	if (arr[sr][sc] == 'p')
		return false;
	if (checkSquare(sr, sc, arr)){
			Coord* coordi = new Coord(sr, sc);
			q.push(*coordi);
			arr[sr][sc] = 'p';
			return true;
	}
	return false;

}

bool RegProtester::checkSquare(int sr, int sc, char (&arr)[64][64]){
	/*cout << sr << " " << sc << endl;*/
	//checking if it is positive and within bounds. then checking the 4x4
	if (sr >= 60 + 1 || sr < 0 || sc >= 60 + 1 || sc < 0)
		return false;
	for (int i = sr; i < sr + 4; i++){
		for (int j = sc; j < sc + 4; j++){
			if (arr[i][j] == 'X'){
				return false;
			}
		}
	}
	return true;
}

bool RegProtester::goBack(){
	if (getX() == 60 && getY() == 60)
		setStatus(false);
	//this gives an array that maps out the thing
	char arr[64][64];
	returnArr(arr);
	map<Coord, Direction> mp;
	queue <Coord> q;
	//start at 60,60
	Coord* start = new Coord(60, 60);
	//processsed
	arr[60][60] = 'p';
	q.push(*start);
	while (!q.empty()){
		Coord curr = q.front();
		q.pop();
		const int cr = curr.getR();
		const int cc = curr.getC();

		if (cr == getX() && cc == getY())
			break;

		//add opposite direction into the map
		if (explore(q, arr, cr - 1, cc)){
			Coord* c = new Coord(cr - 1, cc);
			mp[*c] = right;
		}
		if (explore(q, arr,  cr + 1, cc)){
			Coord* c = new Coord(cr + 1, cc);
			mp[*c] = left;
		}

		if (explore(q, arr, cr, cc + 1)){
			Coord* c = new Coord(cr, cc + 1);
			mp[*c] = down;
		}
		if (explore(q, arr, cr, cc - 1)){
			Coord* c = new Coord(cr, cc - 1);
			mp[*c] = up;
		}
	}
	map<Coord, Direction>::iterator mpit;
	Coord* current = new Coord(getX(), getY());
	mpit = mp.find(*current);
	if (mpit != mp.end()){
		setDirection(mpit->second);
		moveInDir(mpit->second);
		return true;
	}
	return false;
}

//tells you when u are at r,c if you are facing frackman
bool RegProtester::facingFm(){
	if (getDirection() == right){
		if (getSW()->getFm()->getX() > getX())
			return true;
	}
	if (getDirection() == left){
		if (getSW()->getFm()->getX() < getX())
			return true;
	}
	if (getDirection() == up){
		if (getSW()->getFm()->getY() > getY())
			return true;
	}
	if (getDirection() == down){
		if (getSW()->getFm()->getY() < getY())
			return true;
	}
	return false;
}

//gives the direction that the frackman is if in lineofsight. only top line is considered
GraphObject::Direction RegProtester::lineOfSight(){
//check left then right
	if (getDirection() == left || getDirection() == right){
		int i = 1;
		while (getX() - i >= 0){
			
			if (!getSW()->checkforDirt(getX() - i, getY() + 3) && !getSW()->checkforBoulder(getX() - i, getY() + 3))
			{
				
				//2 for loops to check the square of frackman
				for (size_t u = 0; u < 4; u++){
					for (size_t p = 0; p < 4; p++){
						if (getSW()->getFm()->getX() + u == getX() - i && getSW()->getFm()->getY() + p == getY() + 3)
							return left;
					}
				}
			}
			else
				break;
			i++;
		}
	
		i = 1;
		while ((getX() + i) < 61){
			if (!getSW()->checkforDirt(getX() + i, getY() + 3) && !getSW()->checkforBoulder(getX() + i, getY() + 3)){
				for (size_t u = 0; u < 4; u++){
					for (size_t p = 0; p < 4; p++){
						if (getSW()->getFm()->getX() + u == getX() + i && getSW()->getFm()->getY() + p == getY() + 3)
							return right;
					}
				}
			}
			else 
				break;
			i++;
		}
		return none;
	}

	//check up then down. unlike left and right, the heads are in different places now
	if(getDirection() == up){
		int i = 1;
		while ((getY() + i) < 61){
			if (!getSW()->checkforDirt(getX(), getY() + i) && !getSW()->checkforBoulder(getX(), getY() + i)){
				for (size_t u = 0; u < 4; u++){
					for (size_t p = 0; p < 4; p++){
						if (getSW()->getFm()->getX() + u == getX() && getSW()->getFm()->getY() + p == getY() + i)
							return up;
					}
				}
			}
			else 
				break;
			i++;
		}
		return none;
	}
	if (getDirection() == down){
		int i = 1;
		while ((getY() - i) >= 0){
			if (!(getSW()->checkforDirt(getX() + 3, getY() - i)) && !(getSW()->checkforBoulder(getX() + 3, getY() - i))){
				for (size_t u = 0; u < 4; u++){
					for (size_t p = 0; p < 4; p++){
						if (getSW()->getFm()->getX() + u == getX() + 3 && getSW()->getFm()->getY() + p == getY() - i)
							return down;
					}
				}
			}
			else
				break;
			i++;
		}
		return none;
	}
	return none;
}


bool RegProtester::canPerp(Direction currDir){
	if (currDir == right || currDir == left){
		//check up and down
		if (canMoveTo(up, getX(), getY() + 1) || canMoveTo(down, getX(), getY() - 1))
			return true;
	}
	if (currDir == up || currDir == down){
		if (canMoveTo(up, getX() + 1, getY()) || canMoveTo(down, getX() + 1, getY()))
			return true;
	}
	return false;
}


//HardcoreProtester///////////////////////////////////////////////////////////////////////////////

HardProtester::HardProtester(StudentWorld* sw, int startX, int startY, int id, int leftSquares, int tw) : RegProtester(sw, startX, startY, id, leftSquares, tw, 20), orgTicks(tw)
{}

void HardProtester::doSomething()
{
	if (!isAlive())
		return;
	//using this later
	bool skipToNine = false;
	if (getticksToWait() != 0){
		decticksW();
		return;
	}
	else{
		setTicksToWait(orgTicks);
		//waiting period for perpSec. decrement no matter what happens. that's why it starts at 201
		int ps = getperpSec();
		ps--;
		setperpSec(ps);
		if (getperpSec() < 0)
			setperpSec(0);
		//3
		if (getleavetheOil()){
			if (goBack())
				return;
		}//4
		else if (getSW()->distance(getSW()->getFm()->getX(), getSW()->getFm()->getY(), getX(), getY()) <= 4 && facingFm()){
			if (getshoutSec() == 0){
				getSW()->playSound(SOUND_PROTESTER_YELL);
				getSW()->getFm()->getAnnoyed(2);
				resetshoutSec();
			}
			else decshoutSec();
			return;
		}// 5
		else{
			int  M = 16 + getSW()->getLevel() * 2;
			char arr[64][64];
			returnArr(arr);
			map<Coord, int> mp;
			map<Coord, Direction>mp2;
			queue <Coord> q;
			//start at frackman place. trying to find harcore protester
			Coord* start = new Coord(getSW()->getFm()->getX(), getSW()->getFm()->getY());
			//processsed
			arr[getSW()->getFm()->getX()][getSW()->getFm()->getY()] = 'p';
			q.push(*start);

			int distancefromFm = 0;
			mp[*start] = 0;
			while (!q.empty()){
				Coord curr = q.front();
				q.pop();
				const int cr = curr.getR();
				const int cc = curr.getC();
				Coord* c1 = new Coord(cr, cc);
				distancefromFm = mp[*c1];
				distancefromFm++;
				if (cr == getX() && cc == getY())
					break;

				if (explore(q, arr, cr, cc + 1)){
					Coord* c = new Coord(cr, cc + 1);
					mp[*c] = distancefromFm;
					mp2[*c] = down;
				}
				if (explore(q, arr, cr, cc - 1)){
					Coord* c2 = new Coord(cr, cc - 1);
					mp[*c2] = distancefromFm;
					mp2[*c2] = up;
				}

				if (explore(q, arr, cr - 1, cc)){
					Coord* c = new Coord(cr - 1, cc);
					mp[*c] = distancefromFm;
					mp2[*c] = right;
				}
				if (explore(q, arr, cr + 1, cc)){
					Coord* c = new Coord(cr + 1, cc);
					mp[*c] = distancefromFm;
					mp2[*c] = left;
				}
			}
			
			//two iterators for different types of maps
			map<Coord, int>::iterator mpit;
			map<Coord, Direction>::iterator mpit2;
		
			Coord* current = new Coord(getX(), getY());
			mpit = mp.find(*current);
			mpit2 = mp2.find(*current);
			if (mpit != mp.end() && mpit2 != mp2.end()){
				if (mpit->second <= M && getSW()->distance(getSW()->getFm()->getX(), getSW()->getFm()->getY(), getX(), getY()) > 4){
					setDirection(mpit2->second);
					moveInDir(mpit2->second);
					return;
				}
			}

			//6 line of sight stuff

			if (lineOfSight() != none && getSW()->distance(getSW()->getFm()->getX(), getSW()->getFm()->getY(), getX(), getY()) > 4 && canMoveTo(lineOfSight(), getSW()->getFm()->getX(), getSW()->getFm()->getY())){
				Direction ls = lineOfSight(); // returns direction to go
				setDirection(ls); //set direction then take a step
				if (ls == right)
					moveTo(getX() + 1, getY());
				else if (ls == left)
					moveTo(getX() - 1, getY());
				else if (ls == up)
					moveTo(getX(), getY() + 1);
				else if (ls == down)
					moveTo(getX(), getY() - 1);
				if (lineOfSight() != ls) //doesn't see the frackman after moving
					setNumOfLeftSquares(0);
				return;

			}
			else{
				int nsq = getNumOfLeftSquares() - 1;
				setNumOfLeftSquares(nsq);
				if (getNumOfLeftSquares() <= 0){
					unsigned r = rand() % 4;
					size_t c = 0;
					while (c == 0){
						//right
						if (r == 0){
							if (canMoveInDir(right))
							{
								setDirection(right);
								c++;
							}
							else r++;
						}
						if (r == 1){
							if (canMoveInDir(left))
							{
								setDirection(left);
								c++;
							}
							else r++;
						}
						if (r == 2){
							if (canMoveInDir(up))
							{
								setDirection(up);
								c++;
							}
							else r++;
						}
						if (r == 3){
							if (canMoveInDir(down))
							{
								setDirection(down);
								c++;
							}
							else r = 0;
						}
					}
					int ran = (rand() % (60 - 8)) + 8;
					setNumOfLeftSquares(ran);
					skipToNine = true;
				}
			}

			//can go in at least one of the perp. directions
			if (!skipToNine){
				bool first = false;
				bool sec = false;
				if (canPerp(getDirection())){
					if (getperpSec() <= 0){
						if (getDirection() == right || getDirection() == left){
							//check up and down
							first = canMoveTo(up, getX(), getY() + 1);
							sec = canMoveTo(down, getX(), getY() - 1);
							if (first == true && sec == true){
								size_t r = rand() % 2;
								if (r == 1)
									setDirection(up);
								else
									setDirection(down);
							}
							else if (first == true)
								setDirection(up);
							else{
								setDirection(down);
							}
						}
						else if (getDirection() == up || getDirection() == down){
							//check left and right
							first = canMoveTo(right, getX() + 1, getY());
							sec = canMoveTo(left, getX() - 1, getY());
							if (first == true && sec == true){
								size_t r = rand() % 2;
								if (r == 1)
									setDirection(right);
								else
									setDirection(down);
							}
							else if (first == true)
								setDirection(right);
							else
								setDirection(left);
						}
						int ran = (rand() % (60 - 8)) + 8;
						setNumOfLeftSquares(ran);
						setperpSec(200); //reset perpsec
					}
				}
			}
			//9 + 10
			if (canMoveInDir(getDirection()))
				moveInDir(getDirection());
			else
				setNumOfLeftSquares(0);
		}
	}
}





