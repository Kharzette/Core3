#ifndef SHIPTERMINALHELPER_H_
#define SHIPTERMINALHELPER_H_

#include "server/zone/objects/creature/CreatureObject.h"


class ShipTerminalHelper
{
	public:
	static void	sTravelToDest(CreatureObject *pPlayer, int planetIdx, String starPort);
	static void sListPlanetaryDestinations(CreatureObject *pPlayer, int planetIdx);
};

#endif	//SHIPTERMINALHELPER_H_