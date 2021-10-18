#ifndef PRETENDSHIPTERMLANDINGSUICALLBACK_H_
#define PRETENDSHIPTERMLANDINGSUICALLBACK_H_

#include "server/zone/objects/player/sui/SuiCallback.h"
#include "server/zone/objects/creature/CreatureObject.h"
#include "server/zone/objects/scene/SceneObject.h"
#include "server/zone/objects/tangible/terminal/spaceship/ShipTerminalHelper.h"


class PretendShipTermLandingSuiCallback : public SuiCallback {
public:
	PretendShipTermLandingSuiCallback(ZoneServer* serv) : SuiCallback(serv) {
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

		uint64	planetID	=pListBox->getMenuObjectID(idx);
		String	starPort	=pListBox->getMenuItemName(idx);

		ShipTerminalHelper::sTravelToDest(pCreature, planetID, starPort);
	}
};

#endif /* PRETENDSHIPTERMLANDINGSUICALLBACK_H_ */
