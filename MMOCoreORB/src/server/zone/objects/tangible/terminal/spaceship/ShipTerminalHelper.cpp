#include "server/zone/objects/tangible/terminal/spaceship/ShipTerminalHelper.h"
#include "server/zone/Zone.h"
#include "server/zone/managers/planet/PlanetTravelPoint.h"
#include "server/zone/managers/planet/PlanetManager.h"
#include "server/zone/objects/player/sui/listbox/SuiListBox.h"
#include "server/zone/objects/player/sui/SuiWindowType.h"
#include "server/zone/objects/player/sui/callbacks/PretendShipTermLandingSuiCallback.h"
#include "server/zone/objects/player/PlayerObject.h"


void	ShipTerminalHelper::sTravelToDest(CreatureObject *pPlayer, int planetIdx, String starPort)
{	
	Zone		*pZone		=pPlayer->getZone();
	ZoneServer	*pZServer	=pZone->getZoneServer();

	int	numZones	=pZServer->getZoneCount();

	if(planetIdx < 0 || planetIdx >= numZones)
	{
		//warning("Index out of range in TravelToDest()!");
		return;
	}

	Zone	*pDestZone	=pZServer->getZone(planetIdx);
	if(pDestZone == nullptr)
	{
		//This ticket's route is no longer available.
		pPlayer->sendSystemMessage("@travel:route_not_available");
		return;
	}

//	pPlayer->sendSystemMessage("DestZone: " + pDestZone->getZoneName());
//	pPlayer->sendSystemMessage("DestStarPort: " + starPort);

	//most of the below copied from the shuttle / travel ticket stuff
	PlanetManager	*pDestPM	=pDestZone->getPlanetManager();

	PlanetTravelPoint	*pPTP	=pDestPM->getPlanetTravelPoint(starPort);
	if(pPTP == nullptr)
	{
		//The ticket is not valid for the given shuttle.
		pPlayer->sendSystemMessage("@travel:wrong_shuttle");
		return;
	}

	ManagedReference<CreatureObject *>	targetShuttleObject	=pPTP->getShuttle();
	if(targetShuttleObject != nullptr)
	{
		ManagedReference<CityRegion *>	region	=targetShuttleObject->getCityRegion().get();

		if(region != nullptr)
		{
#ifdef ENABLE_CITY_TRAVEL_LIMIT
			if(region->getCurrentPlayerCount() >= MAXIMUM_PLAYER_COUNT)
			{
				pPlayer->sendSystemMessage("Your destination is currently under maintenance, please try again later.");
				return;
			}
#endif
			if(region->isBanned(pPlayer->getObjectID()))
			{
				pPlayer->sendSystemMessage("@city/city:banned_from_that_city"); // You have been banned from traveling to that city by the city militia
				return;
			}
		}
	}

	//calculate arrival position
	Coordinate	p;
	float		x, y;

	p.initializePosition(pPTP->getArrivalPosition());

	ManagedReference<CityRegion *>	region	=targetShuttleObject != nullptr ? targetShuttleObject->getCityRegion().get() : nullptr;

	//Randomize the arrival a bit to try and avoid everyone zoning on top of each other
	//For NPC cities, use the generic method
	if(region == nullptr || region->isClientRegion())
	{
		p.randomizePosition(3);

		x	=p.getPositionX();
		y	=p.getPositionY();
	}
	else
	{
		//relative orientation of the shuttle
		float	oy			=targetShuttleObject->getDirection()->getY();
		float	dirDegrees	=(acos(oy) * 180 / M_PI) * 2;

		//the proper location for arrival is along a 36 degree arc centered on the shuttle's facing axis, between 13 and 16 meters out
		dirDegrees	=dirDegrees - 18 + System::random(36);

		float	dirRadians	=dirDegrees * M_PI / 180;
		float	distance	=13 + System::random(3);

		//update the X & Y positions accordingly
		x	=p.getPositionX() + sin(dirRadians) * distance;
		y	=p.getPositionY() + cos(dirRadians) * distance;
	}

	pPlayer->switchZone(pDestZone->getZoneName(), x, p.getPositionZ(), y, 0);

	//Update the nearest mission for group waypoint for both the arrival and departure planet.
	if(pPlayer->isGrouped())
	{
		GroupObject	*pGroup	=pPlayer->getGroup();

		Locker	clocker(pGroup, pPlayer);

		pGroup->scheduleUpdateNearestMissionForGroup(pZone->getPlanetCRC());

		if(pZone != pDestZone)
		{
			pGroup->scheduleUpdateNearestMissionForGroup(pDestZone->getPlanetCRC());
		}
	}
}


void	ShipTerminalHelper::sListPlanetaryDestinations(CreatureObject *pPlayer, int planetIdx)
{
	Zone	*pZone	=pPlayer->getZone();

	ZoneServer	*pZServer	=pZone->getZoneServer();

	int	numZones	=pZServer->getZoneCount();

	if(planetIdx < 0 || planetIdx >= numZones)
	{
		//warning("Index out of range in ListPlanetaryDestinations()!");
		return;
	}

	ManagedReference<SuiListBox *>	pBox	=nullptr;

	pBox	=new SuiListBox(pPlayer, 0x00, SuiListBox::HANDLETWOBUTTON);
	pBox->setCancelButton(true, "Cancel");
	pBox->setOkButton(true, "Go!");

	Zone	*pZ	=pZServer->getZone(planetIdx);
	String	zn	=pZ->getZoneName();
	
	PlanetManager	*pPM	=pZ->getPlanetManager();

	PlanetTravelPointList	*pTPList	=pPM->getTravelPointList();

	for(int i=0;i < pTPList->size();i++)
	{
		const auto& ptp = pTPList->get(i);

		pBox->addMenuItem(ptp->getPointName(), planetIdx);
	}

	pBox->setPromptText("Choose a destination on planet " + zn);
	pBox->setPromptTitle("Landing Destination");

	pBox->setCallback(new PretendShipTermLandingSuiCallback(pZServer));

	ManagedReference<PlayerObject *>	pPO	=pPlayer->getPlayerObject();

	pPO->addSuiBox(pBox);

	pPlayer->sendMessage(pBox->generateMessage());
}
