#pragma once
/*
 * Copyright 2010-2020 OpenXcom Developers.
 *
 * This file is part of OpenXcom.
 *
 * OpenXcom is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OpenXcom is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenXcom.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <string>
#include <vector>
#include "RuleSoldier.h"

namespace OpenXcom
{

class RuleSkill
{
	private:
		std::string _type;
		RuleItemUseCost _cost;
		RuleItemUseCost _flat;
		BattleActionType _targetMode;
		std::vector<std::string> _compatibleWeapons;
		std::vector<std::string> _requiredBonus;
		bool _isPsiRequired;
		bool _checkHandsOnly;
		int _scriptId;

	public:
		/// Default constructor.
		RuleSkill(const std::string &type);

		void load(const YAML::Node &node, Mod *mod, const ModScript &parsers);
		const std::string getType() const;
		int getScriptId() const;
		bool isPsiRequired() const;
		bool checkHandsOnly() const;
		const RuleItemUseCost getFlat() const;
		const RuleItemUseCost getCost() const;
		BattleActionType getTargetMode() const;
		const std::vector<std::string> getCompatibleWeapons() const;
		const std::vector<std::string> getRequiredBonus() const;
	
		void setScriptId(int scriptId);
		void setIsPsiRequired(bool isPsiRequired);
		void setCheckHandsOnly(bool checkHandsOnly);
		void setFlat(RuleItemUseCost flat);
		void setCost(RuleItemUseCost cost);
		void setTargetMode(BattleActionType targetMode);
		void setCompatibleWeapons(std::vector<std::string> compatibleWeapons);
		void setRequiredBonus(std::vector<std::string> requiredBonus);
};

}
