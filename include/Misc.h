#pragma once

namespace Misc
{
	/// <summary>
	/// Stores information about a template npc
	/// </summary>
	class NPCTPLTInfo
	{
	public:
		/// <summary>
		/// factions associated with the template npc
		/// </summary>
		std::vector<RE::TESFaction*> tpltfactions;
		/// <summary>
		/// keywords associated with the template npc
		/// </summary>
		std::vector<RE::BGSKeyword*> tpltkeywords;
		/// <summary>
		/// race of the template npc
		/// </summary>
		RE::TESRace* tpltrace = nullptr;
		/// <summary>
		/// combat style of the template npc
		/// </summary>
		RE::TESCombatStyle* tpltstyle = nullptr;
		/// <summary>
		/// class of the template npc
		/// </summary>
		RE::TESClass* tpltclass = nullptr;
		/// <summary>
		/// index of the most top-level non-temporary template
		/// </summary>
		uint32_t pluginID = 0x1;
		/// <summary>
		/// the first ActorBase from the top, with a non-runtime formid
		/// </summary>
		RE::TESNPC* base = nullptr;
		/// <summary>
		/// the first LeveledChar with non-runtime formid
		/// </summary>
		RE::TESLevCharacter* baselvl = nullptr;
	};
}
