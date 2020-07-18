#pragma once
#include"character.h"

class Player
{
public:
	Player(std::shared_ptr<Character>character);
	void Update(float elapsd_time);
	std::shared_ptr<Character>GetCharacter() { return mCharacter; }
private:
	std::shared_ptr<Character>mCharacter;
};