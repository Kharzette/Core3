/*
				Copyright <SWGEmu>
		See file COPYING for copying conditions.*/

#ifndef PAYMAINTENANCECOMMAND_H_
#define PAYMAINTENANCECOMMAND_H_

#include "server/zone/objects/scene/SceneObject.h"

class PaymaintenanceCommand : public QueueCommand {
public:

	PaymaintenanceCommand(const String& name, ZoneProcessServer* server)
		: QueueCommand(name, server) {

	}

	int doQueueCommand(CreatureObject* creature, const uint64& target, const UnicodeString& arguments) const {

		if (!checkStateMask(creature))
			return INVALIDSTATE;

		if (!checkInvalidLocomotions(creature))
			return INVALIDLOCOMOTION;

		int	cash	=creature->getCashCredits();
		int	bank	=creature->getBankCredits();

		if((cash + bank) <= 0)
		{
			creature->sendSystemMessage("@player_structure:no_money"); //You do not have any money to pay maintenance.
			return GENERALERROR;
		}

		ManagedReference<PlayerManager*> playerManager = server->getPlayerManager();

		uint64 targetid = creature->getTargetID();

		ManagedReference<SceneObject*> obj = playerManager->getInRangeStructureWithAdminRights(creature, targetid);

		if (obj == nullptr || !obj->isStructureObject())
			return INVALIDTARGET;

		StructureObject* structure = cast<StructureObject*>(obj.get());

		Locker clocker(structure, creature);

		ManagedReference<Zone*> zone = structure->getZone();

		if (zone == nullptr)
			return INVALIDPARAMETERS;

		if (structure->isCivicStructure()) {
			creature->sendSystemMessage("@player_structure:civic_structure_alert"); // Civic structure: Maintenance handled by city.
			return INVALIDTARGET;
		}

		if (structure->isGCWBase()) {
			return INVALIDTARGET;
		}

		StructureManager* structureManager = StructureManager::instance();

		int				amount		=0;
		bool			bBankPay	=false;
		StringBuffer	bankText;

		try {
			UnicodeTokenizer tokenizer(arguments);
			amount	=tokenizer.getIntToken();

			if(tokenizer.hasMoreTokens())
			{
				tokenizer.getStringToken(bankText);

				String	bankStr	=bankText.toString();

				bankStr.changeToUpperCase();
				bBankPay	=(bankStr == "BANK");				
			}

			if(amount > 0)
			{
				structureManager->payMaintenance(structure, creature, amount, bBankPay);
			}
		}
		catch(Exception& e)
		{
			structureManager->promptPayMaintenance(structure, creature);
		}

		return SUCCESS;
	}

};

#endif //PAYMAINTENANCECOMMAND_H_
