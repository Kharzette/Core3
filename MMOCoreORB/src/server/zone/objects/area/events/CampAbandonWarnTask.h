/*
				Copyright <SWGEmu>
		See file COPYING for copying conditions.*/

#ifndef CAMPABANDONWARNTASK_H_
#define CAMPABANDONWARNTASK_H_

#include "server/zone/objects/area/CampSiteActiveArea.h"

namespace server {
namespace zone {
namespace objects {
namespace area {
namespace events {

class CampAbandonWarnTask: public Task {
	ManagedReference<CampSiteActiveArea*> campSite;


public:
	CampAbandonWarnTask(CampSiteActiveArea* camp) {
		campSite = camp;
	}

	void run() {
		if (campSite == nullptr)
			return;

		Locker locker(campSite);
		campSite->warnAbandonCamp();
	}
};


}
}
}
}
}

#endif /* CAMPABANDONWARNTASK_H_ */
