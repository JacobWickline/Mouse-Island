//Jacob Wickline
//CS 252
//Mouse Island
//Sources and Citations: http://www.cplusplus.com/doc/tutorial/files/

#include <iostream> //for cin, cout
#include <fstream>  //for file i/o
#include <string>   //for string data type
#include <random>   //for randomness
#include <chrono>   //for time
#include <thread>   //for pausing

using namespace std;

int getRandomNumber(int, int);
void loadMap(ifstream&, int[][80], int, int);
void printMap(int[][80], int, int);
string simulate(int[][80], int, int, int &, int &, int &, int &, int[][80]);

int main()
{
    //Create the map array
    int theMap[30][80];
    int originalMap[30][80];
    int heatMap[30][80];

    //Open the file
    cout << "Enter a file name: " << endl;
    string fileName = "";
    cin >> fileName;

    std::ifstream inData;
    inData.open(fileName);

    //Get map attributes from the header of the file
    string mapName = "";
    int numSims = 0;
    int rows = 0;
    int cols = 0;
    int runTime = 0;
    int mouseDrownedCount = 0;
    int mouseEatenCount = 0;
    int mouseStarvedToDeathCount = 0;
    int mouseEscapeCount = 0;

    if(inData.is_open()){
        getline(inData, mapName); //name of map
        inData >> numSims;        //times to run the simulation
        inData >> rows;           //number of rows in map
        inData >> cols;           //number of columns in map

        /*
         When reading with >>, it leaves \n in the buffer.
         If we want to read with getline, we have to clear that out
        */
        string clear = "";
        getline(inData,clear); //read \n into an empty string

        //reads the map into the array and close the file
        loadMap(inData, theMap, rows, cols);
        inData.close();

        //copys theMap to the originalMap
        for(int i = 0; i < rows; i++){
            for(int j = 0; j< cols; j++){
                originalMap[i][j] = theMap[i][j];
            }
        }

        //initialize all values in heatMap to 0
        for(int i = 0; i < rows; i++){
            for(int j = 0; j< cols; j++){
                heatMap[i][j] = 0;
            }
        }

        //this runs the simulation until it equals the amount of times specified in the input file
        while(runTime != numSims){
            for(int i = 0; i < rows; i++){
                for(int j = 0; j< cols; j++){
                    theMap[i][j] = originalMap[i][j];
                }
            }
        string result = simulate(theMap, rows, cols, mouseDrownedCount, mouseEatenCount, mouseEscapeCount, mouseStarvedToDeathCount, heatMap);
        runTime++;
        cout <<"Simulation has ended\n\n";
        }
    }

    else{
        cout << "Invalid Input File!" << endl;
    }

    ofstream myfile;
    myfile.open ("output.txt");
    myfile << mapName << endl
    <<"The simulation was ran " << numSims << " times" << endl
    <<"The mouse has drowned " << mouseDrownedCount << " times." << endl
    << "The mouse was eaten by the cat " << mouseEatenCount << " times." << endl
    << "The mouse escaped the island " << mouseEscapeCount << " times." << endl
    << "The mouse starved to death " << mouseStarvedToDeathCount << " times.\n\n" << endl;

    for(int i = 0; i < rows; i++){
        for(int j = 0; j < cols; j++){
            myfile << heatMap[i][j] << " ";
        }
        myfile << endl;
    }
    myfile.close();

    return 0;
}

/*
getRandomNumber takes a minimum value and a maximum value
it returns a random number in that range.

This is a modification of the function I gave you before
from Bjarne Stroustrup. There was a compiler implementation
bug with the random device part of the previous function that
set the same seed every time, making things not very random.

This one appropriately gives randomness.
*/
int getRandomNumber(int min, int max)
{
    static std::mt19937 ran(time(nullptr));
    std::uniform_int_distribution<int> dist(min,max);
    return dist(ran);
}

/*
loadMap takes an input file object (not a string), a 2D array of integers
and the length and width of the array. The maximum number of columns is 80.

It reads the map from the file and puts it in the 2D array.
*/
void loadMap(std::ifstream& inData, int theMap[][80],int length, int width){
    string line = "";
    int row = 0;
    int col = 0;
    if(inData.is_open()){
        while(getline(inData, line)){
            for(char thing: line){
                theMap[row][col] = thing-'0'; //convert character to integer
                col++;
            }
            row++;
            if(row > length or col > width){
                cout << "ERROR: Array Overflow" << endl;
                exit(1);
            }
            col = 0;
        }
    }
    else{
        cout << "Invalid Input File!" << endl;
    }
    inData.close();
}

/*
printMap takes a 2D array of integers
and the length and width of the array. The maximum number of columns is 80.

It prints the map to the screen.
*/
void printMap(int theMap[][80], int length, int width){
    for(int i = 0; i < length; i++){
       for(int j = 0; j < width; j++){
           if(theMap[i][j] == 1){
               cout <<'w';
           }
           else if(theMap[i][j] == 0){
               cout <<' ';
           }

           else if(theMap[i][j] == 3){
               cout <<"m";
           }

           else if(theMap[i][j] == 4){
               cout <<"c";
           }

           else if(theMap[i][j] == 2){
               cout <<"b";
           }

           else if(theMap[i][j] == 6){
               cout <<"h";
           }

           else if(theMap[i][j] == 5){
               cout <<"f";
           }
       }
       cout << endl;
    }
    cout << endl;
}


/*
simulate takes a 2D array of integers
and the length and width of the array. The maximum number of columns is 80.

It returns a string containing the result of the simulation.

It should contain the logic for one full simulation. A simulation should
end when the mouse runs into water, escapes the island, is eaten by the cat,
or dies of starvation.

Right now, the mouse only moves right and goes directly into the water.

You'll need to modify this to meet the requirements of the project.
*/

string simulate(int theMap[][80], int length, int width, int& mouseDrownCount, int& mouseEatenCount, int& mouseEscapeCount, int& mouseStarvedToDeathCount, int heatMap[][80]){
    int mouseRow = 0;
    int mouseCol = 0;
    int mouseDirection = 0;
    int catRow = 0;
    int catCol = 0;
    int catDirection = 0;
    int mouseStarveCounter = 0;
    bool catLeftIsland = 0;

    //Finding the mouse on the map
    for(int i = 0; i < length; i++){
        for(int j = 0; j < width; j++){
            if(theMap[i][j] == 3){
                mouseRow = i;
                mouseCol = j;
                break;
            }
        }
    }

    heatMap[mouseRow][mouseCol] = heatMap[mouseRow][mouseCol] + 1;
    //Finding the cat on the map
    for(int i = 0; i < length; i++){
        for(int j = 0; j < width; j++){
            if(theMap[i][j] == 4){
                catRow = i;
                catCol = j;
                break;
            }
        }
    }

    //print the initial map and delay
    printMap(theMap, length, width);
    std::this_thread::sleep_for(std::chrono::milliseconds(300));

    //this runs the simulation while the mouse's starve counter is not at 100
    while(mouseStarveCounter != 100){
        //blank out the cat and mouse's current spot
        if(theMap[mouseRow][mouseCol] != 6){
            theMap[mouseRow][mouseCol] = 0;
        }


        theMap[catRow][catCol] = 0;

        //Mouse movement system
        mouseDirection = getRandomNumber(1,4);

        if(mouseDirection == 1){
            mouseCol++;
        }

        else if(mouseDirection == 2){
            mouseCol--;
        }

        else if(mouseDirection == 3){
            mouseRow++;
        }

        else if(mouseDirection == 4){
            mouseRow--;
        }

        //Saving cat's position incase he runs into water
        int oldCatRow = catRow;
        int oldCatCol = catCol;

        //Cat movement system
        if(catLeftIsland == false){
        catDirection = getRandomNumber(1,4);

            if(catDirection == 1){
                catCol++;
            }

            else if(catDirection == 2){
                catCol--;
            }

            else if(catDirection == 3){
                catRow++;
            }

            else if(catDirection == 4){
                catRow--;
            }
        }

        //if the mouse hits the water, cat, or made it to the bridge
        if(theMap[mouseRow][mouseCol] == 1){
            cout <<"The mouse has drowned\n" << endl;
            mouseDrownCount++;
            heatMap[mouseRow][mouseCol] = heatMap[mouseRow][mouseCol] + 1;
            return "drown";
        }

        else if(theMap[mouseRow][mouseCol] == 2){
            cout <<"The mouse has escaped the island!\n" << endl;
            mouseEscapeCount++;
            return "escape";
        }

        else if(mouseCol == catCol && mouseRow == catRow){
            cout <<"The cat has eaten the mouse\n" << endl;
            mouseEatenCount++;
            return "eaten";
        }

        else if(theMap[mouseRow][mouseCol] == 5){
            cout <<"The mouse has eaten the food, his starve counter has been reset." << endl;
            mouseStarveCounter = 0;
        }


        if(theMap[catRow][catCol] == 2 && theMap[mouseRow][mouseCol] == 6){
            cout <<"The cat couldn't find the mouse, he left the island\n" << endl;
            catLeftIsland = true;
        }

        if(theMap[catRow][catCol] == 1 || theMap[catRow][catCol] == 5 || theMap[catRow][catCol] == 6){
            catCol = oldCatCol;
            catRow = oldCatRow;
        }

        //places the cat and mouse in the new position
        if(theMap[mouseRow][mouseCol] == 6){
            cout << "mouse is on the hole" << endl;
        }
        else{
            theMap[mouseRow][mouseCol] = 3;
        }

        heatMap[mouseRow][mouseCol] = heatMap[mouseRow][mouseCol] + 1;
        theMap[catRow][catCol] = 4;

        //print the map and wait for a delay
        printMap(theMap, length, width);
        std::this_thread::sleep_for(std::chrono::milliseconds(300));

        //This adds to the mouse's turn
        mouseStarveCounter++;
    }

    if(mouseStarveCounter == 100){
        cout <<"The mouse has starved to death\n" << endl;
        mouseStarvedToDeathCount++;
        return "starve";
    }
}
