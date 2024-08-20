#pragma once
#include <iostream>
#include <chrono>

class GameClock
{
public: 
	static GameClock& getInstance() {
		static GameClock clock;
		return clock;
	}
	
	/*
	* TODO:
	*	Add a getCurrentTime()
	*	Add a isTimeBefore()
	*	Add a isTimeAfter()
	*	Add a getTimeDifference()
	*
	*/
	



	GameClock(const GameClock&) = delete;
	GameClock& operator=(const GameClock&) = delete;
private:
	GameClock() {

	}
};

