#include "SquidEnemy.h"
#include "lib/lib.h"
#include <d3dx9.h>

const int k_squidScore = 30;

SquidEnemy::SquidEnemy()
{
	scoreValue = k_squidScore;
	shipSprite = Lib::LoadSprite("Enemy/SquidEnemy.png");
}
