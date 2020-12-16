#include "OctopusEnemy.h"
#include "lib/lib.h"
#include <d3dx9.h>

const int k_octopusScore = 20;

OctopusEnemy::OctopusEnemy()
{
	scoreValue = k_octopusScore;
	shipSprite = Lib::LoadSprite("Enemy/OctopusEnemy.png");
}
