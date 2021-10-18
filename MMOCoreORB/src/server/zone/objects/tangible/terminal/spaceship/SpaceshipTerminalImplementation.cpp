/*
				Copyright <SWGEmu>
		See file COPYING for copying conditions.
*/

#include "server/zone/Zone.h"
#include "server/zone/managers/planet/PlanetManager.h"
#include "server/zone/objects/tangible/terminal/spaceship/SpaceshipTerminal.h"
#include "server/zone/objects/creature/CreatureObject.h"
#include "server/zone/objects/player/PlayerObject.h"
#include "server/zone/packets/player/EnterTicketPurchaseModeMessage.h"
#include "server/zone/objects/player/sui/listbox/SuiListBox.h"
#include "server/zone/objects/player/sui/SuiWindowType.h"
#include "server/zone/objects/player/sui/callbacks/PretendShipTermPlanetDestSuiCallback.h"


int SpaceshipTerminalImplementation::handleObjectMenuSelect(CreatureObject* player, byte selectedID)
{
	player->sendSystemMessage("... but lets pretend you have one... whoosh!");

	if(selectedID != 20)
	{
		return	0;
	}

	Reference<PlanetTravelPoint*>	ptp	=getPlanetTravelPoint();

	//Complain loudly if we failed to find a travel point for this terminal
	if(ptp == nullptr)
	{
		error("SpaceshipTerminalImplementation::handleObjectMenuSelect(" + String::valueOf(getObjectID()) + " Could not determine related PlanetTravelPoint");
		return	0;
	}

	ManagedReference<SuiListBox *>	pBox	=nullptr;

	pBox	=new SuiListBox(player, 0x00, SuiListBox::HANDLETWOBUTTON);
	pBox->setCancelButton(true, "Cancel");
	pBox->setOkButton(true, "Choose");

	Zone	*pZone	=player->getZone();

	ZoneServer	*pZServer	=pZone->getZoneServer();

	int	numZones	=pZServer->getZoneCount();

	for(int i=0;i < numZones;i++)
	{
		Zone	*pZ	=pZServer->getZone(i);

		PlanetManager	*pMan	=pZ->getPlanetManager();
		if(pMan == nullptr)
		{
			continue;	//space?
		}

		PlanetTravelPointList	*ptpl	=pMan->getTravelPointList();
		if(ptpl == nullptr)
		{
			continue;	//no starports?  or space?
		}

		if(ptpl->size() <= 0)
		{
			continue;	//no travel points
		}

		String	zn	=pZ->getZoneName();

		pBox->addMenuItem(zn, pZ->_getObjectID());
	}

	pBox->setPromptText("Choose a planet to fly to:");
	pBox->setPromptTitle("Planetary Destination");

	pBox->setCallback(new PretendShipTermPlanetDestSuiCallback(pZServer));

	ManagedReference<PlayerObject *>	pPlayer	=player->getPlayerObject();

	pPlayer->addSuiBox(pBox);

	player->sendMessage(pBox->generateMessage());

	return	0;
}


void SpaceshipTerminalImplementation::notifyInsertToZone(Zone* zone)
{
	TerminalImplementation::notifyInsertToZone(zone);
}
