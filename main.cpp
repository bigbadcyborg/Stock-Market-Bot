/*
    Author: Russell Sullivan
    Description: This program will open various files and read in data in order to communicate Then it will make a decision and output a "0", "1", or "2" so that getData.py may execute

    1-25-2022: added getPosition and getPositionCost functions.
    1-31-2022: fixed stod error. Now there is a new bug. The name needs to be communcated between the programs better.
    2-04-2022: fixed the name communcation. Now there is a bug when switching from BTCUSD to ETHUSD while the program is
        running. Something is sending "1 1.805D" to status.txt
            -Note: i believe the dramatic change of prices is triggering a buy/sell which is the cause of this bug.
            -todo(DONE): main needs to get the memo when a new stock is being checked out so it can clear its existing data.
                    The solution to this is to get the stock name from price.txt. if a new stock name is found then
                        reset all data.
            -note: program now detects name changes and creates a new Stock object when a name change is detected
            -todo(DONE): stod bug
    2-07-2022: fixed stod error. Buying and selling decision making in getData.py is semi functional - Still need to test
                buying/selling with actual money. In main.cpp, stock name change detection is semi functional
                bug:*** after switcing from BTCUSD to ETHUSD to BTCUSD the program loses track of BTCUSD position...?!
    2-08-2022: added trim function. Fixed all stod errors.
                -todo(DONE): test buying and selling triggers(done)
                -todo(DONE): test buying and selling thru webull api(done)
                -todo(DONE): market orders cannot be placed on webull, only limit orders. Main must now send a limit number to status.txt when buying/selling
    2-09-2022    buying and selling crypto thru getData fully functional. It now takes in a third variable: lmt price
                    todo(DONE): buy and sell function in main must now send a the lmt price to status.txt
                note: buying and selling fully functional.
                todo:(DONE) redo the initialization of the programs. getData.py needs to be ran first. It should prompt the user
                        for the initial stock name and write it to fStatus. Main.cpp should run instantly at startup.
                todo(done): fix the position bug. sometimes a position cant be found. a temporary fix can be to make it sleep 50 seconds and send then goto the next iteration
                        -the real issue is the numberous calls to the fPosition stream (using getPosition and getPositionCost) while getData.py is also sending data.
                             A refresh function in the Stock class is needed!!!
                            class is needed (fixed)
                todo(DONE): take out bear and bull functionalities. the progroam needs to be on watch for buying and selling at every iteration

    3-4-2022     note: cannot use program on a stock/crypto that the user does not currently own any portion of.
                TODO: ask user at the start of the program wether or not they currently own any of said stock/crypto. And if they would like to trade stock or crypto.
                        -if user does not own anything then ignore the "no position" error. !!!
    5-8-2022        todo: upon startup - get all user input from iPortfolio.exe ( name symbol, crypto?, buy limit, % to spend, sell limit, % to spend, initial mode )
                    - need to figure out why buy/sell quantities are way off...
    5-15-2022
				bug(fixed): internet disconnection over night.
				todo(fixed): handle the exception. try the whole loop and use default catch. if error is caught, sleep for a minute and try again.
				TODO: clear bin after 150 objects have been added to it. this will make the program faster overtime (bugged)
    5-24-2022
			    DONE: main now clears bin every 150 loop iterations.
                DONE: the program now sets new buy/sell limits after buying and selling. Each time the program buys: it decreases the new buy limit by 1%.
                        Each time the program sells: it increases the sell limit by 1%.
                      the program will also spend 50% of money if the spread becomes 1.95% or less
 
    5-25-2022
                bug: after clearing the bin, reading from the bin is throwing an error
                    -fixed by simply clearing the eof flag of bin before using seekg()
                TODO(done): take out bull/bear mode. main should be checking for buy and sell flags simultaneously each iteration.
                        -added cooldowns to buying and selling( 5 * RUN_TIME ) or 5 minutes.
                TODO(done): Make the program keep record of 1-span-streaks. (this will tell us how long the stock has been going down for)
                        -NOTE: since we took out bear/bull modes i made it to where main only checks a stock's asks for spans and 1-span-streaks.
                            -all buys are made if and only if a the 1-span-streak variable reaches 3 or above. this means the stock has been
                                decreasing for 3 minutes or iterations in a row.
 
    5-30-2022
                TODO: implement MACD momentum meter
                TODO: add break-out trigger and trailing stop loss
                        when a breakout occurs(rsi and MACD extremely high) and we have twice as much money settled than money in the market; then we spend 50%
                            and have a trailing stop-loss (THIS IS HIGH RISK!)
    6-14-2022
                Implemented Timeout Tolerance. Main counts the seconds that it doesnt have the baton and sends -1 to status.txt when a timeout has occurred.
                    getData.py should now see the -1 and logout and restart its process. Main waits in 15 second intervals for this to be completed.
    6-19-2022   BUG(fixed): After a timeout has occured and upon main successfully restarting getData.py... main isnt printing the data to terminal.
    6-20-2022   Timeout Tolerance is now fully functional. Assuming buy/sell still works after restarting. Replaced bull/bear mode boolean functionalities(main.cpp and gui.cpp) with crypto/stock market modes. TODO(done): getData.py needs to know whether to work in crypto or stock market.
    7-17-2022   Implemented stock buy/sell functionalities. main and python both recognize whether in stock or crypto mode. 
                TODO(done): Test stock buying/selling.
                        -bug(fixed): main only iterating once when in stock mode
							~main was attempting to calculate profit from our position which was not found.
    8-31-2022.  Added breakout boolean to deactive upper sell limit when a breakout is detected.
				-Fixed stop loss bug. It wasnt handing off baton before aborting because batonSet() is at the end of the loop.
				-debugged crypto stop loss, crypto/stock buy/sell functions
				TODO: implement upper bound sell limit and upper bound buy limit ( our current limits are lower bound ).
    10-14-2022
               -Breakouts are occurring after stop loss triggers but the stop loss is aborting the program. Took out abort() from stop loss/take profit so further volatile moves can be detected.
               -Removed many unneccessary sleep calls wasting time so now the programs execution time has been reduced by several seconds. TODO(done): get program run time per iteration.
               -main runs in less than a second! (without span algorithm)
               -Tested buying/selling on LINUX. All works good!
               -Calculates prints time of baton hand off if it ever lags > 30 seconds (rare)
    10-15-2022
                -TODO:Instead of main sleeping for 30 seconds, change it to allow the user to properly exit from the programs by pressing enter.
    10-20-2022
                -main now detects whether Stock object is crypto or not by searching thru list of crypto symbols.
                   currently accepted cryto= BTCUSD, ETHUSD
                fixed a bug with finding the position of a stock. iHedge now supports both stock/crypto 
                (NEED TO TEST BUY/SELL)
    11-17-2022
              condensed many functions, data, and streams into the Portfolio class. Still much more to objectify
              (TODO)getAsk and getBid need to be modified to search price.txt for the ask/bid depending on the stockName.
    6-27-2023
              Program has several bugs. 
    The status file seems to be clear upon each execution of main(FIXED). Something is truncating all of status.txt each iteration. Fixed by moving the status file clear from out of a nested loop in the main loop of getData.py. Then typed in the initial move in status.txt and the program seems to be running continously off of that. Changed over to BUD stock. status.txt reset itself. Could the file streams in getData.py and main be interfering with eachother? Main initially waits 10 seconds upon startup before looking into status.txt. iHedge opens getData with python3 and then should be setting status.txt. Status.txt seems to be being truncated on its second iteration. I changed fStatus in main from ofstream to fstream. This seems to have fixed the bug. (FIXED)
        Much more testing is needed.
        Ask/bid spread percentage is bugged.
        Testing buy/sell and continueity of the program after the action is performed. status.txt works?
        Testing Crypto/stock decision making
        Testing stock/crypto LIST/DEQUE decision making
        How does the program count the size of the stock

    7/4/2023
            gui.cpp now appends up to 10 stocks' user configurations in e.txt
            todo: modify main to execute and display each stock from e.txt.
                  modify getData.py to read n number of stock symbol data and then append n data segments to files
            -getData now reads n number of stocks from e.txt and output each segment of data to price.txt 
            todo: modify main to parse through n data segments in price.txt

    7/9/2023
            Bugs to fix: ask/bid spread is only being printed once even when there are multiple stocks
                         main isnt printing the current position (@ x.xx)
            the stock class constructor is calculating the stop loss and take profit but it doesnt know 
                    the ask/bid yet. So, i can make a function that loops through each stock and calculates
                    stop loss and take profits after bid and ask is known. (Fixed)
            bug(fixed): running multiple stocks throws an stod object error

            final note: multiple stock bugs are fixed. now, the printing of the data is really unorganized. the             ordering of the priting of data needs to be fixed.

    7/15/2023
            iHedge v0.8.1 saved.
                issues:
                    - all percentages seem to be way off when using multiple stocks (fixed)

    7/23/2023
            Todo(done): removed all prints to the screen in main and portfolio/stock class functions and paste those to:
                    Stock::print()
            -fixed cooldowns for each stock (10 iterations after buying or selling)
            -when selling, the stocks sell limit is increased by 3%
            -when a buy occurs, the stocks buy limit is then decreased by 5%
            -tested stop loss
            -tested buy/sell
            -todo: test when 2 or more actions are required in a single iteration of main/getData.py

    7/30/2023
            fixed problem with getData.py freezing while loading stock data. This problem ocurred
            due to main opening price.txt during the initialization of the Portfolio class to get the initial prices.
            This problem was fixed by adding the baton alogrithm to the beginning of main.
            TODO: test all buying and selling functionalities while the market is live.
            
				
*/

#include <cstring>
#include <cmath>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <unistd.h>
#include <string>
#include <cstdlib>
#include <vector>
#include <cstdio>
#include <algorithm>
#include <ctime>
#include <stack>
#include <vector>

using namespace std;

//PROTOTYPES
double calcGain(double,double);
ostream & printTime( ostream & out );
string getName( fstream & );
string trim(const string &s);
string ltrim(const string &s);
string rtrim(const string &s);
bool isWhole( float f );
int fileSize( fstream & );
void restart();
void killall();

struct Stock{
    double initBid, initAsk, curBid, curAsk, mid, position,
       positionCost, resistance, support, profit, stopLossPercent,
       takeProfitPercent, stopLossLimit, takeProfitLimit, sellLimit,
       sellLimitPercent, buyLimit, buyLimitPercent, stopLoss, profitTake;
    string stockName, buyLimitStr, buyLimitPercentStr, sellLimitStr,
       sellLimitPercentStr, profitTakeStr, stopLossStr;
    int span, index, qty, buyCooldown, sellCooldown;
    bool crypto;

    Stock( string s = "nullname" );
    void buy( const double d, const double );
    void sell( const double d, const double );
    double calcProfitTakeLimit();
    double calcStopLossLimit();
    bool checkName( fstream & );
    double getAsk( fstream & f, int );
    double getBid( fstream & f, int );
    int * calcSpans(const double* const buffer, int length );
    double * getBidsList( fstream & f, int length );
    double * getAsksList( fstream & f, int length );
    double getPosition( fstream &, string, bool );
    double getPositionCost( fstream &f, string n, bool );
    float calcInitBuyLmt();
    double calcSpread();
    double calcSpreadPercent();
    
    void print();
    /*float calcInitBuyPrcnt();
    float calcInitSellLmt();
    float calcInitSellPrcnt();
    float calcInitProfitTake();
    float calcInitStopLoss();*/
};

struct Portfolio{
    double totalMarketValue, cashBalance, dayBuyingPower, overnightBuyingPower, settledFunds,
        unsettledFunds, cryptoBuyingPower, optionBuyingPower;
    vector<Stock*> stockArr;
    unsigned int numStocks, RUNTIME;
    fstream fEvents, fPrice, fPositions, fPortfolio;
    fstream fStatus,bin;
    bool binEmpty, customInitialInputs;
    
    bool checkForBreakouts();
    void addStock( Stock * );
    void removeStock( string n );
    Portfolio();
    void getPrices();
    void getPositions();
    void printNames();
    void writeToBin();
    void getGains();
    void printBids();
    void checkStopLosses();
    void checkTakeProfits();
    void calcProfits();
    ~Portfolio();
    void refresh();        //refreshes data
    void checkSellStates();
    void printLimits();
    void checkBuyStates();
    void errIfEmptyPortfolio();
    void setupStockArr();
    void errorCheckInitLimits();
    double calcSpread();
    double calcSpreadPercent();
    void clearBin();
    void getInitLimits();
    void setStopLossAndTakeProfit();
    void printStocks();
    void checkCooldowns();

    ostream & print( ostream & out );
};

struct Baton{
    bool btn;
    fstream pBaton;
    fstream cBaton;
    time_t initTime; // the starting time after passing off the baton
    time_t curTime;
    
    Baton();
    bool batonCheck();
    void batonSet();
    bool timeout();

};

const string WHITESPACE = " \n\r\t\f\v',";
const int interval = 30;

int main(int argc, char* argv[]){
    Baton baton;
    time(&baton.initTime);
    time_t startTime;
    time_t finishTime; //1 loop cycle
    //wait while python logs in
    cout << "Waiting..." << endl;
    do{//check for baton. if we have baton, run. otherwise pause and check again

        baton.btn = baton.batonCheck();
        //get the current time and check if timeout has occured
        if( !baton.btn ){                
            time(&baton.curTime);
            sleep(1);
            //system("clear");
            if( baton.timeout() ){
                cout << "Error: timeout. Restarting in 15 seconds..." << '\a' <<endl;
                sleep(15);
                //restart getData.py and give it the baton
                restart();
                if( baton.batonCheck() ){
                    baton.batonSet();
                } 
            }
        }
    }while( !baton.btn );
    time(&startTime);

    //initialize my Portfolio object dynamically
    Portfolio * myPortfolioPtr = new Portfolio();
    //set up each stocks stop-loss and take profit levels
    myPortfolioPtr->setStopLossAndTakeProfit();
    //get position (for each stock)
    cout << fixed << setprecision(5);
    POSITIONLABEL0:
    //get the initial prices (for each stock) and sets up stop loss / take profit
    myPortfolioPtr->getPrices();
    myPortfolioPtr->getPositions();
    //myPortfolioPtr->errorCheckInitLimits();
                                                //loop indefinately
    //double * askArr = nullptr;
    //double * bidArr = nullptr;
    //int * spansArr = nullptr;
    Stock * trashPtr = nullptr;
    string name;
    float profit;
    unsigned short int buyCooldown = 0;
    unsigned short int sellCooldown = 0;
    unsigned short int oneSpanStreak = 0;
    short int neg1 = -1;
    time(&baton.initTime);
    bool breakOut = false;//represents if a breakout has occured or not(halts upper sell limit and starts trailing lowersell)
    short int sleepTime;
    time_t initTime;
    time_t curTime;
    string myString = "";    //the input the user sends while the program is paused
POSITIONLABEL2:
try{
    while(true){                                //LOOP STARTS
        myPortfolioPtr->checkCooldowns();
        

        //system("clear");

        do{//check for baton. if we have baton, run. otherwise pause and check again

            baton.btn = baton.batonCheck();
            //get the current time and check if timeout has occured
            if( !baton.btn ){                
                time(&baton.curTime);
                cout << "Waiting for baton("
                     << difftime( baton.curTime, baton.initTime ) - 30 << ")..." << endl;
                sleep(1);
                //system("clear");
                if( baton.timeout() ){
                    cout << "Error: timeout. Restarting in 15 seconds..." << '\a' <<endl;
                    sleep(15);
                    //restart getData.py and give it the baton
                    restart();
                    if( baton.batonCheck() ){
                        baton.batonSet();
                    } 
                }
            }
        }while( !baton.btn );
        time(&startTime);

        cout << fixed << setprecision(8);                          //get bid/ask (for each Stock)
                                                     
                                                                    //get bid and ask span (DISABLED)
            //get ask list and ask spans
            /*if( RUNTIME > 0 && !binEmpty ){
                delete []askArr;                                   //free any remaining allocated memory
                delete []spansArr;
                askArr = getAsksList( bin, RUNTIME );              //get ask list from .bin (with room for 1 more)
                askArr[RUNTIME] = myStockPtr->curAsk;              //add newest ask to ask list
                spansArr = calcSpans( askArr, RUNTIME+1 );         //calculate spans
                myStockPtr->span = spansArr[RUNTIME];              //update myStock's span
                cout << "ask span: " << myStockPtr->span << endl;
                if( myStockPtr->span == 1 ){
                    oneSpanStreak++;                               //accumulate one-span-streak
                } else if( myStockPtr->span > 1 ){
                    oneSpanStreak = 0;
                }
            }*/
                        
            /*if( RUNTIME > 0 && !binEmpty ){ put this in a function
                delete []bidArr;                                    //free any remaining allocated memory
                delete []spansArr;
                bidArr = getBidsList( bin, RUNTIME );                //get bid list from .bin (with room for 1 more)
                bidArr[RUNTIME] = myStockPtr->curBid;                //add newest bid to bid list
                spansArr = calcSpans( bidArr, RUNTIME+1 );           //calculate spans
                myStockPtr->span = spansArr[RUNTIME];                //update myStock's span
                cout << "bid span: " << myStockPtr->span << endl;

                if( myStockPtr->span > 19 )
                    cout << '\a';             //alert user if span > 19
            }*/
        
        //myPortfolioPtr->writeToBin();    //write my Stock object(s) to binary file
        myPortfolioPtr->errIfEmptyPortfolio();

        myPortfolioPtr->refresh();       //refresh portfolio
        
        printTime(cout);                                    //print time 
        myPortfolioPtr->print(cout);                        //print portfolio stats
        //get and print position and position cost(price) for each Stock
        cout << fixed << setprecision(8);
        POSITIONLABEL:
        myPortfolioPtr->getPositions();
        cout << setprecision(3);
        myPortfolioPtr->getPrices();
        //print position data if any for each Stock
        myPortfolioPtr->printStocks();
  
        cout << "checking stop losses / take profits ..." << endl;      //STOP LOSS / PROFIT TAKE (for each Stock)
		myPortfolioPtr->checkStopLosses();
        myPortfolioPtr->checkTakeProfits();

        cout << setprecision(8);
		cout << "checking for bullish flags..." << endl;//check for bullish flags (BULLISH)
        myPortfolioPtr->checkSellStates();              //check sell states for each element in stockArr
        cout << setprecision(2);

        myPortfolioPtr->checkBuyStates();
		cout << "checking for bearish flags..." << endl;//check for buy flags (BEARISH)
        cout << setprecision(8);
        //calc profits for each stock
        myPortfolioPtr->calcProfits();
        cout << setprecision(2);
        
        myPortfolioPtr->clearBin();
        baton.batonSet();
        time(&baton.initTime); // get the time of the baton hand-off
        time(&finishTime);
        cout << "main load time: " << finishTime - startTime << " seconds" << endl;
        
        //give user option to quit program for 30 seconds******bugged
        cout << "Press Enter to exit..." << endl;
        time(&initTime);
        do{
            time(&curTime);
            getline( cin, myString );
            if( myString.length() != 0 )
                killall();
        }while( curTime - initTime < interval );
        myPortfolioPtr->RUNTIME++;                                //loop count accumulator

    }//end of infinite loop
} 
catch(...){
    cout << "error caught... Trying again in 60 seconds...." << endl;
    sleep(60);
    restart();
}
    //delete []askArr;
    //delete []bidArr;
    //delete []spansArr;
    delete myPortfolioPtr;
    for(int i = 0; i < myPortfolioPtr->numStocks; i++ ){
        delete &(myPortfolioPtr->stockArr[i]);
    }


    return 0;

}


//========================================================
//function: CalcGain()
//description: returns the percent gain from the inital to the current
double calcGain( double initial, double current ){
    return ( ( current - initial ) / initial ) * 100;

}


//========================================getAsk===============================================
// receives: nothing
// description: opens price.txt and parses until the stock's data is reached
// returns: the quote of the stock
//========================================================================================================
double getAsk( fstream & f, int stockNum ){
    //read in quote from html filled file
        f.open( "price.txt" );
        if( !f ){                                  //test input stream for html
            cout << "getAsk Error: quoteStreamIn" << endl;
            abort();
        }
        f.seekg(0);
    
        double quote = -1.00;                      //this will contain the quote retreived from input file
        char ch;
        int count = 0;                  //keeps count how many times the askList keyword matches
        string word;
        string keyWord = "'askList':";
        while ( f >> word ){                                         //loop through each word
            if ( strcmp( &word[0], &keyWord[0] ) == 0 ){             //if word matches keyword
                count++;
                //if keyword has been matched as many times as there are number of stocks, then find the ask
                if( count == stockNum ){
                    for( int i = 0; i < 13; i++ ){                       //skip forward 13 bytes
                    f.get( ch );
                    }
        
            
                    word = "";                                           //empty word variable
                    f.get( ch );                                         //loop until the end of quote is reached
                    while( ch != '\'' ){
                       if( ch != ',' ){                                 //skip any ','
                           word += ch;
                      }
                          f.get( ch );  //increment
                    }//word now contains string version of quote price


                    quote = atof( &(word[0]) );                  //quote now contains the current quote price
                    f.close();                                   //break loop
                    //print current quote
                    //cout << "Current Quote: " << fixed << setprecision(3) << quote << endl;
                }
            }
        }

    if( quote == -1.00 ){
        cout << "ERROR RECIEVING QUOTE!!! EXITING PROGRAM";
        killall();
    }

    f.close();
    return quote;

}
//========================================================================================================


//========================================getBid===============================================
// receives: nothing
// description: opens htmlData.txt and parses until the stock's data is reached
// returns: the quote of the stock
//========================================================================================================
double getBid( fstream & f, int stockNum ){
    //read in quote from html filled file
        f.open( "price.txt" );
        if( !f ){                                  //test input stream for html
            cout << "getBid Error: quoteStreamIn" << endl;
            abort();
        }
        f.seekg(0);

        double quote = -1.00;                            //this will contain the quote retreived from inputfile
        char ch;
        string word;
        string keyWord = "'bidList':";
        int count = 0;
        while ( f >> word ){                             //loop through each word
            if ( strcmp( &word[0], &keyWord[0] ) == 0 ){ //if word matches keyword
                count++;
                if( count == stockNum ){ //record bid when keyword matches stockNum
                    for( int i = 0; i < 13; i++ ){           //skip forward 13 bytes
                        f.get( ch );
                    }
        
            
                    word = "";                               //empty word variable
                    f.get( ch );                             //loop until the end of quote is reached
                    while( ch != '\'' ){
                        if( ch != ',' ){                     //skip any ','
                           word += ch;
                       }
                        f.get( ch );  //increment
                    }                                        //word now contains string version of quote price


                    quote = atof( &(word[0]) );              //quote now contains the current quote price
                    f.close();                               //break loop
                    //print current quote
                    //cout << "Current Quote: " << fixed << setprecision(3) << quote << endl;
                }
            }

        }
    if( quote == -1.00 ){
        cout << "ERROR RECIEVING QUOTE!!! EXITING PROGRAM";
        f.close();
        abort();
    } else {
        f.close();
        return quote;
    }

}
//========================================================================================================


//==============================================buy===================================================
//function: buy
//recieves a double to resprent how many shares of the stock to buy
//        and a second double to represent the lowest price to buy at
//        and an int to represent the quantity of stocks to buy/sell (in stock mode)
void Stock::buy( const double d , const double lmt ) {
    //open and test status file
    fstream fStatus;
    fStatus.open("status.txt");
    if( !fStatus ){
        cout << "Error! Bad status file." << endl;
        abort();
    }

    fStatus << fixed << setprecision(8) << 1 << endl << stockName << endl 
        << d << endl << setprecision(0) << lmt << endl << this->crypto;
    cout << '\a';

    fStatus.close();
}
//========================================================================================================


//==============================================sell===================================================
//function: sell
//recieves a double to resprent how many shares of the stock to sell
//        and a second double to represent the highest price to sell at
//        and an int to represent the quantity of stocks to buy/sell (in stock mode)
void Stock::sell( const double d, const double lmt ) {
    //open and test status file
    fstream fStatus;
    fStatus.open("status.txt");
    if( !fStatus ){
        cout << "Error! Bad status file." << endl;
        abort();
    }

    fStatus << fixed << setprecision(8) << 2 << endl << stockName
        << endl << d << endl << setprecision(0) << lmt << endl << this->crypto;
    cout << '\a';
    fStatus.close();
}
//========================================================================================================

//Stock::calcSpread
double Stock::calcSpread(){
    return curAsk - curBid;
}

//Stock::calcSpreadPercent
double Stock::calcSpreadPercent(){
    return (curAsk - curBid) / curAsk * 100;
}

//=======================================================================================
// recieves a buffer full of prices and creates a dynamically allocated array of spans
int * calcSpans(const double* const buffer, int length ){
    //set prices
    Stock prices[length];
    for( int i = 0; i < length; i++ ){
        prices[i].curBid = buffer[i];
    }
    //prices array's Stocks are now filled with prices.

    //create and fill dynamic array
    int * arr = new int[length];

    stack<Stock> s;                                               //create a stack of stocks
    for( int day = 0; day < length; day++ ){                      //loop through each day
     prices[day].index = day;                                     //set index

         while ( !s.empty() && (s.top()).curBid <= prices[day].curBid ){ //if stack is not empty and
          s.pop();                                               //top of stock is <= current day's
                                                                 //price then pop
         }

         if ( s.empty() ){                                       // if empty, span is index + 1
            arr[day] = prices[day].index + 1;
        //cout << prices[day].index + 1 << endl;
        s.push( prices[day] );                                  //push current Stock to stack
         }//else if top of stack's price is greator than current day's price
         else if ( (s.top()).curBid > prices[day].curBid ){
                //cout << day - (s.top()).index << endl;        // stock is the day - top of stack's index
                arr[day] = day - (s.top()).index;
        s.push( prices[day] );                                  //push current stock to stack
         }
    }
    return arr; //MAKE SURE TO DELETE IN MAIN!

}
//=================================================================================================
// name: getBidsList
// recieves: the binary file (already opened) and the number of bids to read
// description: open the binary file at the beggining and
// returns an DYNAMICALLY ALLOCATED array of bids with RUNTIME length
double * getBidsList( fstream & f, int length ){
    f.clear(); //clear eof flag
    double * arr = new double[length+1];    //dynamically allocate memory for the array
                                            //+1 space for the newest bid

    //loop through each object in the binary file until all bids have been read
    f.seekg(0);
    Stock tempStockPtr;
    for( int i = 0; i < length; i++ ){
        f.read( reinterpret_cast<char*>(&tempStockPtr), sizeof(Stock));
        if( tempStockPtr.curBid == -1 ){
            cout << "Error reading Stock object from binary file" << endl;
            cout << "File size: " << fileSize( f ) << endl;
            abort();
        }

        arr[i] = tempStockPtr.curBid;
    }

    return arr; //LEAK DANGER! DONT FORGET TO DELETE THIS DYNAMIC ARRAY IN MAIN!
}

//=================================================================================================

double * getAsksList( fstream & f, int length ){
    f.clear();
    double * arr = new double[length+1];    //dynamically allocate memory for the array
                                            //length+1 because we leave space for the newest entry after
                                            //this is called in main

    //loop through each object in the binary file until all asks have been read
    f.seekg(0);
    Stock tempStockPtr;
    for( int i = 0; i < length; i++ ){
        f.read( reinterpret_cast<char*>(&tempStockPtr), sizeof(Stock) );
        if( tempStockPtr.curAsk == -1 ){
            cout << "Error reading Stock object from binary file" << endl;
            cout << "File size: " << fileSize( f ) << endl;
            abort();
            return 0;
        }

        arr[i] = tempStockPtr.curAsk;
    }

    return arr; //LEAK DANGER! DONT FORGET TO DELETE THIS DYNAMIC ARRAY IN MAIN!
}

//=================================================================================================


//Stock default constructor
//if string name is BTCUSD then main will recognize this object as crypto
Stock::Stock( string s ){
        initBid, initAsk, curBid, curAsk, mid, position,
       positionCost, resistance, support, profit, stopLossPercent,
       takeProfitPercent, stopLossLimit, takeProfitLimit, sellLimit,
       sellLimitPercent, buyLimit, buyLimitPercent = 0;
    this->takeProfitPercent = .90;
    this->stopLossPercent = .99;
    buyCooldown = 0;
    sellCooldown = 0;
    qty = 0;
    positionCost = -2;
    profit = 0.0;
    position = -2;
    stockName = s;
    this->crypto = false;                       //in stock mode by default
    unsigned short int arrLength = 2;
    string strArr[arrLength] = {"BTCUSD", "ETHUSD"};
    for( int i = 0; i < arrLength; i++ ){
        if( !strcmp( &stockName[0], &(strArr[i][0])) ){
            this->crypto = true;
        }
    }
    span = 0;
    index = -1;
    curBid = 0;
    curAsk = 0;
    initBid = 0;
    initAsk = 0;
    mid = 0;
}


//=================================================================================================

Portfolio::Portfolio(){
    customInitialInputs = true;
    numStocks = 0;
    fEvents.open("e.txt");                      //open and test e.txt
    if( !fEvents ){
        cout << "file error e.txt" << endl;
        abort();
    }
    //get stock/crypto name list
    //fEvents first tells us how many stocks are in fEvents before setting
    //    up stockArr with Stock objects.
    fEvents >> numStocks;
    if( numStocks < 1 ){
        cout << "Portfolio::Portfolio error: numStocks < 1" << endl;
        killall();
    }
    fEvents.close();
    setupStockArr();
    getInitLimits();

    bin.open( "dat.bin", ios::binary | ios::in | ios::out | ios::trunc | ios::ate );
    if( !bin ){
        cout << "Error! Bad dat.bin file." << endl;
        abort();
    }
    this->binEmpty = true;
    RUNTIME = 0;    //accumulator for # of loops ran

    fPortfolio.open("myPortfolio.txt");         //open and test myPortfolio.txt
    if( !fPortfolio ){
        cout << "Error! Bad portfolio file." << endl;
        abort();
    }

    fPositions.open("myPositions.txt" );         //open and test myPositions.txt
    if( !fPositions ){
        cout << "Error! Bad positions file." << endl;
        abort();
    }

    fPrice.clear();
    fPrice.open( "price.txt" );                  //open and test price.txt
    if( !fPrice )
        cout << "fPrice error" << endl;
    fPrice.close();

    fStatus.clear();
    fStatus.open( "status.txt" );               //open and test fStatus.txt
    if (!fStatus){
        cout << "file error: fStatus" << endl;
        abort();
    }
    fStatus.close();

    //Next, fEvents checks if user wants custom initial inputs
    /*string userIn = "-1";
    getline( fEvents, userIn, ',');
    customInitialInputs = false;
    if( userIn == "-1" ){
        cout << "Error fetching boolean input from e.txt (-1)" << endl;
        abort();
    } else if( userIn != "0" ){
        customInitialInputs = true;
    } else {
        cout << "Error: invalid boolean input from e.txt" << endl;
        abort();
    }*/

    

    while( !fPortfolio ){
        cout << "Portfolio::Portfolio\nmyPortfolio.txt is unavailable. Please wait..." << endl;
        sleep(10);
    }

    /*if( fin.eof() ){    //if file is loading, wait for it to load
        cout << "File is empty! Attempting to load..." << endl;
        sleep(3);
    }*/
    
    fPortfolio.seekg(0);
    string trash;
    string str;
    //read totalMarketValue
    fPortfolio >> trash >> str;
    str = trim( str );
    this->totalMarketValue = stod( str );

    //read cash Balance
    fPortfolio >> trash >> str;
    str = trim( str );
    this->cashBalance = stod( str );

    //read dayBuyingPower
    fPortfolio >> trash >> str;
    str = trim( str );
    this->dayBuyingPower = stod( str );
    
    //read overnightBuyingPower
    fPortfolio >> trash >> str;
    str = trim( str );
    this->overnightBuyingPower = stod( str );

    //read settledFunds
    fPortfolio >> trash >> str;
    str = trim( str );
    this->settledFunds = stod( str );

    //read unsettledFunds
    fPortfolio >> trash >> str;
    str = trim( str );
    this->unsettledFunds = stod( str );

    //read cryptoBuyingPower
    fPortfolio >> trash >> str;
    str = trim( str );
    this->cryptoBuyingPower = stod( str );

    //read optionBuyingPower
    fPortfolio >> trash >> str;
    str = trim( str );
    this->optionBuyingPower = stod( str );
}


//=================================================================================================


ostream & Portfolio::print( ostream & out ){
    out << "=======================================================================" << endl;
    out << fixed << setprecision(2);
    out << "Market Value:\t        " << this->totalMarketValue << '\t' << right;
    out << fixed << setprecision(2);
    out << "Cash Balance:           " << this->cashBalance << endl << left;
    out << "Crypto Buying Power:    " << this->cryptoBuyingPower << '\t' << right;
    out << "Option Buying Power:    " << this->optionBuyingPower << endl << left;
    out << "Settled Funds:          " << this->settledFunds << '\t' << right;
    out << "Unsettled Funds:        " << this->unsettledFunds << endl << left;
    out << "Day Buying Power:       " << this->dayBuyingPower << '\t' << right;
    out << "Overnight Buying Power: " << this->overnightBuyingPower << endl << left;
    out << "=======================================================================" << endl;


    return out;
}


//=================================================================================================

void Portfolio::refresh(){
if( RUNTIME > 0 ){
    while( !fPortfolio ){
        cout << "Portfolio::refresh:\nmyPortfolio.txt is unavailable. Please wait..." << endl;
        sleep(10);
    }

    if( fPortfolio.eof() ){    //if file is loading, wait for it to load
        cout << "File is empty! Attempting to load..." << endl;
        sleep(3);
    }

    fPortfolio.seekg(0);
    fPortfolio.clear();            //clear file flag
    string trash;
    string str;

    //read totalMarketValue
    fPortfolio >> trash >> str;
    str = trim( str );
    this->totalMarketValue = stod( str );

    //read cash Balance
    fPortfolio >> trash >> str;
    str = trim( str );
    this->cashBalance = stod( str );

    //read dayBuyingPower
    fPortfolio >> trash >> str;
    str = trim( str );
    this->dayBuyingPower = stod( str );
    
    //read overnightBuyingPower
    fPortfolio >> trash >> str;
    str = trim( str );
    this->overnightBuyingPower = stod( str );

    //read settledFunds
    fPortfolio >> trash >> str;
    str = trim( str );
    this->settledFunds = stod( str );

    //read unsettledFunds
    fPortfolio >> trash >> str;
    str = trim( str );
    this->unsettledFunds = stod( str );

    //read cryptoBuyingPower
    fPortfolio >> trash >> str;
    str = trim( str );
    this->cryptoBuyingPower = stod( str );

    //read optionBuyingPower
    fPortfolio >> trash >> str;
    str = trim( str );
    this->optionBuyingPower = stod( str );
    }
}


//=================================================================================================


ostream & printTime( ostream & out ){
    time_t t = time(0);   // get time now
    tm* now = localtime(&t);
    out << (now->tm_mon + 1) << '-'
         <<  now->tm_mday << '-'
         <<  (now->tm_year + 1900) << ' '
         <<  now->tm_hour << ':'
         <<  now->tm_min << ":"
         <<  now->tm_sec
        << endl;
    return out;
}

// searches for name, n, in file f. if no n found, returns -1
// c represents if stock is a crypto or not
double getPosition( fstream & f, string n, bool c){
        if( !f ){
            cout << "file error: getPosition" << endl;
            abort();
        }
        f.seekg(0);
        f.clear();
        char ch;
        string word;
        string keyWord = "'";
        string keyWordPt2 = "'},";
        keyWord += n;
        keyWord += keyWordPt2;    //keyword should now be 'n'},
        double pos = -1;
        while ( f >> word ){                                             //loop through each word
            if( strcmp( &word[0], &keyWord[0] ) == 0 ){                  //if word matches keyword
                if( !c )
                    f >> word >> word >> word >> word; //crypto skips 2 words, stock skips 4
                else if ( c )
                    f >> word >> word; //crypto skips 2 words, stock skips 4
            word = trim( word );
            pos = stod( word );
            break;
            }
        }

        f.seekg(0);
        f.clear();
        return pos;
}

/*
    getPositionCost
    recieves: file stream containing position data, stock name
                 c represents if stock is a crypto or not
    returns: the cost price for the stock position
*/
double getPositionCost( fstream & f, string n, bool c ){
        if( !f ){
            cout << "error:getpositioncost fstream" << endl;
            abort();
        }
        f.seekg(0);
        f.clear();
        char ch;
        string word;
        string keyWord = "'";
        string keyWordPt2 = "'},";
        keyWord += n;
        keyWord += keyWordPt2;    //keyword should now be 'n'},
        
        double pos = -1;
        bool flag = true;
        while ( f >> word && flag ){                                     //loop through each word
            if( strcmp( &word[0], &keyWord[0] ) == 0 ){                  //if word matches keyword
                //skip 7 words if crypto, skip 9 words if stock
            if( !c )
                f >> word >> word >> word >> word >> word >> word >> word >> word >> word;
            else if ( c )
                f >> word >> word >> word >> word >> word >> word >> word;
            f >> word;
            word = trim( word ); // word now contains position cost price
            pos = stod( word );
            flag = false;
            }
        }

        f.seekg(0);
        f.clear();
        return pos;
}


//Stock::checkName
//recieves: the current name and the prices.txt stream (unopened)
//description:
//    compares current stock name with new one and swaps if changed
//returns: true if name changed false if names are same
//            -creates DYNAMICALLY ALLOCATED string with new name and sends address to sPtr
bool Stock::checkName( fstream & f ){
    f.open( "price.txt" );
    if( !f ){
        cout << "file error checkName " << endl;
        abort();
    }
    f.seekg(0);
    f.clear();

    string keyWord = "'symbol':";
    string word;
    string newName;
    while ( f >> word ){
        if( !strcmp( &word[0], &keyWord[0] ) ){
            //skip forward 2 bytes
            f.get();
            f.get();
            getline( f, newName, '\'' );
            //newName now contains the new name
        }
    }

    f.close();
    string temp = this->stockName;
    this->stockName = newName;
    return ( strcmp( &(temp[0]), &newName[0]) );    //compare new name with old one
}

string getName( fstream & f ){
    f.open( "price.txt" );
    if( !f ){
        cout << "file error checkName " << endl;
        abort();
    }

    string keyWord = "'symbol':";
    string word;
    string newName;
    while ( f >> word ){
        if( !strcmp( &word[0], &keyWord[0] ) ){
            //skip forward 2 bytes
            f.get();
            f.get();
            getline( f, newName, '\'' );
            //newName now contains the new name
        }

    }

    f.close();
    return newName;
}


// Strip functions - for trimming characters out of strings
// uses WHITESPACE string to for delimitting characters
string ltrim(const string &s)
{
    size_t start = s.find_first_not_of(WHITESPACE);
    return (start == string::npos) ? "" : s.substr(start);
}
string rtrim(const string &s)
{
    size_t end = s.find_last_not_of(WHITESPACE);
    return (end == string::npos) ? "" : s.substr(0, end + 1);
}
string trim(const std::string &s) {
    return rtrim(ltrim(s));
}


// Baton: default constructor
// description:
//  creates streams for pBaton.txt and cBaton.txt
//
Baton::Baton(){
    btn = false;
    fstream cBaton;
    fstream pBaton;
}

// batonCheck
// description:
//  opens and tests cBaton.txt
//  and then returns the boolean character in cBaton.txt
//  and lastly closes file
bool Baton::batonCheck(){
    bool b;
    this->cBaton.open( "cBaton.txt" );
    if( !cBaton ){
        cout << "error in batonCheck()... cBaton.txt failed" << endl;
        abort();
    }
    
    cBaton >> b;
    cBaton.clear();
    cBaton.close();
    
    if( b != 0 && b != 1 ){
        cout << "BatonCheck() error: invalid input." << endl;
        abort();
    }
    
    this->btn = b;
    return b;
}

// batonSet
// description: sets cBaton.txt to 0
//      and pBaton.txt to 1
void Baton::batonSet(){
    bool b = true;
    this->cBaton.open( "cBaton.txt", ios::out | ios::trunc );
    if( !cBaton ){ //test file stream
        cout << "error opening cBaton.txt in batonSet() " << endl;
        abort();
    }
    cBaton << 0;
    if( cBaton >> b ){
        cout << "batonSet failed. cBaton is still true." << endl;
        abort();
    }
    cBaton.close();
    cBaton.clear();
    
    this->pBaton.open( "pBaton.txt", ios::out | ios::trunc );
    if( !pBaton ){ //test file stream
        cout << "error opening pBaton.txt" << endl;
        abort();
    }
    pBaton << 1;
    pBaton >> b;
    if( b != 1 ){
        cout << "batonSet failed. pBaton is still false." << endl;
        abort();
    }
    pBaton.close();
    pBaton.clear();
}


//returns true if f is a whole number and false if not.
bool isWhole( float f ){
    if( floor(f) == f )
        return true;
    else
        return false;
}


//returns the size of a file in bytes.
int fileSize( fstream & f ){
    f.clear();
    f.seekg( 0, ios::beg );
    int beg = f.tellg();
    f.seekg( 0, ios::end );
    int end = f.tellg();
    return end - beg;
}

//return: true if cur - start >= SLEEP_TIME + 30 sec
bool Baton::timeout(){
    bool b = false;
    if( difftime( this->curTime, this->initTime ) >= ( 60 ) ){
        b = true;
    }
    return b;
}

//restarts getData.py and main
void restart() {
    cout << "Restarting getData.py..." << endl;
    system("pkill python3");
    cout << endl;
    sleep(1);
    cout << "Replacing main..." << endl;
    system("python3 getData.py & ./main & wait");
    cout << "Restarted." << endl;
	exit(1);
}

//properly exits programs (OS : LINUX)
void killall(){
    system("pkill python3 ; pkill main");
}

//Portfolio Destructor
Portfolio::~Portfolio(){
    for(int i = 0; i < numStocks; i++ ){
        delete stockArr[i];
    }
    fEvents.close();
    fStatus.close();
    fPrice.close();
    fPortfolio.close();
}

//get prices for each Stock
void Portfolio::getPrices(){
    if( numStocks == 0 ){
        cout << "Portfolio::getPrices() Error... no stocks available... killing processes..." << endl;
        killall();
    }
    for( int i = 0; i < this->numStocks; i++ ){
            stockArr[i]->curAsk = getAsk( fPrice, i+1 );
            stockArr[i]->curBid = getBid( fPrice, i+1 );
            if( RUNTIME == 0){//set initial stock prices
                stockArr[i]->initAsk = stockArr[i]->curAsk;
                stockArr[i]->initBid = stockArr[i]->curBid;
            }
        //error check
        if( stockArr[i]->initAsk == 0 || stockArr[i]->initBid == 0 ){
            cout << "getPrices() error: initBid or initAsk is zero." << endl;
            killall();
        }
    }

}

//get positions for each Stock
//must be called before printStocks
void Portfolio::getPositions(){
    fPositions.seekg(0);
    for( int i = 0; i < this->numStocks; i++ ){
        this->stockArr[i]->position = getPosition( fPositions, this->stockArr[i]->stockName,     
            this->stockArr[i]->crypto );
        if( this->stockArr[i]->position > 0 ){
            this->stockArr[i]->positionCost = getPositionCost( fPositions, this->stockArr[i]->stockName, 
            this->stockArr[i]->crypto  );
        } 
        else{
            this->stockArr[i]->position = 0;
        }
    }
}

//print name of each Stock
void Portfolio::printNames(){
    for( int i = 0; i < this->numStocks; i++ ){
        cout << "Portfolio Element #" << i << ": " << stockArr[i]->stockName << endl;
    }
}

//Writes each stock in stockArr to binary file
void Portfolio::writeToBin(){
    for( int i = 0; i < numStocks; i++ ){
        bin.write( reinterpret_cast<char*>( &(this->stockArr[i]) ), sizeof(Stock) );
    }
    binEmpty = false;
}

//prints gains for each Stock
void Portfolio::getGains(){
    for( int i = 0; i < this->numStocks; i++ ){
           if( stockArr[i]->position ){
                stockArr[i]->profit = calcGain(stockArr[i]->positionCost,stockArr[i]->curBid);
                cout<< '\t' << "(" << setprecision(2)
                << stockArr[i]->profit << "%)" << endl;
            } else if( stockArr[i]->position < 0 )
                cout << endl;
            if( stockArr[i]->position ){
                //do nothing
            } else if ( stockArr[i]->position < 0 )
                cout << endl;
    }
}

//prints bid of each Stock
void Portfolio::printBids(){
    for( int i = 0; i < this->numStocks; i++ ){
        cout << stockArr[i]->stockName << " bid: " << stockArr[i]->curBid << "\t\t";
    }
}

//Checks stop losses for each Stock
void Portfolio::checkStopLosses(){
    for( int i = 0; i < this->numStocks; i++ ){
        if( stockArr[i]->position > 0 && !stockArr[i]->sellCooldown ){
            stockArr[i]->profit = calcGain( getPositionCost( fPositions, stockArr[i]->stockName, stockArr[i]->crypto ),
                stockArr[i]->curBid );
            if( stockArr[i]->profit <= stockArr[i]->stopLoss ){
	     		if( stockArr[i]->crypto )
             	    stockArr[i]->sell( stockArr[i]->position * stockArr[i]->stopLossPercent, ( stockArr[i]->curBid 
                        * 95 ));
			    else if ( !stockArr[i]->crypto ){
				    stockArr[i]->qty = (stockArr[i]->position * stockArr[i]->stopLossPercent) / stockArr[i]->curBid;
            	    stockArr[i]->sell( stockArr[i]->position, 999999 );
			}
        	cout << "STOP LOSS TRIGGERED" << endl;
            stockArr[i]->sellCooldown = 30;    //set sell cooldown to 30 minutes
		    sleep(5);  //wait for sell to go thru
        	}
		}
    }
}

void Portfolio::checkTakeProfits(){
    for( int i = 0; i < this->numStocks; i++ ){
        if( stockArr[i]->position > 0 && !stockArr[i]->sellCooldown ){
            stockArr[i]->profit = calcGain( getPositionCost( fPositions, stockArr[i]->stockName, stockArr[i]->crypto ), stockArr[i]->curBid );
            if( stockArr[i]->profit >= stockArr[i]->profitTake ){
	     		if( stockArr[i]->crypto )
             	    stockArr[i]->sell( stockArr[i]->position * stockArr[i]->takeProfitPercent,  
                        ( stockArr[i]->curBid * 95 ));
			    else if ( !stockArr[i]->crypto ){
				    stockArr[i]->qty = (stockArr[i]->position * stockArr[i]->takeProfitPercent ) / stockArr[i]->curBid;
            	    stockArr[i]->sell( stockArr[i]->qty, stockArr[i]->qty );
			}
        	cout << "TAKE PROFIT TRIGGERED" << endl;
            stockArr[i]->sellCooldown = 30;    //set sell cooldown to 30 minutes
		    sleep(5);  //wait for sell to go thru
        	}
		}
    }
}

//calculate the limit of gain at which to take profit
double Stock::calcProfitTakeLimit(){
    return ( (profitTake / 100) * positionCost ) + positionCost;
}

//calculate the limit at which to trigger stop loss
//stopLossPercent represents at how much % gain we will be selling
double Stock::calcStopLossLimit(){

    double d = positionCost + (positionCost * (stopLoss / 100));
    return d;
}

//calculate profits from position of each stock
void Portfolio::calcProfits(){
    for( int i = 0; i < this->numStocks; i++ ){
        stockArr[i]->profit = calcGain( getPositionCost( fPositions, stockArr[i]->stockName, stockArr[i]->crypto  ), stockArr[i]->curAsk );
    }
}

//check sell states for each element in stockArr
void Portfolio::checkSellStates(){
    for( int i = 0; i < this->numStocks; i++ ){
        if( stockArr[i]->crypto && stockArr[i]->curBid >= stockArr[i]->sellLimit && stockArr[i]->span >= 1  
                && !(stockArr[i]->sellCooldown) && stockArr[i]->position > 0 && !checkForBreakouts() ){    
            stockArr[i]->sell( (stockArr[i]->position * .97 ) * stockArr[i]->sellLimitPercent, ( stockArr[i]->curBid * .95 ) );
            stockArr[i]->sellLimit = stockArr[i]->sellLimit * 1.03;
            //every sell after x increase results in 3% increased limits for future profits
            stockArr[i]->sellCooldown = 10;
        }   //check sell state (if !crypto) for element in stockArr 
         else if( !stockArr[i]->crypto && stockArr[i]->curBid >= stockArr[i]->sellLimit && stockArr[i]->span >= 1
                && !(stockArr[i]->sellCooldown) && stockArr[i]->position > 0){
            stockArr[i]->qty = (stockArr[i]->position * stockArr[i]->sellLimitPercent) / stockArr[i]->curBid;
            stockArr[i]->sell( stockArr[i]->qty, stockArr[i]->qty );
            stockArr[i]->sellLimit = stockArr[i]->sellLimit * 1.03;
            stockArr[i]->sellCooldown = 10;
        }
    }
}


//print limit data for each element in stockArr
void Portfolio::printLimits(){
    for( int i = 0; i < this->numStocks; i++ ){
        //cout << "init bid:" << stockArr[i]->initBid << endl;
        //cout << "init ask:" << stockArr[i]->initAsk << endl;
        cout << "Buy limit: ask < " << stockArr[i]->buyLimit << endl;
        cout << "Sell limit: bid > " << stockArr[i]->sellLimit << endl;
        cout << setprecision(3) << "Take-Profit at " << stockArr[i]->calcProfitTakeLimit()
            << " (+" << stockArr[i]->profitTake << "%)" << endl;
        cout << "Stop-Loss at " << stockArr[i]->calcStopLossLimit()
             << "(" << stockArr[i]->stopLoss << "%)" << setprecision(3);
    }
}

//check buy states
void Portfolio::checkBuyStates(){
    for( int i = 0; i < this->numStocks; i++ ){
        //check buy state (if crypto) for each element in stockArr
        if( stockArr[i]->crypto && stockArr[i]->curAsk <= stockArr[i]->buyLimit 
            && !stockArr[i]->buyCooldown /*&& stockArr[i]->span == 1 && oneSpanStreak >= 2*/ ){
            stockArr[i]->buy( ( (this->cryptoBuyingPower * .97 ) * stockArr[i]->buyLimitPercent )
			/ (stockArr[i]->curAsk) , stockArr[i]->curAsk * 1.05 );
            stockArr[i]->buyLimit = stockArr[i]->buyLimit * .95;
            stockArr[i]->buyCooldown = 10;
        }
        //check buy state (if stock) for each element in stockArr
        if( !stockArr[i]->crypto && stockArr[i]->curAsk <= stockArr[i]->buyLimit && !stockArr[i]->buyCooldown /*&& stockArr[i]->span == 1 && oneSpanStreak >= 2*/){
            stockArr[i]->qty = (this->dayBuyingPower * stockArr[i]->buyLimitPercent) / stockArr[i]->curAsk;
            stockArr[i]->buy( stockArr[i]->qty , stockArr[i]->qty );
            stockArr[i]->buyLimit = stockArr[i]->buyLimit * .95;
            stockArr[i]->buyCooldown = 10;
        }
    }
}

//Uses polymorphism to loop through each stock and check for breakouts while adjusting limits accordlingly.
//A breakout is determined through various algorithmic indicators in a minute/hourly/daily/monthly scope.
//The current breakout indicators are: 
//Momentum Meter(12 day and 26 day MACD EMA ), 
//signal line (9 day ema of MACD line),
//MACD Histogram, ( MACD line - signal line )  *use price rejection as entry trigger* 
//    - MACD > signa line then bullish flag
//    - MACD < signal line then bearish flag
//(halts upper sell limit and starts trailing lowersell)
//TODO: implement functionality to check option orders
bool Portfolio::checkForBreakouts(){
    bool breakoutFlag = false;
    for( int i = 0; i < this->numStocks; i++ ){
       //check Stock for a breakout in each scope
    }
    return breakoutFlag;
}

/*float Stock::calcInitBuyLmt(){
    return
}

float Stock::calcInitBuyPrcnt(){
    return;
}

float Stock::calcInitSellLmt(){
    return;
}

float Stock::calcInitSellPrcnt(){
    return;
}

//returns the percent of profit gain to take profits at
float Stock::calcInitProfitTake(){
    float f = stod(this->profitTakeStr);
    return f;
}

//returns the percent of profit loss to trigger stop loss
float Stock::calcInitStopLoss(){
     float f = stod(this->stopLossStr);
     return f;
}*/

/*
*/
void Portfolio::getInitLimits(){
    fEvents.open("e.txt");
    if(!fEvents){
        cout << "getInitLimits() error: fevents stream" << endl;
        killall();
    }
    string tempStr = "";
    string str;
    fEvents.seekg(0);
    getline( fEvents, str); //skip line
    for( int i = 0; i < this->numStocks; i++ ){
    //get buy limit, buy limit %, sell limit, sell limit % from e.txt (for each stock)
        if( customInitialInputs ){
            getline( fEvents, stockArr[i]->stockName, ',' );
            getline( fEvents, stockArr[i]->buyLimitStr, ',' );    //set buyLimit
            getline( fEvents, stockArr[i]->buyLimitPercentStr, ',' ); //set buyLimitPercent
            getline( fEvents, stockArr[i]->sellLimitStr, ',' );   //set sellLimit
            getline( fEvents, stockArr[i]->sellLimitPercentStr, ',' ); //set sellLimitPercent
            getline( fEvents, stockArr[i]->profitTakeStr, ',' );  //set profitTake
            getline( fEvents, stockArr[i]->stopLossStr );         //set stopLoss
            //set profitTake
            stockArr[i]->profitTake = stod(trim(stockArr[i]->profitTakeStr));
            //set stopLoss
            stockArr[i]->stopLoss = stod(trim(stockArr[i]->stopLossStr));
            
            //convert strings to doubles
            stockArr[i]->buyLimit = stod( trim(stockArr[i]->buyLimitStr) );
            // % of our available money we will spend once the user limit is surpassed
            stockArr[i]->buyLimitPercent = stod( trim(stockArr[i]->buyLimitPercentStr)) / 100;
            stockArr[i]->sellLimit = stod( trim(stockArr[i]->sellLimitStr) );
	        // % of our shares we will sell once the user sell limit is surpassed
            stockArr[i]->sellLimitPercent = stod( trim(stockArr[i]->sellLimitPercentStr)) / 100;
            stockArr[i]->takeProfitLimit = stod( trim(stockArr[i]->profitTakeStr) );
            stockArr[i]->stopLossLimit = stod( trim(stockArr[i]->stopLossStr) );
        } else if (!customInitialInputs){
            //set buyLimit
           /*     stockArr[i]->buyLimit = stockArr[i]->calcInitBuyLmt();
            //set buyLimitPercent
                stockArr[i]->buyLimitPercent = stockArr[i]->calcInitBuyPrcnt();
            //set sellLimit
                stockArr[i]->sellLimit = stockArr[i]->calcInitSellLmt();
            //set sellLimitPercent
                stockArr[i]->sellLimitPercent = stockArr[i]->calcInitSellPrcnt();
            //set profitTake
                stockArr[i]->profitTake = stockArr[i]->calcInitProfitTake();
            //set stopLoss
                stockArr[i]->stopLoss = stockArr[i]->calcInitStopLoss();*/
        }
    }
    fEvents.close();
}

void Portfolio::errIfEmptyPortfolio(){
        if( fPortfolio.eof() ){
            cout << "Portfolio File empty!" << endl;
            abort();
        }
}

void Portfolio::clearBin(){
//clear bin after every 150 loops by closing and re-opening the file
        if( !(RUNTIME % 150) && !binEmpty && RUNTIME != 0 ){
            cout << "Adjusting bin size... (" << fileSize(bin) << ")" << '\a' << endl;
            bin.close();
            bin.open( "dat.bin", ios::binary | ios::in | ios::out | ios::trunc | ios::ate );
            if( !bin || ( fileSize(bin) != 0 ) ){
                cout << "Error while clearing dat.bin file." << endl;
                abort();
            }
            binEmpty = true;
            cout << "bin empty." << endl;
        }
}

// setupStockArr()
// after retrieving numStocks from fEvents, we can now add Stock 
//     objects to the stockArr dynamic array.
void Portfolio::setupStockArr(){
    fEvents.open("e.txt");
    if(!fEvents){
        cout << "setupStockArr() error: fEvents fstream" << endl;
        killall();    
    }
    fEvents.seekg(0);
    string tempStockName = "";
    getline(fEvents,tempStockName); //skip first line
    for( int i = 0 ; i < numStocks; i++ ){
        //get stockName from fEvents
        getline( fEvents, tempStockName, ',' );//get each stockName from fEvents
        
        //fStatus << '0' << endl << stockArr[i]->stockName << endl;//send fStatus each StockName
       if( tempStockName == "" ){
            cout << "Error finding a stockName" << endl;
            abort();
        }
        stockArr.push_back(new Stock( tempStockName ));
        getline( fEvents, tempStockName, ',' );//skip a data segment
        getline( fEvents, tempStockName, ',' );//skip a data segment
        getline( fEvents, tempStockName, ',' );//skip a data segment
        getline( fEvents, tempStockName, ',' );//skip a data segment
        getline( fEvents, tempStockName, ',' );//skip a data segment
        getline( fEvents, tempStockName, ',' );//skip a data segment
        
    }
    fStatus.close();
    fEvents.close();
}

// if any limits are still set to their initalized amount of zero, throw error
void Portfolio::errorCheckInitLimits(){
for( int i = 0; i < numStocks; i++ ){
    if( stockArr[i]->buyLimit < 0 || isalpha(stockArr[i]->buyLimit) ){ //error check buyLimit
        cout << stockArr[i]->stockName << "buy limit input error: " << stockArr[i]->buyLimit << endl;
        abort();
    }
    if( stockArr[i]->sellLimit == 0 || isalpha(stockArr[i]->sellLimit) ){ //error check sellLimit
        cout << stockArr[i]->stockName << "sell limit input error: " << stockArr[i]->sellLimit << endl;
        abort();
    }
    if( !stockArr[i]->takeProfitLimit || isalpha(stockArr[i]->takeProfitLimit) ){ //error check buyLimit
        cout << stockArr[i]->stockName << "take profit input error: " << stockArr[i]->takeProfitLimit << endl;
        abort();
    }
    if( !stockArr[i]->stopLossLimit || isalpha(stockArr[i]->stopLossLimit) ){ //error check sellLimit
        cout << stockArr[i]->stockName << "stop loss input error: " << stockArr[i]->stopLossLimit << endl;
        abort();
    }
}
}


//loops through each stock in stockArr and sets up stop loss and take profits
//must be called after prices are first retrieved
void Portfolio::setStopLossAndTakeProfit(){
    int num = this->numStocks;
    for( int i = 0; i < num; i++ ){
        //now that the prices are updated set up stop loss and take profits 
        stockArr[i]->takeProfitLimit = stockArr[i]->calcProfitTakeLimit();  //set initial takeProfit value
        stockArr[i]->stopLossLimit = stockArr[i]->calcStopLossLimit();      //set initial stopLoss value
    }
}

// Stock::print()
// prints some of the Stock object data.
void Stock::print(){
    cout << '-' << stockName << '-' << endl;
    if( !position ){
        cout << "no position found" << endl;    
    } else if (position > 0) {
        cout << "Position: " << position
                 << " @ "<< positionCost << "\t(" << calcGain(positionCost, curBid) << "%)"<< endl;
    } else{
        cout << "Stock::print() error: position... killing processes..." << endl; 
        killall();   
    }
    cout << "Current Ask: " << curAsk << endl;
    cout << "Current Bid: " << curBid << endl;
    cout << setprecision(3) << "Ask/Bid Spread: " << calcSpread() << setprecision(2);
    cout << "   (" << calcSpreadPercent()  << "%)" << setprecision(8) << endl;
    cout << setprecision(3) <<  "Buy limit: ask < " << buyLimit << " -> searching when to spend " 
         << buyLimitPercentStr << '%' << endl;
    if( position ){
    cout << "Sell limit: bid > " << sellLimit << " -> searching when to sell " << sellLimitPercentStr << '%' << endl;
    cout << "Take-Profit at " << calcProfitTakeLimit()
         << " (+" << profitTake << "%)" << endl;
    cout << "Stop-Loss at " << calcStopLossLimit()
         << " (" << stopLoss << "%)" << setprecision(3) << endl;
    }
    if( buyCooldown )
        cout << "buyCooldown: " << buyCooldown << endl;
    if( sellCooldown )
        cout << "sellCooldown: " << sellCooldown << endl;
    cout <<"----------------------------------------------------------------------" << endl;
}

// Portfolio::printStocks()
// casts print on each Stock object in stockArr
void Portfolio::printStocks(){
    if( numStocks == 0 ){
        cout << "printStocks error... zero stocks... aborting..." << endl;
        killall();    
    }
    for( int i = 0; i < numStocks; i++ ){
        stockArr[i]->print();   
    }
}

void Portfolio::checkCooldowns(){
    for( int i = 0; i < numStocks; i++ ){
        if( stockArr[i]->buyCooldown ){
            stockArr[i]->buyCooldown--;
        }
        if( stockArr[i]->sellCooldown ){
            stockArr[i]->sellCooldown--;
        }
    }
}
