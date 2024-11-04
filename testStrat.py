import yfinance as yf
import pandas as pd
import talib
import matplotlib.pyplot as plt
import numpy as np
import warnings
import math
import sys

# Suppress future warnings
warnings.simplefilter(action='ignore', category=FutureWarning)


def backtest_strategy(symbol, start_date, end_date, initial_investment):
    # Load the stock data
    stock_data = calculate_technical_indicators(symbol, start_date, end_date)

    # Initialize the buy and sell lists
    buy_list = []
    sell_list = []
    total_profit = 0.0
    current_position = 0
    position_cost = 0
    numBuying = 1
    buy_price = 0
    numSelling = 1
    myMoney = initial_investment
    numBuying = 0
            


    # Plotting
    plt.figure(figsize=(12,5))

    # Plot the stock price
    plt.plot(stock_data.index, stock_data['Close'], label='Stock Price', color='blue')

    # Plot the Bollinger Bands
    plt.plot(stock_data.index, stock_data['MA20'], label='Middle Bollinger Band', color='red')
    plt.plot(stock_data.index, stock_data['Upper Band'], label='Upper Bollinger Band', color='cyan')
    plt.plot(stock_data.index, stock_data['Lower Band'], label='Lower Bollinger Band', color='cyan')



    # Plot the orange arrow where the span is one and the previous span is greater than 50
    for i in range(1, len(stock_data)):
        # Get the current price
        current_price = stock_data['Close'][i]
    # Plot the first purchase of stock
        if buy_list:
            first_purchase_index, first_purchase_value = buy_list[0]
            stopLoss = first_purchase_value * .75
            plt.plot(stock_data.index[i], first_purchase_value, marker='.', color='black')
            #number of stocks to buy
            #(my money / current price ) / x
            #if the market is in an uptrend over the years then x should be like 5
            if( current_price > first_purchase_value ):
                numBuying = math.floor( (myMoney / current_price) / 5 )
            elif( current_price < first_purchase_value ):
            #if the market is in a downtrend over the years then x should be like 10
                numBuying = math.floor( (myMoney / current_price) / 10 )

            #number of stocks to sell if triggered
            #current position / x
            #if the market is in an uptrend over the years then x should be higher like 10
            if( current_price > first_purchase_value ):
                numSelling = math.floor(current_position / 10 )
            #if market is in a downtrend over the years then x should be 2
            elif (current_price < first_purchase_value):
                numSelling = math.floor(current_position / 2 )

            if( current_position == 0 ):
                position_cost = current_price
        elif not buy_list:
            numBuying = math.floor( (myMoney / current_price) / 10 )
            stopLoss = current_price


       #sell signal
        if (stock_data['Close'][i] > stopLoss ):
            if (stock_data['Close'][i] > stock_data['Upper Band'][i] and position_cost <= current_price):
                # Calculate profit if there are stocks to sell
                if current_position > 1:
                    if numSelling == 0:
                        numSelling = 1
                    profit = (current_price - position_cost) * numSelling #calculate profit
                    total_profit += profit
                    current_position = current_position - numSelling
                    myMoney += (current_price * numSelling)
                    plt.plot(stock_data.index[i], stock_data['Close'][i], color='red', marker='v', markersize=8)
                    sell_list.append(current_price)
                    print(f"Sold {numSelling} @ {current_price}\nCurrent Position: {current_position}")
                    print(f"Total profit: {total_profit}\nPosition_cost:{position_cost}\n")
        elif (current_price < stopLoss ):
            # Calculate profit if there are stocks to sell
            if current_position > 1:
                if numSelling == 0:
                    numSelling = 1
                numSelling = current_position
                profit = (current_price - position_cost) * numSelling #calculate profit
                total_profit += profit
                current_position = current_position - numSelling
                myMoney += (current_price * numSelling)
                plt.plot(stock_data.index[i], stock_data['Close'][i], color='red', marker='v', markersize=8)
                sell_list.append(current_price)
                print(f"Sold {numSelling} @ {current_price}\nCurrent Position: {current_position}")
                print(f"Total profit: {total_profit}\nPosition_cost:{position_cost}\n")
            

       #buy signal
       #if positive momentum and the price has gone down for 4 days in a row
        elif stock_data['MACD'][i] > 0 and numBuying > 0 and stock_data['MACD'][i] > stock_data['MACD'][i-1] and current_price >= stopLoss:
            #if stock span is 1 for consequtive days and below 1.05x lower band
            if(stock_data['Stock_Span'][i] == 1 and
                stock_data['Stock_Span'][i-1] == 1 and
                stock_data['Stock_Span'][i-2] == 1 and
                stock_data['Stock_Span'][i-3] == 1 and
                stock_data['Close'][i] < (stock_data['Lower Band'][i] * 1.05)):
                    #calculate position_cost
                    if( current_position > 0):
                        position_cost = (position_cost + current_price) / 2
                    elif( current_position == 0):
                        position_cost = current_price
                    plt.plot(stock_data.index[i], stock_data['Close'][i], color='green', marker='v', markersize=8)
                    buy_list.append((len(buy_list), current_price))
                    current_position += numBuying
                    print(f"Purchased {numBuying} @ {current_price}\nCurrent Position: {current_position}\nPosition_cost:{position_cost}\n\n")
                    myMoney -= current_price * numBuying

        #or buy if stock close is below lower band
        elif numBuying > 0 and stock_data['Close'][i] < stock_data['Lower Band'][i] and current_price >= stopLoss:
                #calculate position_cost
            if( current_position > 0):
                position_cost = (position_cost + current_price) / 2
            elif( current_position == 0):
                position_cost = current_price
            plt.plot(stock_data.index[i], stock_data['Close'][i], color='green', marker='v', markersize=8)
            buy_list.append((len(buy_list), current_price))
            current_position += numBuying
            myMoney -= current_price * numBuying
            print(f"Purchased {numBuying} @ {current_price}\nCurrent Position: {current_position}\nPosition_cost:{position_cost}\n\n")







    print(f"Initial Investment: {initial_investment}")
    print(f"My cash: {myMoney:.2f}")
    moneyInvested = current_price * current_position
    print(f"Market Value of Investments: {moneyInvested:.2f}")
    investedToCashRatio = moneyInvested / myMoney
    print(f"Invested vs Cash Ratio: {investedToCashRatio:.2f}")
    print(f"Current Price: {stock_data['Close'][-1]:.2f}")
    print(f"Current Position Cost: {position_cost:.2f}")
    print(f"Current Position: {current_position}")
    totalMoney = moneyInvested + myMoney
    print(f"My Total: {totalMoney:.2f}")
    # Calculate total percent profit
    exiting_profit = (current_position * stock_data['Close'][-1]) - (current_position * position_cost)
    print(f"Exiting Profit: {exiting_profit:.2f}")
    total_percent_profit = (totalMoney / initial_investment) * 100
    print(f"Total Profit: {total_percent_profit:.2f}%")

    # Plot the MACD histogram
    plt.bar(stock_data.index, stock_data['MACD'], label='MACD Histogram', color='gray')
        

    # Plot the buy signals
    #for buy in buy_list:
        #plt.plot(buy[0], buy[1], color='green', marker='v', markersize=8)

    # Plot the sell signals
    #for sell in sell_list:
        #plt.plot(sell[0], sell[1], color='red', marker='^', markersize=8)

    plt.title(f'{symbol} Buy and Sell Signals with Bollinger Bands and Ichimoku Kinko Hyo Spans')
    plt.xlabel('Date')
    plt.ylabel('Price')
    plt.legend(loc='upper left')
    plt.grid(True)
    plt.show()


def check_for_buy_signal(data, i):
    # Define your buy signal logic here using the technical indicators
    # Example: Buy when the price crosses above the lower Bollinger Band, ROC is positive, or RSI is below 30
    if (data['Close'][i] < data['Lower Band'][i]):
        return True
    # Add more conditions based on other indicators for a buy signal
    return False

def check_for_sell_signal(data, i):
    # Define your sell signal logic here using the technical indicators
    # Example: Sell when the price crosses below the upper Bollinger Band, MACD is negative, or RSI is above 70
    if (data['Close'][i] > data['Upper Band'][i]):
        return True
    # Add more conditions based on other indicators for a sell signal
    return False

def calculate_stock_span(rates):
    stack = []  # Stack to store indices of rates
    spans = [1] * len(rates)  # Initialize spans with default value 1

    # Iterate through each rate
    for i in range(len(rates)):
        # Pop elements from the stack while the stack is not empty and the current rate is greater than the rate at the top of the stack
        while stack and rates[i] >= rates[stack[-1]]:
            stack.pop()

        # If the stack is empty after popping, the span is i + 1
        # Otherwise, the span is i - the index at the top of the stack
        spans[i] = i + 1 if not stack else i - stack[-1]

        # Push the current index onto the stack
        stack.append(i)

    return spans

def calculate_technical_indicators(stock_symbol, start_date, end_date):
    # Download stock data
    stock_data = yf.download(stock_symbol, start=start_date, end=end_date)

    # Calculate ROC
    stock_data['ROC'] = talib.ROC(stock_data['Close'], timeperiod=14)

    # Calculate MACD
    stock_data['MACD'], stock_data['Signal Line'], _ = talib.MACD(stock_data['Close'])

    # Calculate Bollinger Bands
    stock_data['MA20'] = stock_data['Close'].rolling(window=20).mean()
    stock_data['Upper Band'] = stock_data['MA20'] + 2 * stock_data['Close'].rolling(window=20).std()
    stock_data['Lower Band'] = stock_data['MA20'] - 2 * stock_data['Close'].rolling(window=20).std()

    # Calculate Stochastic Oscillator
    stock_data['%K'], stock_data['%D'] = talib.STOCH(stock_data['High'], stock_data['Low'], stock_data['Close'])

    # Calculate Tenkan-sen (Conversion Line)
    nine_period_high = stock_data['High'].rolling(window=9).max()
    nine_period_low = stock_data['Low'].rolling(window=9).min()
    stock_data['Tenkan_sen'] = (nine_period_high + nine_period_low) / 2

    # Calculate Kijun-sen (Base Line)
    twenty_six_period_high = stock_data['High'].rolling(window=26).max()
    twenty_six_period_low = stock_data['Low'].rolling(window=26).min()
    stock_data['Kijun_sen'] = (twenty_six_period_high + twenty_six_period_low) / 2

    # Calculate Senkou Span A (Leading Span A)
    stock_data['Senkou_span_A'] = ((stock_data['Tenkan_sen'] + stock_data['Kijun_sen']) / 2).shift(26)

    # Calculate Senkou Span B (Leading Span B)
    fifty_two_period_high = stock_data['High'].rolling(window=52).max()
    fifty_two_period_low = stock_data['Low'].rolling(window=52).min()
    stock_data['Senkou_span_B'] = ((fifty_two_period_high + fifty_two_period_low) / 2).shift(26)

    # Calculate Stock Span
    rates = stock_data['Close'].tolist()
    stock_data['Stock_Span'] = calculate_stock_span(rates)

    return stock_data

def main():
    if len(sys.argv) != 5:
        print("Parameters should include stock symbol, start date(YYYY-MM-DD), end date(YYYY-MM-DD), and initial investment")
        sys.abort(1)

    start_date = sys.argv[1]
    end_date = sys.argv[2]
    init_investment = float(sys.argv[4])

    stock_symbol = sys.argv[3]
    # Plot the data
    backtest_strategy(stock_symbol, start_date, end_date, init_investment)

if __name__ == "__main__":
    main()


