#include "ExtraLifePowerup.h"
#include "lib/lib.h"
#include <d3dx9.h>

ExtraLifePowerup::ExtraLifePowerup(float posX, float posY) : Powerup(posX, posY)
{
	type = PowerupType::ExtraLife;
	sprite = Lib::LoadSprite("Powerup/ExtraLife.png");
}
