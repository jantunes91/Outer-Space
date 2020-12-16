#include "IncreaseFireRatePowerup.h"
#include "lib/lib.h"
#include <d3dx9.h>

IncreaseFireRatePowerup::IncreaseFireRatePowerup(float posX, float posY) : Powerup(posX, posY)
{
	type = PowerupType::IncreaseFireRate;
	sprite = Lib::LoadSprite("Powerup/FireRate.png");
}
