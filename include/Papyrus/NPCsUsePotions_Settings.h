#pragma once

namespace Papyrus
{
	namespace SettingsAPI
	{
		/// <summary>
		/// Registers papyrus functions
		/// </summary>
		/// <param name="a_vm"></param>
		void Register(RE::BSScript::Internal::VirtualMachine* a_vm);

		/*
		bool Dummy(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*);

		void Dummy(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, bool enabled);

		float Dummy(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*);

		void Dummy(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, float value);

		int Dummy(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*);

		void Dummy(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, int value);
		*/

		// general

		/// <summary>
		/// Returns the maximum duration that is accounted for for restoration potions in milliseconds
		/// </summary>
		/// <param name="a_vm"></param>
		/// <param name="a_stackID"></param>
		/// <param name=""></param>
		/// <returns></returns>
		int Get_MaxDuration(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*);

		/// <summary>
		/// Sets the maximum duration that is accounted for for restoration potions in miliseconds
		/// </summary>
		/// <param name="a_vm"></param>
		/// <param name="a_stackID"></param>
		/// <param name=""></param>
		/// <param name="milliseconds">the maximum duration that is accounted for for restoration potions</param>
		void Set_MaxDuration(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, int milliseconds);

		/// <summary>
		/// Returns the maximum duration that is accounted for for fortification potions in milliseconds
		/// </summary>
		/// <param name="a_vm"></param>
		/// <param name="a_stackID"></param>
		/// <param name=""></param>
		/// <returns></returns>
		int Get_MaxFortifyDuration(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*);

		/// <summary>
		/// Sets the maximum duration that is accounted for for fortification potions in miliseconds
		/// </summary>
		/// <param name="a_vm"></param>
		/// <param name="a_stackID"></param>
		/// <param name=""></param>
		/// <param name="milliseconds">the maximum duration that is accounted for for restoration potions</param>
		void Set_MaxFortifyDuration(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, int milliseconds);

		/// <summary>
		/// Returns the name of the given alchemic effects base value
		/// </summary>
		/// <param name="a_vm"></param>
		/// <param name="a_stackID"></param>
		/// <param name=""></param>
		/// <param name="value"></param>
		/// <returns></returns>
		RE::BSFixedString ToStringAlchemicEffect(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, int value);

		// System
		namespace System
		{
			/// <summary>
			/// Returns the current time between cycles in milliseconds
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <returns></returns>
			int Get_CycleTime(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*);

			/// <summary>
			/// Sets the time between cycles [if the value is not inside an acceptable range it will be adjusted]
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <param name="milliseconds">the time between cycles in milliseconds</param>
			void Set_CycleTime(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, int milliseconds);
		}

		// Usage
		namespace Usage
		{
			/// <summary>
			/// Returns the global cooldown applied to potion, poison, and food usage in milliseconds
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <returns></returns>
			int Get_GlobalCooldown(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*);

			/// <summary>
			/// Sets the global cooldown applied to potion, poison, and food usage
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <param name="milliseconds"></param>
			void Set_GlobalCooldown(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, int milliseconds);

			/// <summary>
			/// Returns the applied global cooldown for potions, after compatibility considerations
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <param name="milliseconds"></param>
			int Get_EffectiveGlobalCooldownPotions(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*);

			/// <summary>
			/// Returns the applied global cooldown for poisons, after compatibility considerations
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <param name="milliseconds"></param>
			int Get_EffectiveGlobalCooldownPoisons(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*);

			/// <summary>
			/// Returns the applied global cooldown for food, after compatibility considerations
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <param name="milliseconds"></param>
			int Get_EffectiveGlobalCooldownFood(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*);

			/// <summary>
			/// Returns wether item usage is disabled while an npc is staggered
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			bool Get_DisableItemUsageWhileStaggered(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*);

			/// <summary>
			/// Sets whether item usahe is disabled while an npc is staggered
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <param name="disable"></param>
			void Set_DisableItemUsageWhileStaggered(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, bool disabled);

			/// <summary>
			/// Returns wether item usage is disabled while an npc is flying
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			bool Get_DisableItemUsageWhileFlying(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*);

			/// <summary>
			/// Sets whether item usahe is disabled while an npc is flying
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <param name="disable"></param>
			void Set_DisableItemUsageWhileFlying(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, bool disabled);

			/// <summary>
			/// Returns wether item usage is disabled while an npc is bleeding out
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			bool Get_DisableItemUsageWhileBleedingOut(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*);

			/// <summary>
			/// Sets whether item usahe is disabled while an npc is bleeding out
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <param name="disable"></param>
			void Set_DisableItemUsageWhileBleedingOut(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, bool disabled);

			/// <summary>
			/// Returns wether item usage is disabled while an npc is sleeping
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			bool Get_DisableItemUsageWhileSleeping(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*);

			/// <summary>
			/// Sets whether item usahe is disabled while an npc is sleeping
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <param name="disable"></param>
			void Set_DisableItemUsageWhileSleeping(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, bool disabled);

			/// <summary>
			/// Returns whether processing of NPCs that are not active followers of the player is disabled
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <returns></returns>
			bool Get_DisableNonFollowerNPCs(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*);

			/// <summary>
			/// Sets whether processing of NPCs that are not active followers of the player is disabled
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <param name="disable"></param>
			void Set_DisableNonFollowerNPCs(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, bool disable);

			/// <summary>
			/// Returns whether out-of-combat processing has been disabled
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <returns></returns>
			bool Get_DisableOutOfCombatProcessing(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*);

			/// <summary>
			/// Sets whether out-of-combat processing is disabled
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <param name="disabled"></param>
			void Set_DisableOutOfCombatProcessing(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, bool disabled);

			/// <summary>
			/// Returns whether item usage is disabled for npcs that are excluded from distribution
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <returns></returns>
			bool Get_DisableItemUsageForExcludedNPCs(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*);

			/// <summary>
			/// Sets whether item usage is disabled for npcs that are excluded from distribution
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <param name="disabled"></param>
			void Set_DisableItemUsageForExcludedNPCs(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, bool disabled);
		}

		// Potions
		namespace Potions
		{
			/// <summary>
			/// Returns whether the suage of magicka potions is enabled
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <returns></returns>
			bool Get_Potions_EnableMagickaRestoration(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*);

			/// <summary>
			/// Sets whether the usage of magicka potions is enabled
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <param name="enabled"></param>
			void Set_Potions_EnableMagickaRestoration(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, bool enabled);

			/// <summary>
			/// Returns whether the usage of stamina potions is enabled
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <returns></returns>
			bool Get_Potions_EnableStaminaRestoration(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*);

			/// <summary>
			/// Sets whether the usage of stamina potions is enabled
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <param name="enabled"></param>
			void Set_Potions_EnableStaminaRestoration(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, bool enabled);

			/// <summary>
			/// Returns whether the usage of health potions is enabled
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <returns></returns>
			bool Get_Potions_EnableHealthRestoration(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*);

			/// <summary>
			/// Sets whether the usage of health potions is enabled
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <param name="enabled"></param>
			void Set_Potions_EnableHealthRestoration(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, bool enabled);

			/// <summary>
			/// Returns whether the usage of potions with negative effects is allowed
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <returns></returns>
			bool Get_AllowDetrimentalEffects(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*);

			/// <summary>
			/// Sets whether the usage of potions with negative effects is allowed
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <param name="allow"></param>
			void Set_AllowDetrimentalEffects(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, bool allow);

			/// <summary>
			/// Returns whether NPCs that have their weapons sheathed should be treated as out-of-combat
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <returns></returns>
			bool Get_HandleWeaponSheathedAsOutOfCombat(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*);

			/// <summary>
			/// Sets whether NPCs that have their weapons sheathed should be treated as out-of-combat
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <param name="enabled"></param>
			void Set_HandleWeaponSheathedAsOutOfCombat(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, bool enabled);

			/// <summary>
			/// Returns the threshold for the usage of health potions
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <returns></returns>
			float Get_HealthThreshold(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*);

			/// <summary>
			/// Sets the threshold for the usage of health potions
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <param name="value"></param>
			void Set_HealthThreshold(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, float value);

			/// <summary>
			/// Returns the threshold for the usage of magicka potions
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <returns></returns>
			float Get_MagickaThreshold(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*);

			/// <summary>
			/// Sets the threshold for the usage of magicka potions
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <param name="value"></param>
			void Set_MagickaThreshold(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, float value);

			/// <summary>
			/// Returns the threshold for the usage of stamina potions
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <returns></returns>
			float Get_StaminaThreshold(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*);

			/// <summary>
			/// Sets the threshold for the usage of stamina potions
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <param name="value"></param>
			void Set_StaminaThreshold(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, float value);

			/// <summary>
			/// Returns the chance that an NPC will use a potion if they are able to
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <returns></returns>
			int Get_UsePotionChance(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*);

			/// <summary>
			/// Sets the chance that an NPC will use a potion if they are able to
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <param name="value"></param>
			void Set_UsePotionChance(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, int value);

			/// <summary>
			/// Returns whether the given AlchemicEffect has been prohibited
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <param name="value"></param>
			/// <returns></returns>
			bool IsEffectProhibited(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, int value);

			/// <summary>
			/// Inverts the current prohibition status of the given alchemic effect
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <param name="value"></param>
			void InvertEffectProhibited(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, int value);
		}

		namespace Poisons
		{
			/// <summary>
			/// Returns whether the usage of poison is enabled
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <returns></returns>
			bool Get_EnablePoisons(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*);

			/// <summary>
			/// Sets whether the usage of poisons is enabled
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <param name="enabled"></param>
			void Set_EnablePoisons(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, bool enabled);

			/// <summary>
			/// Returns whether poisons with positive effects may be used
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <returns></returns>
			bool Get_AllowPositiveEffects(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*);

			/// <summary>
			/// Sets whther poisons with positive effects may be used
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <param name="enabled"></param>
			void Set_AllowPositiveEffects(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, bool allow);

			/// <summary>
			/// Returns whether the usage of poisons is disabled while weapons are sheathed
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <returns></returns>
			bool Get_DontUseWithWeaponsSheathed(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*);

			/// <summary>
			/// Sets whether the usage of poisons is disabled while weapons are sheathed
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <param name="enabled"></param>
			void Set_DontUseWithWeaponsSheathed(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, bool disabled);

			/// <summary>
			/// Returns whether NPCs should not use poisons when facing an enemy with 100 poison resistance
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <returns></returns>
			bool Get_DontUseAgainst100PoisonResist(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*);

			/// <summary>
			/// Sets whether NPCs should not use poisons when facing an enemy with 100 poison resistance
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <param name="disabled"></param>
			void Set_DontUseAgainst100PoisonResist(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, bool disabled);

			/// <summary>
			/// Returns the level scale of enemies in regard to the player, at which the player begins using poisons [or relationship with EnemyNumberThreshold]
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <returns></returns>
			float Get_EnemyLevelScalePlayerLevel(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*);

			/// <summary>
			/// Sets the level scale of enemies in regard to the player, at which the player begins using poisons [or relationship with EnemyNumberThreshold]
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <param name="value"></param>
			void Set_EnemyLevelScalePLayerLevel(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, float value);

			/// <summary>
			/// Returns the number of enemies that have to participate in a fight, for the player to start using poisons [or relationship with EnemyLevelScalePlayerLevel]
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <returns></returns>
			int Get_EnemyNumberThreshold(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*);

			/// <summary>
			/// Sets the number of enemies that have to participate in a fight, for the player to start using poisons [or relationship with EnemyLevelScalePlayerLevel]
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <param name="value"></param>
			void Set_EnemyNumberThreshold(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, int value);

			/// <summary>
			/// Returns the chance that an NPC will use a poison if they can
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <returns></returns>
			int Get_UsePoisonChance(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*);

			/// <summary>
			/// Sets the chance that an NPC will use a poison if they can
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <param name="value"></param>
			void Set_UsePoisonChance(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, int value);

			/// <summary>
			/// Returns the base dosage applied to all poisons
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <returns></returns>
			int Get_Dosage(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*);

			/// <summary>
			/// Sets the base dosage applied to all poisons
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <param name="value"></param>
			void Set_Dosage(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, int value);

			/// <summary>
			/// Returns whether the given AlchemicEffect has been prohibited
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <param name="value"></param>
			/// <returns></returns>
			bool IsEffectProhibited(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, int value);

			/// <summary>
			/// Inverts the current prohibition status of the given alchemic effect
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <param name="value"></param>
			void InvertEffectProhibited(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, int value);
		}

		namespace FortifyPotions
		{
			/// <summary>
			/// Returns whether the usage of fortify potions is enabled
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <returns></returns>
			bool Get_EnableFortifyPotions(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*);

			/// <summary>
			/// Sets whether the usage of fortify potions is enabled
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <param name="enabled"></param>
			void Set_EnableFortifyPotions(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, bool enabled);

			/// <summary>
			/// Returns whether fortification potions whould not be used when weapons are sheathed
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <returns></returns>
			bool Get_DontUseWithWeaponsSheathed(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*);

			/// <summary>
			/// Sets whether fortification potions should not be used when weapons are sheathed
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <param name="disabled"></param>
			void Set_DontUseWithWeaponsSheathed(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, bool disabled);

			/// <summary>
			/// Returns the level scale of enemies in regard to the player, at which the player begins using fortification potions [or relationship with EnemyNumberThreshold]
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <returns></returns>
			float Get_EnemyLevelScalePlayerLevelFortify(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*);

			/// <summary>
			/// Sets the level scale of enemies in regard to the player, at which the player begins using fortification potions [or relationship with EnemyNumberThreshold]
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <param name="value"></param>
			void Set_EnemyLevelScalePlayerLevelFortify(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, float value);

			/// <summary>
			/// Returns the number of enemies that have to participate in a fight, for the player to start using fortification potions [or relationship with EnemyLevelScalePlayerLevel]
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <returns></returns>
			int Get_EnemyNumberThresholdFortify(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*);

			/// <summary>
			/// Sets the number of enemies that have to participate in a fight, for the player to start using fortification potions [or relationship with EnemyLevelScalePlayerLevel]
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <param name="value"></param>
			void Set_EnemyNumberThresholdFortify(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, int value);

			/// <summary>
			/// Returns the chance for an NPC to use a fortification potion when they may do so
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <returns></returns>
			int Get_UseFortifyPotionChance(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*);

			/// <summary>
			/// Sets the chance for an NPC to use a fortification potion when they may do so
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <param name="value"></param>
			void Set_UseFortifyPotionChance(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, int value);
		}

		namespace Food
		{
			/// <summary>
			/// Returns whether the usage of food is enabled
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <returns></returns>
			bool Get_EnableFood(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*);

			/// <summary>
			/// Sets whether the usage of food is enabled
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <param name="enabled"></param>
			void Set_EnableFood(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, bool enabled);

			/// <summary>
			/// Returns whether the usage of food with detrimental effects is allowed
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <returns></returns>
			bool Get_AllowDetrimentalEffects(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*);

			/// <summary>
			/// Sets whether the usage of food with detrimental effects is allowed
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <param name="allow"></param>
			void Set_AllowDetrimentalEffects(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, bool allow);

			/// <summary>
			/// Returns whether to restrict food usage to the beginning of a fight
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <returns></returns>
			bool Get_RestrictFoodToCombatStart(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*);

			/// <summary>
			/// Sets whether to restrict food usage to the beginning of a fight
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <param name="enabled"></param>
			void Set_RestrictFoodToCombatStart(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, bool enabled);

			/// <summary>
			/// Returns whether food usage is disabled for followers
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <returns></returns>
			bool Get_DisableFollowers(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*);

			/// <summary>
			/// Sets whether food usage is disabled for followers
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <param name="disabled"></param>
			void Set_DisableFollowers(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, bool disabled);

			/// <summary>
			/// Returns whether food should not be used while weapons are sheathed
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <returns></returns>
			bool Get_DontUseWithWeaponsSheathed(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*);

			/// <summary>
			/// Sets whether food should not be used while weapons are sheathed
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <param name="disabled"></param>
			void Set_DontUseWithWeaponsSheathed(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, bool disabled);

			/// <summary>
			/// Returns whether the given AlchemicEffect has been prohibited
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <param name="value"></param>
			/// <returns></returns>
			bool IsEffectProhibited(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, int value);

			/// <summary>
			/// Inverts the current prohibition status of the given alchemic effect
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <param name="value"></param>
			void InvertEffectProhibited(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, int value);
		}

		namespace Player
		{
			/// <summary>
			/// Returns whether player potion usage is enabled
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <returns></returns>
			bool Get_PlayerPotions(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*);

			/// <summary>
			/// Sets whether player potion usage is enabled
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <param name="enabled"></param>
			void Set_PlayerPotions(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, bool enabled);

			/// <summary>
			/// Returns whether player poison usage is enabled
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <returns></returns>
			bool Get_PlayerPoisons(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*);

			/// <summary>
			/// Sets whether player poison usage is enabled
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <param name="enabled"></param>
			void Set_PlayerPoisons(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, bool enabled);

			/// <summary>
			/// Returns whether player fortify potion usage is enabled
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <returns></returns>
			bool Get_PlayerFortifyPotions(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*);

			/// <summary>
			/// Sets whether player fortify potion usage is enabled
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <param name="enabled"></param>
			void Set_PlayerFortifyPotions(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, bool enabled);

			/// <summary>
			/// Returns whether player food usage is enabled
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <returns></returns>
			bool Get_PlayerFood(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*);

			/// <summary>
			/// Sets whether player food usage is enabled
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <param name="enabled"></param>
			void Set_PlayerFood(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, bool enabled);

			/// <summary>
			/// Returns whether only favorite items may be used by the player
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <returns></returns>
			bool Get_UseFavoritedItemsOnly(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*);

			/// <summary>
			/// Sets whether only favorite items may be used by the player
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <param name="enabled"></param>
			void Set_UseFavoritedItemsOnly(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, bool enabled);

			/// <summary>
			/// Returns whether favorite items should not be used by player
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <returns></returns>
			bool Get_DontUseFavoritedItems(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*);

			/// <summary>
			/// Sets whther favorite items should not be used by player
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <param name="disabled"></param>
			void Set_DontUseFavoritedItems(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, bool disabled);

			/// <summary>
			/// Returns whether player won't eat raw food
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <returns></returns>
			bool Get_DontEatRawFood(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*);

			/// <summary>
			/// Sets whether player won't eat raw food
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <param name="disabled"></param>
			void Set_DontEatRawFood(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, bool disabled);

			/// <summary>
			/// Returns whether the player won't drink alcohol
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <returns></returns>
			bool Get_DontDrinkAlcohol(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*);

			/// <summary>
			/// Sets whether the player won't drink alcohol
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <param name="disabled"></param>
			void Set_DontDrinkAlcohol(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, bool disabled);
		}

		namespace Distribution
		{
			/// <summary>
			/// Returns whether poison distribution is enabled
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <returns></returns>
			bool Get_DistributePoisons(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*);

			/// <summary>
			/// Sets whether poison distribution is enabled
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <param name="enabled"></param>
			void Set_DistributePoisons(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, bool enabled);

			/// <summary>
			/// Returns whether potion distribution is enabled
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <returns></returns>
			bool Get_DistributePotions(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*);

			/// <summary>
			/// Sets whether potion distribution is enabled
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <param name="enabled"></param>
			void Set_DistributePotions(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, bool enabled);

			/// <summary>
			/// Returns whether fortify potion distribution is enabled
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <returns></returns>
			bool Get_DistributeFortifyPotions(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*);

			/// <summary>
			/// Sets whether fortify potion distribution is enabled
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <param name="enabled"></param>
			void Set_DistributeFortifyPotions(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, bool enabled);

			/// <summary>
			/// Returns whether food distribution is enabled
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <returns></returns>
			bool Get_DistributeFood(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*);

			/// <summary>
			/// Sets whether food distribution is enabled
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <param name="enabled"></param>
			void Set_DistributeFood(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, bool enabled);

			/// <summary>
			/// Returns whether CustomItem distribution is enabled
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <returns></returns>
			bool Get_DistributeCustomItems(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*);

			/// <summary>
			/// Sets whether CustomItem distribution is enabled
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <param name="enabled"></param>
			void Set_DistributeCustomItems(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, bool enabled);

			/// <summary>
			/// Returns the maximum level for easy NPCs
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <returns></returns>
			int Get_LevelEasy(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*);

			/// <summary>
			/// Sets the maximum level for easy NPCs
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <param name="value"></param>
			void Set_LevelEasy(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, int value);

			/// <summary>
			/// Returns the maximum level for normal NPCs
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <returns></returns>
			int Get_LevelNormal(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*);

			/// <summary>
			/// Sets the maximum level for normal NPCs
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <param name="value"></param>
			void Set_LevelNormal(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, int value);

			/// <summary>
			/// Returns the maximum level for difficult NPCs
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <returns></returns>
			int Get_LevelDifficult(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*);

			/// <summary>
			/// Sets the maximum level for difficult NPCs
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <param name="value"></param>
			void Set_LevelDifficult(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, int value);

			/// <summary>
			/// Returns the maximum level for Insane NPCs
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <returns></returns>
			int Get_LevelInsane(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*);

			/// <summary>
			/// Sets the maximum level for Insane NPCs, NPCs above this level are considered boss NPCs
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <param name="value"></param>
			void Set_LevelInsane(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, int value);

			/// <summary>
			/// Returns whether NPC difficulty is tied to Game difficulty
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <returns></returns>
			bool Get_GameDifficultyScaling(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*);

			/// <summary>
			/// Sets whether NPC difficulty is tied to Game difficulty
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <param name="enabled"></param>
			void Set_GameDifficultyScaling(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, bool enabled);

			/// <summary>
			/// Returns the maximum magnitude for weak potions
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <returns></returns>
			int Get_MaxMagnitudeWeak(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*);

			/// <summary>
			/// Sets the maximum magnitude for weak potions
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <param name="value"></param>
			void Set_MaxMagnitudeWeak(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, int value);

			/// <summary>
			/// Returns the maximum magnitude for standard potions
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <returns></returns>
			int Get_MaxMagnitudeStandard(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*);

			/// <summary>
			/// Sets the maximum magnitude for standard potions
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <param name="value"></param>
			void Set_MaxMagnitudeStandard(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, int value);

			/// <summary>
			/// Returns the maximum magnitude for potent potions
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <returns></returns>
			int Get_MaxMagnitudePotent(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*);

			/// <summary>
			/// Sets the maximum magnitude for potent potions
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <param name="value"></param>
			void Set_MaxMagnitudePotent(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, int value);

			/// <summary>
			/// Returns the scale of the distribution probability for items matching the combat styles primary combat modifiers
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <returns></returns>
			float Get_StyleScalingPrimary(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*);

			/// <summary>
			/// Sets the scale of the distribution probability for items matching the combat styles primary combat modifiers
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <param name="value"></param>
			void Set_StyleScalingPrimary(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, float value);

			/// <summary>
			/// Retruns the scale of the distribution probability for items matching the combat styles secondary combat modifiers
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <returns></returns>
			float Get_StyleScalingSecondary(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*);

			/// <summary>
			/// Sets the scale of the distribution probability for items matching the combat styles secondary combat modifiers
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <param name="value"></param>
			void Set_StyleScalingSecondary(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, float value);

			/// <summary>
			/// Returns the general probability scaling
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <returns></returns>
			float Get_ProbabilityScaling(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*);

			/// <summary>
			/// Sets the general probability scaling
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <param name=""></param>
			void Set_ProbabilityScaling(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, float value);

			/// <summary>
			/// Returns the general probability scaling
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <returns></returns>
			bool Get_DoNotDistributeMixedInvisPotions(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*);

			/// <summary>
			/// Sets the general probability scaling
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <param name=""></param>
			void Set_DoNotDistributeMixedInvisPotions(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, bool value);

			float Get_ProbabilityAdjuster(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, int effect, int itemtype);
			void Set_ProbabilityAdjuster(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, int effect, int itemtype, float value);
		}

		namespace Removal
		{
			/// <summary>
			/// Returns whether alchemy items are removed from NPCs inventory on death
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <returns></returns>
			bool Get_RemoveItemsOnDeath(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*);

			/// <summary>
			/// Sets whether alchemy items are removed from NPCs inventory on death
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <param name="enabled"></param>
			void Set_RemoveItemsOnDeath(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, bool enabled);

			/// <summary>
			/// Returns the chance for an item to be removed [calculated for all items individually]
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <returns></returns>
			int Get_ChanceToRemoveItem(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*);

			/// <summary>
			/// Sets the chance for an item to be removed [calculated for all items individually]
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <param name="value"></param>
			void Set_ChanceToRemoveItem(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, int value);

			/// <summary>
			/// Returns the maximum amount of items that may remain in NPCs inventory
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <returns></returns>
			int Get_MaxItemsLeft(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*);

			/// <summary>
			/// Sets the maximum amount of items that may remain in NPCs inventory
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <param name="value"></param>
			void Set_MaxItemsLeft(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, int value);
		}

		namespace Whitelist
		{
			/// <summary>
			/// Returns whether whitelist is enabled for Items [not whitelisted items will not be distributed and used]
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <returns></returns>
			bool Get_EnabledItems(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*);

			/// <summary>
			/// Sets whether whitelist is enabled for items [not whitelisted items will not be distributed and used]
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <param name="enabled"></param>
			void Set_EnabledItems(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, bool enabled);

			/// <summary>
			/// Returns whether whitelist is enabled for NPCs [not whitelisted NPCs will not get and use items]
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <returns></returns>
			bool Get_EnabledNPCs(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*);

			/// <summary>
			/// Sets whether whitelist is enabled for NPCs [not whitelisted NPCs will not get and use items]
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <param name="enabled"></param>
			void Set_EnabledNPCs(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, bool enabled);
		}

		namespace Fixes
		{
			/// <summary>
			/// Returns whether Skill Boost Perks are distributed to all elligible NPCs
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <returns></returns>
			bool Get_ApplySkillBoostPerks(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*);

			/// <summary>
			/// Sets whether Skill Boost Perks are distributed to all elligible NPCs [requires Restart]
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <param name="enabled"></param>
			void Set_ApplySkillBosstPerks(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, bool enabled);

			/// <summary>
			/// Returns whether the sounds of potions, poisons, and food will be fixed by modifying the Records on game load
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <returns></returns>
			bool Get_ForceFixPotionSounds(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*);

			/// <summary>
			/// Sets whether the sounds of potions, poisons, and food will be fixed by modifying the Records on game load [requires Restart]
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <param name="enabled"></param>
			void Set_ForceFixPotionSounds(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, bool enabled);
		}

		namespace Compatibility
		{
			/// <summary>
			/// Returns whether all npcs that are in the ActorTypeCreature and/or ActorTypeAnimal faction and do not have a dedicated rule are removed from item distribution, item usage, and perk distribution
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <returns></returns>
			bool Get_DisableCreaturesWithoutRules(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*);

			/// <summary>
			/// Sets whether all npcs that are in the ActorTypeCreature and/or ActorTypeAnimal faction and do not have a dedicated rule are removed from item distrubtion, item usage, and perk distribution
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <param name="enabled"></param>
			void Set_DisableCreaturesWithoutRules(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, bool enabled);

			/// <summary>
			/// Returns whether Animated Poisons animations should be used by NPCs
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <returns></returns>
			bool Get_AnimatedPoisons_Enabled(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*);

			/// <summary>
			/// Sets whether Animated Poisons animations should be used by NPCs
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <param name="enabled"></param>
			void Set_AnimatedPoisons_Enabled(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, bool enabled);

			/// <summary>
			/// Returns whether the poison dosage system used by Animated Poisons should be preferred over the built-in system [removed in Animated Poisons 5]
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <returns></returns>
			bool Get_AnimatedPoisons_UsePoisonDosage(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*);

			/// <summary>
			/// Sets whether the poison dosage system used by Animated Poisons should be preferred over the built-in system [removed in Animated Poisons 5]
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <param name="enabled"></param>
			void Set_AnimatedPoisons_UsePoisonDosage(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, bool enabled);

			/// <summary>
			/// Returns whether Animated Potions animations should be used by NPCs
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <returns></returns>
			bool Get_AnimatedPotions_Enabled(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*);

			/// <summary>
			/// Sets whether Animated Potions animations should be used by NPCs
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <param name="enabled"></param>
			void Set_AnimatedPotions_Enable(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, bool enabled);


			/// <summary>
			/// Returns whether CACO is loaded
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <returns></returns>
			bool CACO_Loaded(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*);

			/// <summary>
			/// Returns whether Apothecary is loaded
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <returns></returns>
			bool Apothecary_Loaded(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*);

			/// <summary>
			/// Returns whether Animated Poisons is loaded
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <returns></returns>
			bool AnimatedPoisons_Loaded(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*);

			/// <summary>
			/// Returns whether Animated Potions is loaded
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <returns></returns>
			bool AnimatedPotions_Loaded(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*);

			/// <summary>
			/// Returns whether ZUPA is loaded
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <returns></returns>
			bool ZUPA_Loaded(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*);

			/// <summary>
			/// Returns whetehr Sacrosanct is loaded
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <returns></returns>
			bool Sacrosanct_Loaded(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*);

			/// <summary>
			/// Returns whether Ultimate Potions [AE] is loaded
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <returns></returns>
			bool UltimatePotions_Loaded(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*);
		}

		namespace Debug
		{
			/// <summary>
			/// Returns whether logging is enabled
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <returns></returns>
			bool Get_EnableLog(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*);

			/// <summary>
			/// Sets whether logging is enabled
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <param name="enabled"></param>
			void Set_EnableLog(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, bool enabled);

			/// <summary>
			/// Returns whether logging during plugin load is enabled
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <returns></returns>
			bool Get_EnableLoadLog(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*);

			/// <summary>
			/// Sets whether logging during plugin load is enabled
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <param name="enabled"></param>
			void Set_EnableLoadLog(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, bool enabled);

			/// <summary>
			/// Returns the detail of logging
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <returns></returns>
			int Get_LogLevel(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*);

			/// <summary>
			/// Sets the detail of logging
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <param name="value"></param>
			void Set_LogLevel(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, int value);

			/// <summary>
			/// Returns the detail of profiling
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <returns></returns>
			int Get_ProfileLevel(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*);

			/// <summary>
			/// Sets the detail of profiling
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <param name="value"></param>
			void Set_ProfileLevel(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, int value);

			/// <summary>
			/// Returns whether profiling is enabled
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <returns></returns>
			bool Get_EnableProfiling(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*);

			/// <summary>
			/// Sets whether profiling is enabled
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <param name="enabled"></param>
			void Set_EnableProfiling(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, bool enabled);
		}

		namespace Statistics
		{
			/// <summary>
			/// Returns the number of actors saved in the last savegame
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <returns></returns>
			long Get_ActorsSaved(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*);

			/// <summary>
			/// Returns the number of actors read from the last savegame
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <returns></returns>
			long Get_ActorsRead(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*);

			/// <summary>
			/// Returns the number of potions used this game session
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <returns></returns>
			long Get_PotionsUsed(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*);

			/// <summary>
			/// Returns the number of poisons used this game session
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <returns></returns>
			long Get_PoisonsUsed(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*);

			/// <summary>
			/// Returns the number of food eaten this game session
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <returns></returns>
			long Get_FoodUsed(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*);

			/// <summary>
			/// Returns the number of game events handled this game session
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <returns></returns>
			long Get_EventsHandled(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*);

			/// <summary>
			/// Returns the number of actors that have been handled this game session
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <returns></returns>
			long Get_ActorsHandledTotal(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*);

			/// <summary>
			/// Returns the number of actors handled in the last cycle
			/// </summary>
			/// <param name="a_vm"></param>
			/// <param name="a_stackID"></param>
			/// <param name=""></param>
			/// <returns></returns>
			long Get_ActorsHandled(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*);
		}
	}
}
