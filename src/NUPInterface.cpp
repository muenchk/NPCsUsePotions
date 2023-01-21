#pragma once

#include <NUPInterface.h>
#include <semaphore>
#include "Settings.h"
#include "Data.h"

namespace NPCsUsePotions
{
	// class NUPInterface

	NUPInterface::NUPInterface() noexcept
	{
		apiNUP = GetCurrentThreadId();
	}

	NUPInterface::~NUPInterface() noexcept
	{

	}

	void NUPInterface::EnableCureDiseasePotionUsage(bool enable)
	{
		logger::info("[NUPInterface] [EnableCureDiseasePotionUsage]");
		Settings::Internal::_useCureDiseaseEffect = enable;
	}

	std::binary_semaphore datalock{ 1 };

	NUPActorInfoHandle* NUPInterface::GetActorInfoHandle(RE::Actor* actor)
	{
		datalock.acquire();
		auto handle = NUPActorInfoHandle::Create(actor);
		datalock.release();
		return handle;
	}




	// class NUPActorInfoHandle

	NUPActorInfoHandle* NUPActorInfoHandle::Create(RE::Actor* actor)
	{
		ActorInfo* actorinfo = Data::GetSingleton()->FindActor(actor);
		auto handle = new NUPActorInfoHandle();
		actorinfo->handles.push_back(handle);
		handle->acinfo = (void*)actorinfo;
		handle->valid = true;
		return handle;
	}

	void NUPActorInfoHandle::Invalidate()
	{
		valid = false;
		ActorInfo* actorinfo = (ActorInfo*)acinfo;
		datalock.acquire();
		auto itr = actorinfo->handles.begin();
		while (itr != actorinfo->handles.end()) {
			if ((*itr) == this)
				actorinfo->handles.erase(itr);
		}
		datalock.release();
		acinfo = nullptr;
	}

	uint32_t NUPActorInfoHandle::Version()
	{
		if (valid)
			return ((ActorInfo*)acinfo)->version;
		else
			return UINT32_MAX;
	}

	bool NUPActorInfoHandle::IsBoss()
	{
		if (valid)
			return ((ActorInfo*)acinfo)->_boss;
		else
			return false;
	}

	bool NUPActorInfoHandle::IsAutomaton()
	{
		if (valid)
			return ((ActorInfo*)acinfo)->_automaton;
		else
			return false;
	}

	bool NUPActorInfoHandle::IsValid()
	{
		return valid;
	}

	int NUPActorInfoHandle::GetGlobalCooldownTimer()
	{
		if (valid)
			return ((ActorInfo*)acinfo)->globalCooldownTimer;
		else
			return INT_MAX;
	}

	NUPActorInfoHandle::ActorStrength NUPActorInfoHandle::GetActorStrength()
	{
		if (valid)
			return static_cast<NUPActorInfoHandle::ActorStrength>(static_cast<uint32_t>(((ActorInfo*)acinfo)->actorStrength));
		else
			return NUPActorInfoHandle::ActorStrength::Weak;
	}

	NUPActorInfoHandle::ItemStrength NUPActorInfoHandle::GetItemStrength()
	{
		if (valid)
			return static_cast<NUPActorInfoHandle::ItemStrength>(static_cast<uint32_t>(((ActorInfo*)acinfo)->itemStrength));
		else
			return NUPActorInfoHandle::ItemStrength::kWeak;
	}

	bool NUPActorInfoHandle::DistributedCustomItems()
	{
		if (valid)
			return ((ActorInfo*)acinfo)->_distributedCustomItems;
		else
			return true;
	}

	float NUPActorInfoHandle::GetLastDistrTime()
	{
		if (valid)
			return ((ActorInfo*)acinfo)->lastDistrTime;
		else
			return FLT_MAX;
	}

	float NUPActorInfoHandle::GetNextFoodTime()
	{
		if (valid)
			return ((ActorInfo*)acinfo)->nextFoodTime;
		else
			return FLT_MAX;
	}

	int NUPActorInfoHandle::GetDurRegeneration()
	{
		if (valid)
			return ((ActorInfo*)acinfo)->durRegeneration;
		else
			return INT_MAX;
	}

	int NUPActorInfoHandle::GetDurFortify()
	{
		if (valid)
			return ((ActorInfo*)acinfo)->durFortify;
		else
			return INT_MAX;
	}

	int NUPActorInfoHandle::GetDurStamina()
	{
		if (valid)
			return ((ActorInfo*)acinfo)->durStamina;
		else
			return INT_MAX;
	}

	int NUPActorInfoHandle::GetDurMagicka()
	{
		if (valid)
			return ((ActorInfo*)acinfo)->durMagicka;
		else
			return INT_MAX;
	}

	int NUPActorInfoHandle::GetDurHealth()
	{
		if (valid)
			return ((ActorInfo*)acinfo)->durHealth;
		else
			return INT_MAX;
	}

	std::string NUPActorInfoHandle::GetName()
	{
		if (valid)
			return ((ActorInfo*)acinfo)->name;
		else
			return "";
	}

	std::string NUPActorInfoHandle::GetPluginname()
	{
		if (valid)
			return ((ActorInfo*)acinfo)->pluginname;
		else
			return "";
	}

	RE::FormID NUPActorInfoHandle::GetFormID()
	{
		if (valid)
			return ((ActorInfo*)acinfo)->formid;
		else
			return 0;
	}

	RE::Actor* NUPActorInfoHandle::GetActor()
	{
		if (valid)
			return ((ActorInfo*)acinfo)->actor;
		else
			return nullptr;
	}

	std::vector<std::tuple<RE::TESBoundObject*, int, int8_t, std::vector<std::tuple<uint64_t, uint32_t, std::string>>, std::vector<std::tuple<uint64_t, uint32_t, std::string>>, bool>> NUPActorInfoHandle::CustomItems()
	{
		if (valid) {
			std::vector<std::tuple<RE::TESBoundObject*, int, int8_t, std::vector<std::tuple<uint64_t, uint32_t, std::string>>, std::vector<std::tuple<uint64_t, uint32_t, std::string>>, bool>> ret;
			CustomItem* cit;
			for (int i = 0; i < ((ActorInfo*)acinfo)->citems->items.size(); i++) {
				cit = ((ActorInfo*)acinfo)->citems->items[i];
				ret.push_back({ cit->object, cit->num, cit->chance, cit->conditionsall, cit->conditionsany, cit->giveonce });
			}
			return ret;
		}
		else
			return std::vector<std::tuple<RE::TESBoundObject*, int, int8_t, std::vector<std::tuple<uint64_t, uint32_t, std::string>>, std::vector<std::tuple<uint64_t, uint32_t, std::string>>, bool>>{};
	}

	std::vector<std::tuple<RE::TESBoundObject*, int, int8_t, std::vector<std::tuple<uint64_t, uint32_t, std::string>>, std::vector<std::tuple<uint64_t, uint32_t, std::string>>, bool>> NUPActorInfoHandle::CustomDeath()
	{
		if (valid) {
			std::vector<std::tuple<RE::TESBoundObject*, int, int8_t, std::vector<std::tuple<uint64_t, uint32_t, std::string>>, std::vector<std::tuple<uint64_t, uint32_t, std::string>>, bool>> ret;
			CustomItem* cit;
			for (int i = 0; i < ((ActorInfo*)acinfo)->citems->death.size(); i++) {
				cit = ((ActorInfo*)acinfo)->citems->death[i];
				ret.push_back({ cit->object, cit->num, cit->chance, cit->conditionsall, cit->conditionsany, cit->giveonce });
			}
			return ret;
		}
		else
			return std::vector<std::tuple<RE::TESBoundObject*, int, int8_t, std::vector<std::tuple<uint64_t, uint32_t, std::string>>, std::vector<std::tuple<uint64_t, uint32_t, std::string>>, bool>>{};
	}

	std::vector<std::tuple<RE::AlchemyItem*, int, int8_t, std::vector<std::tuple<uint64_t, uint32_t, std::string>>, std::vector<std::tuple<uint64_t, uint32_t, std::string>>>> NUPActorInfoHandle::CustomPotions()
	{
		if (valid) {
			std::vector<std::tuple<RE::AlchemyItem*, int, int8_t, std::vector<std::tuple<uint64_t, uint32_t, std::string>>, std::vector<std::tuple<uint64_t, uint32_t, std::string>>>> ret;
			CustomItem* cit;
			for (int i = 0; i < ((ActorInfo*)acinfo)->citems->potions.size(); i++) {
				cit = ((ActorInfo*)acinfo)->citems->potions[i];
				ret.push_back({ cit->object->As<RE::AlchemyItem>(), cit->num, cit->chance, cit->conditionsall, cit->conditionsany });
			}
			return ret;
		}
		else
			return std::vector<std::tuple<RE::AlchemyItem*, int, int8_t, std::vector<std::tuple<uint64_t, uint32_t, std::string>>, std::vector<std::tuple<uint64_t, uint32_t, std::string>>>>{};
	}

	std::vector<std::tuple<RE::AlchemyItem*, int, int8_t, std::vector<std::tuple<uint64_t, uint32_t, std::string>>, std::vector<std::tuple<uint64_t, uint32_t, std::string>>>> NUPActorInfoHandle::CustomFortify()
	{
		if (valid) {
			std::vector<std::tuple<RE::AlchemyItem*, int, int8_t, std::vector<std::tuple<uint64_t, uint32_t, std::string>>, std::vector<std::tuple<uint64_t, uint32_t, std::string>>>> ret;
			CustomItem* cit;
			for (int i = 0; i < ((ActorInfo*)acinfo)->citems->fortify.size(); i++) {
				cit = ((ActorInfo*)acinfo)->citems->fortify[i];
				ret.push_back({ cit->object->As<RE::AlchemyItem>(), cit->num, cit->chance, cit->conditionsall, cit->conditionsany });
			}
			return ret;
		}
		else
			return std::vector<std::tuple<RE::AlchemyItem*, int, int8_t, std::vector<std::tuple<uint64_t, uint32_t, std::string>>, std::vector<std::tuple<uint64_t, uint32_t, std::string>>>>{};
	}

	std::vector<std::tuple<RE::AlchemyItem*, int, int8_t, std::vector<std::tuple<uint64_t, uint32_t, std::string>>, std::vector<std::tuple<uint64_t, uint32_t, std::string>>>> NUPActorInfoHandle::CustomPoisons()
	{
		if (valid) {
			std::vector<std::tuple<RE::AlchemyItem*, int, int8_t, std::vector<std::tuple<uint64_t, uint32_t, std::string>>, std::vector<std::tuple<uint64_t, uint32_t, std::string>>>> ret;
			CustomItem* cit;
			for (int i = 0; i < ((ActorInfo*)acinfo)->citems->poisons.size(); i++) {
				cit = ((ActorInfo*)acinfo)->citems->poisons[i];
				ret.push_back({ cit->object->As<RE::AlchemyItem>(), cit->num, cit->chance, cit->conditionsall, cit->conditionsany });
			}
			return ret;
		}
		else
			return std::vector<std::tuple<RE::AlchemyItem*, int, int8_t, std::vector<std::tuple<uint64_t, uint32_t, std::string>>, std::vector<std::tuple<uint64_t, uint32_t, std::string>>>>{};
	}

	std::vector<std::tuple<RE::AlchemyItem*, int, int8_t, std::vector<std::tuple<uint64_t, uint32_t, std::string>>, std::vector<std::tuple<uint64_t, uint32_t, std::string>>>> NUPActorInfoHandle::CustomFood()
	{
		if (valid) {
			std::vector<std::tuple<RE::AlchemyItem*, int, int8_t, std::vector<std::tuple<uint64_t, uint32_t, std::string>>, std::vector<std::tuple<uint64_t, uint32_t, std::string>>>> ret;
			CustomItem* cit;
			for (int i = 0; i < ((ActorInfo*)acinfo)->citems->food.size(); i++) {
				cit = ((ActorInfo*)acinfo)->citems->food[i];
				ret.push_back({ cit->object->As<RE::AlchemyItem>(), cit->num, cit->chance, cit->conditionsall, cit->conditionsany });
			}
			return ret;
		}
		else
			return std::vector<std::tuple<RE::AlchemyItem*, int, int8_t, std::vector<std::tuple<uint64_t, uint32_t, std::string>>, std::vector<std::tuple<uint64_t, uint32_t, std::string>>>>{};
	}

	std::unordered_map<uint32_t, int> NUPActorInfoHandle::CustomItemsset()
	{
		if (valid)
			return ((ActorInfo*)acinfo)->citems->itemsset;
		else
			return std::unordered_map<uint32_t, int>{};
	}

	std::unordered_map<uint32_t, int> NUPActorInfoHandle::CustomDeathset()
	{
		if (valid)
			return ((ActorInfo*)acinfo)->citems->deathset;
		else
			return std::unordered_map<uint32_t, int>{};
	}

	std::unordered_map<uint32_t, int> NUPActorInfoHandle::CustomPotionsset()
	{
		if (valid)
			return ((ActorInfo*)acinfo)->citems->potionsset;
		else
			return std::unordered_map<uint32_t, int>{};
	}

	std::unordered_map<uint32_t, int> NUPActorInfoHandle::CustomFortifyset()
	{
		if (valid)
			return ((ActorInfo*)acinfo)->citems->fortifyset;
		else
			return std::unordered_map<uint32_t, int>{};
	}

	std::unordered_map<uint32_t, int> NUPActorInfoHandle::CustomPoisonsset()
	{
		if (valid)
			return ((ActorInfo*)acinfo)->citems->poisonsset;
		else
			return std::unordered_map<uint32_t, int>{};
	}

	std::unordered_map<uint32_t, int> NUPActorInfoHandle::CustomFoodset()
	{
		if (valid)
			return ((ActorInfo*)acinfo)->citems->foodset;
		else
			return std::unordered_map<uint32_t, int>{};
	}
}
