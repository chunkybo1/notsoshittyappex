#include "Modules.h"

void Modules::Glow(BaseEntity pEntity, LocalEntity pLocal)
{
	pEntity.glowStyle = { 101,101,96,90 }; //Bright Glow, default is just outline

	if (pEntity.iTeam() != pLocal.iTeam()) 
	pEntity.Glow(255.f, 0.f, 0.f);
	else
	{
		pEntity.Glow(0.f, 255.f, 0.f);
	}
}