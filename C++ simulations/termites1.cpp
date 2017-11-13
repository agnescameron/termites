//a simulation of termite behaviour as a model for planning in the built environment,
// based on the model proposed by Cortois and Heymans
//Agnes Cameron, 2017

#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <algorithm>
#include <math.h>
#include <vector>
#include <ctime>
#include <ctype.h>
#include <sstream>
#include <fstream>

using namespace std;

    
    ofstream cellfile;
    ofstream termitefile;
    ofstream coverfile;

    //defines a vector type of mixed 
    struct cell{
        int x;
        int y;
        float pheremone;
        double probPick;
        //initialises earth
        int earth = 6;
        int termitePop;
        bool moving = false;
        bool isDest = false;
        bool isDeadEnd = false;
    };
    
    struct termite{
        int x;
        int y;
        bool hasEarth;
    };
    
    struct movement{
        int x;
        int y;
        int numSteps;
    };

//prints the status of the grid (earth)
void printEarth(vector<cell> cellVector, int cycles){
    int citySize = sqrt(cellVector.size());
    
    cout << "earth, cycles is " << cycles << endl;
    for(int i=1; i<citySize; i++){
    cout << "- ";
    }
    cout << endl;
    for(int j = 1; j<citySize; j++){
        for(int i=1; i<citySize; i++){
            cout << "|" << cellVector[i + j*citySize].earth;
        }
        cout << "|" << endl;   
    }
    for(int i=1; i<citySize + 1; i++){
    cout << "- ";
    }
    cout << endl << endl;
}

//prints the status of the grid (earth)
void printThreshold(vector<cell> cellVector, int cycles, float &coverage){
    int citySize = sqrt(cellVector.size());
    int totalBuildings = 0;
    
    cout << endl;
    for(int j = 1; j<citySize; j++){
        for(int i=1; i<citySize; i++){
            if(cellVector[i + j*citySize].isDest ==1)
                cout << "X";
            else if(cellVector[i + j*citySize].isDeadEnd ==1)
                cout << "0";                
            else if(cellVector[i + j*citySize].moving ==1)
                cout << "o";
            else if(cellVector[i + j*citySize].earth>6){
                cout << "█";
                totalBuildings = totalBuildings+1;
            }
            else
                cout << " ";
        }
        cout << endl;
    }

    cout << endl << endl;
    
    for(int i=1; i<citySize; i++)
        cout << "#";
        
    coverage = (float)totalBuildings/cellVector.size();
    cout << "Coverage is " << coverage*100 << " percent" << endl;
}

//prints the status of the grid (termites)
void printTermites(vector<cell> cellVector, int cycles){
        int citySize = sqrt(cellVector.size());
    
    cout << "termites" << endl;
    for(int i=1; i<citySize; i++){
    cout << "- ";
    }
    cout << endl;
    for(int j = 1; j<citySize; j++){
        for(int i=1; i<citySize; i++){
            cout << "|" << cellVector[i + j*citySize].termitePop;
        }
        cout << "|" << endl;   
    }
    for(int i=1; i<citySize + 1; i++){
    cout << "- ";
    }
    cout << endl << endl;
}


//prints the status of the grid (earth)
void findCoverage(vector<cell> cellVector, int cycles, float &coverage){
    int citySize = sqrt(cellVector.size());
    int totalBuildings = 0;
    
    cout << "thresholded, cycles is " << cycles << endl;

    cout << endl;
    for(int j = 1; j<citySize; j++){
        for(int i=1; i<citySize; i++){
            if(cellVector[i + j*citySize].earth>6){
                totalBuildings = totalBuildings+1;
            }
        }
    }

    coverage = (float)totalBuildings/cellVector.size();
    cout << "Coverage is " << coverage*100 << " percent" << endl;
}

//for each cell, calculates the probability that a termite may pick
//up earth in that cell
void probEarthPick(vector<cell> &cellVector){
    float totalPheremone = 0;
    int a = 0;
    do{
        totalPheremone = totalPheremone + cellVector[a].pheremone;
        a = a+1;
    }while(a <= cellVector.size());

    float avPheremone = totalPheremone/(float)cellVector.size(); 
    a = 0;    
        do{
        if(totalPheremone > 0 && (double)cellVector[a].pheremone/avPheremone > 0){
            cellVector[a].probPick = (double)cellVector[a].pheremone/avPheremone;
            //cout << "total pheremone is " << totalPheremone << " probPick is " << cellVector[a].probPick << endl;
        }
        else{
            cellVector[a].probPick = 1;
        }
        
        a = a+1;
        }while(a <= cellVector.size());
}

//measures the average time needed to navigate from one square to another, randomly samples
void walkability(vector<cell> cellVector, int cycles, float coverage){
    double distNorth;
    double distNE;
    double distEast;
    double distSE;
    double distSouth;
    double distSW;
    double distWest;
    double distNW;
    bool chosenDirection;
    int citySize = sqrt(cellVector.size());
    int fails = 0;
    int x = 0;
    int threshold = 7;
    float walkability = 0;
    int distanceTravelled = 0;
    int distMoved = 0;
    bool broken = false;
                

        do{
            int randCell1 = rand() % cellVector.size();
            int randCell2 = rand() % cellVector.size();
            if(cellVector[randCell1].earth < threshold && cellVector[randCell2].earth < threshold && cellVector[randCell1].x < citySize && cellVector[randCell1].y < citySize && cellVector[randCell2].x < citySize && cellVector[randCell2].y < citySize && cellVector[randCell1].x > 0 && cellVector[randCell1].y > 0 && cellVector[randCell2].x > 0 && cellVector[randCell2].y > 0){
                bool destinationFound = false;
                int posi = cellVector[randCell1].x;
                int posj = cellVector[randCell1].y;
                int desti = cellVector[randCell2].x;
                int destj = cellVector[randCell2].y;
                int numSteps = 0;
                bool samePlace = false;
                
                cellVector[randCell1].isDest = true;
                cellVector[randCell2].isDest = true;
                
                vector <movement> moveVector;
                cout << " gets here " << moveVector.size() << endl;
                
                do{
                   cout << "pos is " << posi << " " << posj << " dest is " << desti << " " << destj << endl;
                    chosenDirection = false;
                    
                    vector <float> distVector;
                    distNorth = sqrt(((float)(cellVector[(posj+1)*citySize + posi].x - desti)*(cellVector[(posj+1)*citySize + posi].x - desti)) + (float)(cellVector[(posj+1)*citySize + posi].y - destj)*(cellVector[(posj+1)*citySize + posi].y - destj));
                    distVector.push_back(distNorth);
                    
                    distNE = sqrt((float)(cellVector[(posj+1)*citySize + posi+1].x - desti)*(cellVector[(posj+1)*citySize + posi+1].x - desti) + (float)(cellVector[(posj+1)*citySize + posi+1].y - destj)*(cellVector[(posj+1)*citySize + posi+1].y - destj));
                    distVector.push_back(distNE);
                    
                    distEast = sqrt((float)(cellVector[posj*citySize + posi+1].x - desti)*(cellVector[posj*citySize + posi+1].x - desti) + (float)(cellVector[posj*citySize + posi+1].y - destj)*(cellVector[posj*citySize + posi+1].y - destj));
                    distVector.push_back(distEast);
                    
                    distSE = sqrt((float)(cellVector[(posj-1)*citySize + posi+1].x - desti)*(cellVector[(posj-1)*citySize + posi+1].x - desti) + (float)(cellVector[(posj-1)*citySize + posi+1].y - destj)*(cellVector[(posj-1)*citySize + posi+1].y - destj));
                    distVector.push_back(distSE);
                    
                    distSouth = sqrt((float)(cellVector[(posj-1)*citySize + posi].x - desti)*(cellVector[(posj-1)*citySize + posi].x - desti) + (float)(cellVector[(posj-1)*citySize + posi].y - destj)*(cellVector[(posj-1)*citySize + posi].y - destj));
                    distVector.push_back(distSouth);
                    
                    distSW =sqrt((float)(cellVector[(posj-1)*citySize + posi-1].x - desti)*(cellVector[(posj-1)*citySize + posi-1].x - desti) +  (float)(cellVector[(posj-1)*citySize + posi-1].y - destj)*(cellVector[(posj-1)*citySize + posi-1].y - destj));
                    distVector.push_back(distSW);
                    
                    distWest = sqrt((float)(cellVector[posj*citySize + posi-1].x - desti)*(cellVector[posj*citySize + posi-1].x - desti) + (float)(cellVector[posj*citySize + posi-1].y - destj)*(cellVector[posj*citySize + posi-1].y - destj));
                    distVector.push_back(distWest);
                    
                    distNW = sqrt((float)(cellVector[(posj+1)*citySize + posi-1].x - desti)*(cellVector[(posj+1)*citySize + posi-1].x - desti) + (float)(cellVector[(posj+1)*citySize + posi-1].y - destj)*(cellVector[(posj+1)*citySize + posi-1].y - destj));
                    distVector.push_back(distNW);
                    
                                        
                    cellVector[posj*citySize + posi].moving = true;
                    printThreshold(cellVector, cycles, coverage);
                    system("sleep 0.1");
                    cellVector[posj*citySize + posi].moving = false;
                    
                    do{
                                            
                    int minEl = (min_element(distVector.begin(), distVector.end()) - distVector.begin());
                    //cout << "test vector index " << minEl << endl;
                    
                        
                        //cout << "gets here" << endl;
                        if(minEl == 0){
                             if(cellVector[(posj+1)*citySize + posi].earth < threshold && posj+1 <= citySize){
                                 posj = posj+1;
                                 chosenDirection = true;
                             }
                             else distVector.erase(distVector.begin() + minEl);
                        }
                        else if(minEl == 1){
                             if(cellVector[(posj+1)*citySize + posi+1].earth < threshold && posi+1 <= citySize && posj + 1 <= citySize && cellVector[(posj+1)*citySize + posi+1].isDeadEnd == false){
                                 posi = posi+1;
                                 posj = posj+1;
                                 movement m {posi, posj, numSteps};
                                 moveVector.push_back(m);
                                 chosenDirection = true;
                             }
                             else distVector.erase(distVector.begin() + minEl);
                        }
                        else if(minEl == 2){
                             if(cellVector[(posj)*citySize + posi+1].earth < threshold && posi+1<=citySize && cellVector[(posj)*citySize + posi+1].isDeadEnd == false){
                                 posi = posi+1;
                                 movement m {posi, posj, numSteps};
                                 moveVector.push_back(m);                                 
                                 chosenDirection = true;
                             }
                             else distVector.erase(distVector.begin() + minEl);
                        }
                        else if(minEl == 3){
                             if(cellVector[(posj-1)*citySize + posi+1].earth < threshold && posi+1<=citySize && posj-1>=0 && cellVector[(posj-1)*citySize + posi+1].isDeadEnd == false){
                                 posi = posi+1;                                 
                                 posj = posj-1;
                                 movement m {posi, posj, numSteps};
                                 moveVector.push_back(m);                                 
                                 chosenDirection = true;
                             }
                             else distVector.erase(distVector.begin() + minEl);
                        }
                        else if(minEl == 4){
                             if(cellVector[(posj-1)*citySize + posi].earth < threshold && posj-1>=0 && cellVector[(posj-1)*citySize + posi].isDeadEnd == false){
                                 posj = posj-1;
                                 movement m {posi, posj, numSteps};
                                 moveVector.push_back(m);                                 
                                 chosenDirection = true;
                             }
                             else distVector.erase(distVector.begin() + minEl);
                        }
                        else if(minEl == 5){
                             if(cellVector[(posj-1)*citySize + posi-1].earth < threshold && posj-1>=0 && posi-1>=0  && cellVector[(posj-1)*citySize + posi].isDeadEnd == false){
                                 posi = posi-1;
                                 posj = posj-1;
                                 movement m {posi, posj, numSteps};
                                 moveVector.push_back(m);                                 
                                 chosenDirection = true;
                             }
                             else distVector.erase(distVector.begin() + minEl);
                        }
                        else if(minEl == 6){
                             if(cellVector[(posj)*citySize + posi-1].earth < threshold && posi-1>=0 && cellVector[(posj)*citySize + posi-1].isDeadEnd == false){
                                 posi = posi-1;
                                 movement m {posi, posj, numSteps};
                                 moveVector.push_back(m);                                 
                                 chosenDirection = true;
                             }
                             else distVector.erase(distVector.begin() + minEl);
                        }                        
                        else if(minEl == 7){
                             if(cellVector[(posj+1)*citySize + posi-1].earth < threshold && posi-1>=0 && posj+1<=citySize && cellVector[(posj+1)*citySize + posi-1].isDeadEnd == false){
                                 posi = posi-1;
                                 posj = posj+1;
                                 movement m {posi, posj, numSteps};
                                 moveVector.push_back(m);                       
                                 chosenDirection = true;
                             }
                             else distVector.erase(distVector.begin() + minEl);
                        }                   
                    
                    // for(int i = 0; i<distVector.size(); i++){
                    //      cout << "distvector "<< i << " is" << distVector[i] << " size is " << distVector.size() << " numsteps is " << numSteps << endl;
                    // }
                    
                    if (distVector.size() < 2){
                        cout << "dead end" << endl;
                            numSteps = citySize*2;
                            fails = fails+1;
                            broken = true;
                            break;
                    }
                    
                
                    if(numSteps > 1 && ((moveVector[numSteps].x == moveVector[numSteps-1].x && moveVector[numSteps].y == moveVector[numSteps-1].y) || (moveVector[numSteps].x == moveVector[numSteps-2].x && moveVector[numSteps].y == moveVector[numSteps-2].y)|| (moveVector[numSteps].x == moveVector[numSteps-3].x && moveVector[numSteps].y == moveVector[numSteps-3].y))){
                            //cout << "same place " << endl;
                            numSteps = citySize*2;
                            samePlace = true;
                            break;
                     }
                        
                    }while(chosenDirection == false);
                    
                    numSteps = numSteps + 1;
                    
                    if (posi == citySize || posj == citySize || posi == 0 || posj == 0){
                        //cout << "broken" << endl;
                        numSteps = citySize*2;
                        broken = true;
                        break;  
                    }

                    
                    if(posi == desti && posj == destj)
                        destinationFound = true;
                    
                }while(destinationFound == false && numSteps < citySize*2);
   
                
                cellVector[randCell1].isDest = false;
                cellVector[randCell2].isDest = false;
                
                if(broken == false)
                distMoved = moveVector.size();
                
                else if(samePlace == true)
                break;
                
                else
                distMoved = citySize;
                
                distanceTravelled = distanceTravelled + distMoved;
                walkability = (float)distanceTravelled/(float)(x*citySize);
                
                cout << "walkability is: " << walkability << ", percentage fails: " << (float)fails/(float)x << endl;
                
                broken = false;
                
                //increments the number of cell-to-cell distances tested
                x = x+1;  
                
            }
                
        }while(x < citySize);


}

//calculates termite motion
void moveTermites(vector<cell> cellVector, vector<termite> &termiteVector){
    int a = 0;
    int i = 0;
    int maxpop = 10;
    bool migrated = false;
    int citySize = sqrt(cellVector.size());

    do{
        int termitePosi = termiteVector[a].x;
        int termitePosj = termiteVector[a].y;

        int cellVectorPos = termitePosj*sqrt(cellVector.size()) + termitePosi;
        //useful ? 
        //cellVector[cellVectorPos].termitePop = cellVector[cellVectorPos].termitePop + 1;
        do{
        int randDirection = rand() %8;
            
            //northwest
            if (randDirection == 0 && cellVector[(termitePosj+1)*sqrt(cellVector.size()) + termitePosi-1].termitePop < maxpop && 0 < termitePosj+1 && termitePosj+1 < citySize){
                if(termiteVector[a].hasEarth == true){
                    termiteVector[a].x = termitePosi-1;
                    termiteVector[a].y = termitePosj+1;
                }
                else{
                    termiteVector[a].x = termitePosi+1;
                    termiteVector[a].y = termitePosj-1;
                }
                migrated = true;
            }
            
            //north
            if (randDirection == 1 && cellVector[(termitePosj+1)*sqrt(cellVector.size()) + termitePosi].termitePop < maxpop && 0 < termitePosj+1 && termitePosj+1 < citySize){
                if(termiteVector[a].hasEarth == true){
                    termiteVector[a].x = termitePosi;
                    termiteVector[a].y = termitePosj+1;
                }
                else{
                    termiteVector[a].x = termitePosi;
                    termiteVector[a].y = termitePosj-1;
                }
                migrated = true;
            }
            
            
            //northeast
            else if (randDirection == 2 && cellVector[(termitePosj+1)*sqrt(cellVector.size()) + termitePosi+1].termitePop < maxpop && termitePosj+1 < citySize && termitePosi+1 < citySize){
                if(termiteVector[a].hasEarth == true){
                    termiteVector[a].x = termitePosi+1;
                    termiteVector[a].y = termitePosj+1;
                }
                else{
                    termiteVector[a].x = termitePosi-1;
                    termiteVector[a].y = termitePosj-1;
                }
                migrated = true;
            }
            
            //east
            else if (randDirection == 3 && cellVector[(termitePosj)*sqrt(cellVector.size()) + termitePosi+1].termitePop < maxpop && 0 < termitePosi + 1 && termitePosi + 1 < citySize){
                if(termiteVector[a].hasEarth == true){
                    termiteVector[a].x = termitePosi+1;
                    termiteVector[a].y = termitePosj;
                }
                else{
                    termiteVector[a].x = termitePosi-1;
                    termiteVector[a].y = termitePosj;
                }
                migrated = true;
            }
            
            //southeast
            else if (randDirection == 4 && cellVector[(termitePosj-1)*sqrt(cellVector.size()) + termitePosi+1].termitePop < maxpop && 0 < termitePosj-1 && termitePosj+1 < citySize){
                if(termiteVector[a].hasEarth == true){
                    termiteVector[a].x = termitePosi+1;
                    termiteVector[a].y = termitePosj-1;
                }
                else{
                    termiteVector[a].x = termitePosi-1;
                    termiteVector[a].y = termitePosj+1;
                }
                migrated = true;
            }
            
            
            //south
            else if (randDirection == 5 && cellVector[(termitePosj-1)*sqrt(cellVector.size()) + termitePosi].termitePop < maxpop && 0 < termitePosj-1 && termitePosj-1 < citySize){
                if(termiteVector[a].hasEarth == true){
                    termiteVector[a].x = termitePosi;
                    termiteVector[a].y = termitePosj-1;
                }
                else{
                    termiteVector[a].x = termitePosi;
                    termiteVector[a].y = termitePosj+1;
                }
                migrated = true;
            }
            
            //southwest
            else if (randDirection == 6 && cellVector[(termitePosj-1)*sqrt(cellVector.size()) + termitePosi-1].termitePop < maxpop && 0 < termitePosi - 1 && 0 < termitePosj - 1){
                if(termiteVector[a].hasEarth == true){
                    termiteVector[a].x = termitePosi-1;
                    termiteVector[a].y = termitePosj-1;
                }
                else{
                    termiteVector[a].x = termitePosi+1;
                    termiteVector[a].y = termitePosj+1;
                }
                migrated = true;
            }
            
            //west
            else if(randDirection == 7 && cellVector[(termitePosj)*sqrt(cellVector.size()) + termitePosi-1].termitePop < maxpop &&  0 < termitePosi - 1 && termitePosi - 1 < citySize){
                if(termiteVector[a].hasEarth == true){
                    termiteVector[a].x = termitePosi-1;
                    termiteVector[a].y = termitePosj;
                }
                else{
                    termiteVector[a].x = termitePosi+1;
                    termiteVector[a].y = termitePosj;
                }
                migrated = true;
            }
        a = a+1;
        }while(migrated == false && a<8);
        
        //cout << "prob pick " << probPick << " prob place " << probPlace << endl;
        a = a+1;
        
        
    }while(a < termiteVector.size());
}

void evapPheremone(vector<cell> &cellVector, float evapRate){
    int a = 0;
    do{
        cellVector[a].pheremone = cellVector[a].pheremone*evapRate;
        a = a+1;
    }while(a <= cellVector.size());
}

void calculateTermiteStatus(vector<cell> &cellVector, vector<termite> &termiteVector, int maxheight){
    
    probEarthPick(cellVector);
    int a = 0;
    int i = 0;
    
    do{
        cellVector[i].termitePop = 0;
        i = i + 1;
    }while(i < cellVector.size());
    
    do{
        int termitePosi = termiteVector[a].x;
        int termitePosj = termiteVector[a].y;

        int cellVectorPos = termitePosj*sqrt(cellVector.size()) + termitePosi;
        cellVector[cellVectorPos].termitePop = cellVector[cellVectorPos].termitePop + 1;
        float normalProb = cellVector[cellVectorPos].probPick + 1/cellVector[cellVectorPos].probPick;
        double probPick = cellVector[cellVectorPos].probPick/normalProb;
        double probPlace = (1/cellVector[cellVectorPos].probPick)/normalProb;
        
        //cout << "prob pick " << probPick << " prob place " << probPlace << endl;
        a = a+1;
        
        if(termiteVector[a].hasEarth == true){
            if ((rand() % 100) > probPlace*100 && cellVector[cellVectorPos].earth < maxheight){
                cellVector[cellVectorPos].earth = cellVector[cellVectorPos].earth + 1;
                cellVector[cellVectorPos].pheremone = cellVector[cellVectorPos].pheremone + 1;
                termiteVector[a].hasEarth = false;
            }
        }
        else{
            if ((rand() % 100) > probPick*100 && cellVector[cellVectorPos].earth > 0){
            termiteVector[a].hasEarth = true;
            cellVector[cellVectorPos].earth = cellVector[cellVectorPos].earth - 1;
            }
        }
    }while(a < termiteVector.size());

}

void initialiseTermites(vector<cell> cellVector, vector<termite> &termiteVector){
    int a = 0;
    
    do{
       int chosenCell = rand()%cellVector.size();
       if(cellVector[chosenCell].termitePop <= 10){
        termiteVector[a].x = cellVector[chosenCell].x;
        termiteVector[a].y = cellVector[chosenCell].y;
        
        a = a+1;   
       }
       
       int hasEarth = rand()%1;
       if(hasEarth == 1)
        termiteVector[a].hasEarth = true;
        
    }while(a < termiteVector.size());
}



int main(){
    int citySize = 0;
    int population = 0;
    int numCycles = 0;
    int maxHeight = 0;
    int cycles = 0;
    float evapRate = 1.0;
    float coverage = 0;
    char continu = 'y';
    char walk = 'n';
    bool sizeChosen = false;
    bool popChosen = false;
    bool cyclesChosen = false;
    bool maxHeightChosen = false;
    bool evapRateChosen = false;
    bool continuChosen = false;
    bool walkabilityChosen = false;
    
    srand (time(NULL));
    //choose size of city
    do{
        cout << "Choose the size of the 'city' square (40 gives a 40x40 grid, and so on): ";
        cin >> citySize;
        
        if (cin.fail())
        {
            cout << "incorrect input, please enter a numerical value" << endl;
            
            // get rid of failure state
            cin.clear();
            // discard input
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        
        else{
            sizeChosen = true;
            //cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }while(sizeChosen == false);
    
    //choose size of population
    do{
        cout << "Choose the size of the termite population (eg. 100) ";
        cin >> population;
        
        if (cin.fail())
        {
            cout << "incorrect input, please enter a numerical value " << endl;
            
            // get rid of failure state
            cin.clear();
            // discard input
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        
        else{
            popChosen = true;
            //cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }while(popChosen == false);
    
    //choose max height of earth on one square
    do{
        cout << "Choose the max height of 'earth' on any cell (eg. 10) ";
        cin >> maxHeight;
        
        if (cin.fail())
        {
            cout << "incorrect input, please enter a numerical value ";
            
            // get rid of failure state
            cin.clear();
            // discard input
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        
        else{
            maxHeightChosen = true;
            //cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }while(maxHeightChosen == false);
    
    //choose the pheremone evaporation rate
    do{
        cout << "Choose the pheromone evaoration rate (between 0 and 1) ";
        cin >> evapRate;
        
        if (cin.fail() || evapRate > 1 || evapRate < 0)
        {
            cout << "incorrect input, please enter a numerical value " << endl;
            
            // get rid of failure state
            cin.clear();
            // discard input
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        
        else{
            evapRateChosen = true;
            //cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }while(evapRateChosen == false);
    
    vector<cell> cellVector(citySize*citySize);
    vector<termite> termiteVector(population);
    
    
    //assign each vector element to a cell
    for(int j = 0; j<citySize; j++){
        for(int i = 0; i<citySize; i++){
            cellVector[i + j*citySize].x = i;
            cellVector[i + j*citySize].y = j;
        }
    }
    
    initialiseTermites(cellVector, termiteVector);
    
    //choose number of steps for simulation
    do{
        cout << "How many cycles for the simulation ";
    		  cin >> numCycles;
    		  
        if (cin.fail())
        {
            cout << "incorrect input, please enter a numerical value" << endl;
            
            // get rid of failure state
            cin.clear();
            // discard input
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        
        else{
            cyclesChosen = true;
            //cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }while(cyclesChosen == false);
    
        vector<float> coverVector(numCycles);
        //start the simulation
        do{
            
            //calculate the status of each termite
            calculateTermiteStatus(cellVector, termiteVector, maxHeight);
            //moves termites
            moveTermites(cellVector, termiteVector);
            //evaporates pheremone
            evapPheremone(cellVector, evapRate);

            //print the grid status
            //printEarth(cellVector, cycles);
            
            //prints thresholded street plan
            printThreshold(cellVector, cycles, coverage);
            system("sleep 0.2");
            
            findCoverage(cellVector, cycles, coverage);
            //printTermites(cellVector, cycles);
            //system("sleep 0.5");
            
            //cout << "before, coverage is " << coverage <<endl;
            coverVector[cycles] = coverage;
            //cout << "after" << endl;
            
            cycles = cycles+1;
        }while(cycles < numCycles);


    
        cellfile.open("../sample_data/cellfile_newdata.csv");
        //write cell values to csv file
        for(int j = 0; j<citySize*citySize; j++){
                cellfile << cellVector[j].x << "," << cellVector[j].y << "," << cellVector[j].earth;
            cellfile << "\n";
        }
        cellfile.close();
        
        coverfile.open("../sample_data/coverfile_newdata.csv");
        for(int k = 0; k<numCycles; k++){
                coverfile << coverVector[k] << "\n";
        }
        coverfile.close();

        
        do
            {
                 //decides whether to continue current simulation
                cout << "Calculate Walkability? (y/n): ";
	            cin >> walk;
	            
	            if(walk == 'y' || walk == 'n'){
		        cin.ignore(numeric_limits<streamsize>::max(), '\n');
		        walkabilityChosen = true;
		    }
	            else{
		        cout << "incorrect value, please enter y or n " << endl;
		        walkabilityChosen = false;
		        //cin.ignore(numeric_limits<streamsize>::max(), '\n');
		    }
            }while(walkabilityChosen == false);        
        
        if(walk == 'y'){
            walkability(cellVector, cycles, coverage);
        }
        
        
        do
            {
                 //decides whether to continue current simulation
                cout << "Continue simulation? (y/n): ";
	            cin >> continu;
	            
	            if(continu == 'y' || continu == 'n'){
		        continuChosen = true;
		        cycles = 0;
		        cin.ignore(numeric_limits<streamsize>::max(), '\n');
		    }
	            else{
		        cout << "incorrect value, please enter y or n " << endl;
		        continuChosen = false;
		        //cin.ignore(numeric_limits<streamsize>::max(), '\n');
		    }
            }while(continuChosen == false);        
        
    
}