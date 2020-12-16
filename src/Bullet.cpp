#include "Bullet.h"
#include "lib/lib.h"
#include <d3dx9.h>

const float k_bulletSpeed = 300;

Bullet::Bullet()
{
	positionX = 0;
	positionY = 0;
	bulletDirectionModifier = 0;
	toDestroy = false;

	sprite = Lib::LoadSprite("Bullets/Bullet.png");
	color = 0;

	Lib::LoadAndPlayAudioClip("shoot.wav");
}

void Bullet::Tick()
{
	if (toDestroy) return;

	positionY += Lib::GetTimeSinceLastFrameInSeconds() * k_bulletSpeed * bulletDirectionModifier;
	
	if (OutsideViewport())
	{
 		toDestroy = true;
	}
	else
	{
		Lib::DrawSprite(sprite, positionX, positionY, k_bulletSizeX, k_bulletSizeY, 0, color);
	}
}

bool Bullet::OutsideViewport()
{
	return !(k_windowWidth * 0.5f - k_windowWidth < positionX + k_bulletSizeX &&
		     k_windowWidth * 0.5f + k_windowWidth > positionX - k_bulletSizeX &&
		     k_windowHeight * 0.5f - k_windowHeight < positionY - k_bulletSizeY &&
		     k_windowHeight * 0.5f + k_windowHeight > positionY + k_bulletSizeY);
}
