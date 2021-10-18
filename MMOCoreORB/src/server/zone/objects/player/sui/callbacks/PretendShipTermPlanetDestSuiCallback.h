#ifndef PRETENDSHIPTERMPLANETDESTSUICALLBACK_H_
#define PRETENDSHIPTERMPLANETDESTSUICALLBACK_H_

#include "server/zone/objects/player/sui/SuiCallback.h"
#include "server/zone/objects/creature/CreatureObject.h"
#include "server/zone/objects/scene/SceneObject.h"
#include "server/zone/objects/tangible/terminal/spaceship/ShipTerminalHelper.h"


class PretendShipTermPlanetDestSuiCallback : public SuiCallback
{
public:
	PretendShipTermPlanetDestSuiCallback(ZoneServer* serv) : SuiCallback(serv)
	{
	}


	void run(CreatureObject *pCreature, SuiBox *pSui, uint32 eventIdx, Vector<UnicodeString> *pArgs)
    {
		bool    bCancel =(eventIdx == 1);

		if(!pSui->isListBox() || bCancel || pArgs->size() < 1)
		{
			return;
		}

		int	idx	=Integer::valueOf(pArgs->get(0).toString());

		SuiListBox	*pListBox	=cast<SuiListBox *>(pSui);

		if(idx < 0 || idx >= pListBox->getMenuSize())
		{
			return;
		}

		int	planetIndex	=pListBox->getMenuObjectID(idx);

		ShipTerminalHelper::sListPlanetaryDestinations(pCreature, planetIndex);
	}
};

#endif /* PRETENDSHIPTERMPLANETDESTSUICALLBACK_H_ */
