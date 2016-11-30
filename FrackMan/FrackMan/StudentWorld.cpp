#include "StudentWorld.h"
#include "Actor.h"
#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <iomanip>
using namespace std;

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}

StudentWorld::~StudentWorld(){
	cleanUp();
}

int StudentWorld::init()
{
	fm = new Frackman(this);

	//make new dirt objects from rows 0-59 and column 
	for (int o = 0; o < VIEW_WIDTH; o++){
		for (int j = 0; j < 59+1; j++){
			dirtArray[o][j] = new Dirt(this,o,j);
		}
	}

	//deleting dirt for mine shaft
	for (int k = 30; k < 33 + 1; k++){
		for (int l = 4; l < 59+1; l++){
			delete dirtArray[k][l];
			dirtArray[k][l] = nullptr;
		}
	}

	// finding the int boulder,gold,barreLs for the number of things to have

	int B = min(static_cast<int>(getLevel()/2 + 2), 6);  //boulder
	int G = max(static_cast<int>(5 - getLevel() / 2), 2); // gold
	int L = min(static_cast<int>(2 + getLevel()), 20);    //barrel
	int p = 1;											//protester
	// setting the number of Barrels that you have to collect to L
	numberOfBarrels = L;
	ticksSinceLastProtester = 0;
	numberOfProtesters = 1;
	
	//adding the actors: 'b', 'g' , 'l' , 'p' stands for objects
	addActors(B, 'b');
	addActors(L, 'l');
	addActors(G, 'g');
	//adding first protester////////////////////////////////////////
	//chance it will be hardcore protester
	/*int probabilityOfHardcore = min(90, static_cast<int>(getLevel()* 10 + 30));*/
	int probabilityOfHardcore = 100;
	int proProb = rand() % 100 + 1; // numbers go from 100 to 1
	//# of leftsquares and ticks to wait
	int numSquares = (rand() % (60 - 8)) + 8;
	int ticksToWaitBetweenMoves = max(0, 3-static_cast<int>( getLevel() / 4));
	if (proProb > probabilityOfHardcore){
		//make regular
		v.push_back(new RegProtester(this, 60, 60, IID_PROTESTER, numSquares, ticksToWaitBetweenMoves,5));
	}
	else
	{
		//make hardcore
		v.push_back(new HardProtester(this, 60, 60, IID_HARD_CORE_PROTESTER, numSquares, ticksToWaitBetweenMoves));
		
	}

	// deleting the dirt for the boulder
	for (vector<Actor*>::iterator it = v.begin(); it != v.end(); it++){
		//if the vector iterator is at a boulder
		if ((*it)->returnIdentity() == 4){
			for (int p = (*it)->getX(); p < (*it)->getX() +4 && p < 63 + 1 && p >= 0; p++){
				for (int j = (*it)->getY(); j < (*it)->getY() +4 && j < 60 && j >= 0; j++){
					if (checkforDirt(p, j))
						destroyDirt(p, j);
				}
			}
		}
	}
	return 1;
}

int StudentWorld::move()
{
	//set game text
	setDisplayText();

	// tell everyone to doSomething
	fm->doSomething();
	for (vector<Actor*>::iterator it = v.begin(); it != v.end(); it++)
	{
		if ((*it)->isAlive()){
			(*it)->doSomething();
			if (fm->isAlive() == false){
				decLives();
				return GWSTATUS_PLAYER_DIED;
			}
			if (getnumberOfBarrels() == 0)
				return GWSTATUS_FINISHED_LEVEL;
		}
	}

	//add new actors
	//adding protesters///////////////////////////////////////////////////////////
	//first protester must be added in the first tick
	int T = max(25, 200 - static_cast<int>(getLevel()));
	int P = min(15, 2 + static_cast<int>(getLevel()*1.5));
	if (ticksSinceLastProtester == T){
		if (numberOfProtesters < P){
			int probabilityOfHardcore = min(90, static_cast<int>(getLevel() * 10 + 30));
			int proProb = rand() % 100 + 1; // numbers go from 100 to 1
			int numSquares = (rand() % (60 - 8)) + 8;
			int ticksToWaitBetweenMoves = max(0, 3 - static_cast<int>(getLevel() / 4));
			if (proProb <= probabilityOfHardcore){
				//make hardcore
				v.push_back(new HardProtester(this, 60, 60, IID_HARD_CORE_PROTESTER, numSquares, ticksToWaitBetweenMoves));
			}
			else
			{
				//make regular
				v.push_back(new RegProtester(this, 60, 60, IID_PROTESTER, numSquares, ticksToWaitBetweenMoves,5));
			}
			ticksSinceLastProtester = 0;
			numberOfProtesters++;
		}
	}
	else
		ticksSinceLastProtester++;

	///1/G chance to add water pool or sonar kit///////////////////////////////////////////
	unsigned G = getLevel() * 25 + 300;
	unsigned q = rand() % G; // getting a random digit between 0 and 300
	//successful at adding
	if (q == 1){ 
		unsigned r = rand() % 5;
		int tempSec = max(100, static_cast<int>(300 - 10 * getLevel()));
		//1/5 chance it is a sonar
		if (r == 1){
			v.push_back(new Sonar(this, 0, 60,tempSec));
		}
		else{
			//random dirt-less spot to put the water
			unsigned count = 0;
			//two vectors to store the x and the y
			vector<int> xCoord;
			vector<int> yCoord;
			//vector to store the boulder locations
			vector <Actor*> boulder;
			getActors(4, boulder);
			//every sqaure possible 
			bool b = false;
			for (unsigned i = 0; i < 61; i ++){
				for (unsigned k = 0; k < 61; k++){
					//check every square 4x4 at every point
					if (!checkforDirt(i, k)){
						b = true;
						unsigned iu = 0;
						for (unsigned p = 0; p < 4; p++){
							for (unsigned o = 0; o < 4; o++){
								if (checkforDirt(i + p, k + o)){
									iu++;
								}
							}
						}
						if (iu != 0)
							b = false;
					}
					else continue;

			//check for boulder coordinates
					bool dontAdd = false;
					for (unsigned i1 = 0; i1 < boulder.size(); i1++){
						if (boulder[i1]->getX() == i && boulder[i1]->getY() == k)
							dontAdd = true;
							// count for the rand function
					}
			//add only if no dirt and boulder
					if (b && !dontAdd){
						count++;
						//record the open square place
						xCoord.push_back(i);
						yCoord.push_back(k);
					}
				}
			}
			if (count != 0){
				//works since rand starts from 0 1/5 -> 0 1 2 3 4 
				size_t co = 0;
				//add one water if it isn't where fm super close to where frackman is standing
				while (co != 1){
					unsigned c = rand() % count;
					//apparently distance to frackman doesn't matter. so it could appear on the frackman
					/*if (distance(getFm()->getX(), getFm()->getY(), xCoord[c], yCoord[c]) >= 3){*/
						v.push_back(new Water(this, xCoord[c], yCoord[c], tempSec));
						co++;
				/*	}*/
				}
			}
		}
	}
	////////////////////////////////////////////////////////////////////////////////////////////

	//delete dead actors
	for (vector<Actor*>::iterator i = v.begin(); i != v.end();)
	{
		if ((*i)->isAlive() == false)
		{
			delete (*i);
			i = v.erase(i); // erase returns the position after the erased
		}
		else i++;
	}

	//check for frackman alive or dead
	if (fm->isAlive() == false){
		decLives();
		return GWSTATUS_PLAYER_DIED;
	}

	// player has completed level if he collects all the barrels
	if (getnumberOfBarrels() == 0){
		playSound(SOUND_FINISHED_LEVEL);
		return GWSTATUS_FINISHED_LEVEL;
	}

//continue game if u haven't died
	return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp(){
	delete fm;
	for (int i = 0; i < VIEW_WIDTH; i++){
		for (int j = 0; j < 59+1; j++){
			delete dirtArray[i][j];
			dirtArray[i][j] = nullptr;
		}
	}
	for (vector<Actor*>::iterator it = v.begin(); it != v.end();)
	{
		delete (*it);
		it = v.erase(it);
	}
}

void StudentWorld::setDisplayText(){
	unsigned score = getScore();
	unsigned level = getLevel();
	unsigned lives = getLives();
	unsigned health = fm->getHP();
	unsigned squirts = fm->getunitWater();
	unsigned gold = fm->getGold();
	unsigned sonar = fm->getSonar();
	unsigned barrelsLeft = getnumberOfBarrels();

	string s = makeDisplayNeat(score, level, lives, health, squirts, gold, sonar, barrelsLeft);
	setGameStatText(s);
}

string StudentWorld::makeDisplayNeat(unsigned score, unsigned level, unsigned lives, unsigned health, 
	unsigned squirts, unsigned gold, unsigned sonar, unsigned barrelsLeft)
{
	//get 8 digits
	string sc = to_string(score);
	string sc2;
	if (score > 999999)
		sc = "999999";
	for (int n = 0; n < static_cast<int>(6 - sc.length()); n++){
		sc2 += '0';
	}
	sc2+=sc;

	string text = "Scr: " + sc2 + "  " + "Lvl: " + setDigits(level, 2) + "  " + "Lives: " + setDigits(lives, 1)
		+ "  " + "Hlth: " + setDigits((health*10), 3) + "%" + "  " + "Wtr: " + setDigits(squirts, 2) + "  " +
		"Gld: " + setDigits(gold, 2) + "  " + "Sonar: " + setDigits(sonar, 2) + "  " + "Oil Left: " + setDigits(barrelsLeft, 2);
	return text;
}

//gives the numbers leading spaces and the correct length
string StudentWorld::setDigits(int item, int l){
	string sc = to_string(item);
	string sc2;
	for (int n = 0; n < static_cast<int>(l - sc.length()); n++){
		sc2 += ' ';
	}
	sc2 += sc;
	return sc2;
}

//checks for Dirt starting from the leftmost corner
bool StudentWorld::checkforDirt(int lx, int ly){
	if (ly >= 60)
		return false;
	if (dirtArray[lx][ly] != nullptr)
		return true;
	return false;
}

//destroys dirt
void StudentWorld::destroyDirt(int i, int j){
	delete dirtArray[i][j];
	dirtArray[i][j] = nullptr;
}

bool StudentWorld::checkforBoulder(unsigned lx, unsigned ly){
	vector <Actor*> b;
	getActors(4, b);
	//iterate through all the boulders
	for (size_t k = 0; k < b.size(); k++){
		//check all 4pixels
		for (size_t i = 0; i < 4; i++){
			for (size_t j = 0; j < 4; j++){
				if (lx == (*b[k]).getX() + i && ly == (*b[k]).getY()){
					return true;
				}
			}
		}
	}
	return false;
}

//distance formula
double StudentWorld::distance(int x1, int y1, int x2, int y2){
	return sqrt(((x1 - x2)*(x1 - x2)) + (y1 - y2)*(y1 - y2));
}

//returns a vector of the type of actors u want
void StudentWorld::getActors(unsigned id, vector <Actor*> &temp){
	//goes through the vector
	for (vector<Actor*>::iterator it = v.begin(); it != v.end(); it++){
		if ((*it)->returnIdentity() == id){
			temp.push_back(*it);
		}
	}
}

//b = boulder, s = squirt, l = barrel, g = goldnugget
//only passing in sr and sc if there is a starting point for it
void StudentWorld::addActors(int count, char c, int sr, int sc)
{
	//(rand()%(max-min))+min; gives random number between max and min 
	int x = 0;
	int y = 0;
	//add actor
	for (int i = 0; i < count;){
		bool b = true;
		x = (rand() % (60 - 0)) + 0;
		y = (rand() % (56 - 20)) + 20;
		//only check for the barrels and boulders and gold nuggets
		if (c == 'b' || c == 'l' ||c == 'g'){
			//check radius of 6 for nearby objects
			for (unsigned n = 0; n < v.size(); n++){
				if (distance(x, y, v[n]->getX(), v[n]->getY()) <= 6)
					b = false;
			}

			// no actors in the vertical mine shaft
			if (x >= 27 && x <= 33 && y >= 1 && y <= 60)
				b = false;
		}

		//nothing wrong then add boulders and increment
		if (b){
			if (c == 'b'){
				v.push_back(new Boulder(this, x, y));
			}
			else if (c == 'l'){
				v.push_back(new Barrel(this, x, y));
			}
			else if (c == 's'){
				//make a squirt in the frackman direction
				v.push_back(new Squirt(this, sr, sc,getFm()->getDirection()));
			}
			else if (c == 'g'){
				if (sr == 99 && sc == 99){//only these coordinates if frackman made it
					v.push_back(new Gold(this, getFm()->getX(), getFm()->getY(), true, true, false, 100));
				}
				else
					v.push_back(new Gold(this, x, y, false, false, true, 9999));
			}
			i++;
			}
		}
	}
	



