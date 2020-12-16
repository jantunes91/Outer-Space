#include "CrabEnemy.h"
#include "lib/lib.h"
#include <d3dx9.h>

const int k_crabScore = 10;

CrabEnemy::CrabEnemy()
{
	scoreValue = k_crabScore;
	shipSprite = Lib::LoadSprite("Enemy/CrabEnemy.png");
}
