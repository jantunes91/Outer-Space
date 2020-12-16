#include "Enemy.h"
#include "lib/lib.h"
#include <cmath>
#include <d3dx9.h>

const float k_explosionTime = 500;
const float k_explostionAssets = 5;
const float k_explosionSizeX = 32;
const float k_explosionSizeY = 32;

Enemy::Enemy()
{
	alive = true;
	positionX = 0;
	positionY = 0;
	deathTimeStamp = 0;
	scoreValue = 0;
	explosionRotation = 0;
	shipSprite = nullptr;
}

void Enemy::Draw()
{
	if (alive)
	{
		Lib::DrawSprite(shipSprite, positionX, positionY, k_enemySizeX, k_enemySizeY);
	}
	else if (Lib::GetTimeInMS() - deathTimeStamp < k_explosionTime)
	{
		unsigned int elapsedTimeSinceDeath = Lib::GetTimeInMS() - deathTimeStamp;
		if (elapsedTimeSinceDeath <= k_explosionTime)
		{
			float f = elapsedTimeSinceDeath / k_explosionTime;

			int spriteIndex = static_cast<int>(round(f * k_explostionAssets));
			IDirect3DTexture9* explosionSprite = Lib::LoadSprite("Enemy/Explosion/" + std::to_string(spriteIndex) + ".png");

			Lib::DrawSprite(explosionSprite, positionX, positionY, k_explosionSizeX, k_explosionSizeY, explosionRotation);
		}
	}
	
}

void Enemy::Die()
{
	deathTimeStamp = Lib::GetTimeInMS();
	alive = false;
	explosionRotation = Lib::GetRandomRotationInRads();
}

bool Enemy::IsAlive()
{
	return alive;
}

int Enemy::ScoreValue()
{
	return scoreValue;
}
