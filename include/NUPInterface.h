#pragma once
#include <functional>
#include <stdint.h>
#include <windows.h> 

namespace NPCsUsePotions
{
	/// <summary>
	/// Storage for information about a loaded actor
	/// object must be destroyed in the receiver
	/// </summary>
	class NUPActorInfoHandle
	{
	private:
		bool valid = false;
		void* acinfo;

	public:
		/// <summary>
		/// Determines the strength of an Item
		/// </summary>
		enum class ItemStrength
		{
			kWeak = 1,
			kStandard = 2,
			kPotent = 3,
			kInsane = 4
		};
		/// <summary>
		/// Determines the strength of an Actor
		/// </summary>
		enum class ActorStrength
		{
			Weak = 0,
			Normal = 1,
			Powerful = 2,
			Insane = 3,
			Boss = 4,
		};

		/// <summary>
		/// returns custom items that may be distributed to the actor on combat enter
		/// </summary>
		std::vector<std::tuple<RE::TESBoundObject*, int, int8_t, uint64_t, uint64_t, bool>> CustomItems();
		std::unordered_map<uint32_t, int> CustomItemsset();
		/// <summary>
		/// returns custom items that may be distributed to the actor upon their death
		/// </summary>
		std::vector<std::tuple<RE::TESBoundObject*, int, int8_t, uint64_t, uint64_t, bool>> CustomDeath();
		std::unordered_map<uint32_t, int> CustomDeathset();
		/// <summary>
		/// returns custom potions that may be distributed to the actor
		/// </summary>
		std::vector<std::tuple<RE::AlchemyItem*, int, int8_t, uint64_t, uint64_t>> CustomPotions();
		std::unordered_map<uint32_t, int> CustomPotionsset();
		/// <summary>
		/// returns custom fortify potions that may be distributed to the actor
		/// </summary>
		std::vector<std::tuple<RE::AlchemyItem*, int, int8_t, uint64_t, uint64_t>> CustomFortify();
		std::unordered_map<uint32_t, int> CustomFortifyset();
		/// <summary>
		/// returns custom poisons that may be distributed to the actor
		/// </summary>
		std::vector<std::tuple<RE::AlchemyItem*, int, int8_t, uint64_t, uint64_t>> CustomPoisons();
		std::unordered_map<uint32_t, int> CustomPoisonsset();
		/// <summary>
		/// returns custom food that may be distributed to the actor
		/// </summary>
		std::vector<std::tuple<RE::AlchemyItem*, int, int8_t, uint64_t, uint64_t>> CustomFood();
		std::unordered_map<uint32_t, int> CustomFoodset();

		/// <summary>
		/// The actor
		/// </summary>
		RE::Actor* GetActor();
		/// <summary>
		/// form id of the actor
		/// </summary>
		RE::FormID GetFormID();
		/// <summary>
		/// pluginname the actor is defined in
		/// </summary>
		std::string GetPluginname();
		/// <summary>
		/// name of the actor
		/// </summary>
		std::string GetName();
		/// <summary>
		/// Current remaining cooldown on health potions
		/// </summary>
		int GetDurHealth();
		/// <summary>
		/// Current remaining cooldown on magicka potions
		/// </summary>
		int GetDurMagicka();
		/// <summary>
		/// Current remaining cooldown on stamina potions
		/// </summary>
		int GetDurStamina();
		/// <summary>
		/// Current remaining cooldown on fortify potions
		/// </summary>
		int GetDurFortify();
		/// <summary>
		/// Current remaining cooldown on regeneration potions
		/// </summary>
		int GetDurRegeneration();
		/// <summary>
		/// time when the actor may use the next food item -> compare with RE::Calendar::GetSingleton()->GetDaysPassed();
		/// </summary>
		float GetNextFoodTime();
		/// <summary>
		/// Time the npc was last given items
		/// </summary>
		float GetLastDistrTime();

		/// <summary>
		/// whether custom items have already been given to the npc
		/// </summary>
		bool DistributedCustomItems();

		ActorStrength GetActorStrength();
		ItemStrength GetItemStrength();

		bool IsBoss();
		bool IsAutomaton();

		bool IsValid();

		uint32_t Version();

		NUPActorInfoHandle()
		{
		}

		~NUPActorInfoHandle()
		{
			Invalidate();
		}

		void Invalidate();

		static NUPActorInfoHandle* Create(RE::Actor* actor);
	};

	class NUPInterface
	{
	public:

		NUPInterface() noexcept;
		virtual ~NUPInterface() noexcept;

		static NUPInterface* GetSingleton() noexcept
		{
			static NUPInterface singleton;
			return std::addressof(singleton);
		}

		// Interface functions
		
		/// <summary>
		/// Enables / Disables the usage of cure disease potions by NPCsUsePotions.
		/// </summary>
		/// <param name="enable">Whether the usage of cure disease potions should be enabled</param>
		/// <returns></returns>
		virtual void EnableCureDiseasePotionUsage(bool enable);

		virtual NUPActorInfoHandle* GetActorInfoHandle(RE::Actor* actor);
		

	private:

		unsigned long apiNUP = 0;


		// private stuff

	};

	typedef void* (*_RequestPluginAPI)();

	[[nodiscard]] inline void* RequestPluginAPI()
	{
		auto pluginHandle = GetModuleHandle("NPCsUsePotions.dll");
		if (pluginHandle != 0) {
			_RequestPluginAPI requestAPIFunction = (_RequestPluginAPI)GetProcAddress(pluginHandle, "RequestPluginAPI");
			if (requestAPIFunction) {
				return requestAPIFunction();
			}
		}
		return nullptr;
	}
}
