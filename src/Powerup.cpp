#include "Powerup.h"
#include "lib/lib.h"
#include <d3dx9.h>

const float k_powerupSpeed = 200;

Powerup::Powerup(float posX, float posY)
{
	sprite = nullptr;
	type = PowerupType::None;

	positionX = posX;
	positionY = posY;

	toDestroy = false;
}

void Powerup::Tick()
{
	if (toDestroy) return;

	positionY += Lib::GetTimeSinceLastFrameInSeconds() * k_powerupSpeed;

	if (OutsideViewport())
	{
		toDestroy = true;
	}
	else
	{
		Lib::DrawSprite(sprite, positionX, positionY, k_powerupSizeX, k_powerupSizeY);
	}
}

bool Powerup::OutsideViewport()
{
	return !(k_windowWidth * 0.5f - k_windowWidth < positionX + k_powerupSizeX &&
		     k_windowWidth * 0.5f + k_windowWidth > positionX - k_powerupSizeX &&
		     k_windowHeight * 0.5f - k_windowHeight < positionY - k_powerupSizeY &&
		     k_windowHeight * 0.5f + k_windowHeight > positionY + k_powerupSizeY);
}
