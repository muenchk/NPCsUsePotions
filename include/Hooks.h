#pragma once
namespace Hooks
{
	/// <summary>
	/// executed when confirming fast travel message box on world map
	/// </summary>
	class FastTravelConfirmHook
	{
	public:
		static void InstallHook() {
			REL::Relocation<uintptr_t> target{ REL::VariantID(52236, 53127, 0x91a4e0), REL::VariantOffset(0x31, 0x31, 0x31) };
			auto& trampoline = SKSE::GetTrampoline();

			_FastTravelConfirm = trampoline.write_call<5>(target.address(), FastTravelConfirm);
		}

	private:
		static bool FastTravelConfirm(uint64_t self, uint64_t menu);
		static inline REL::Relocation<decltype(FastTravelConfirm)> _FastTravelConfirm;
	};

	/// <summary>
	/// executed when fading to fast travel (from world map, not carriage)
	/// </summary>
	class FadeThenFastTravelHook
	{
	public:
		static void InstallHook()
		{
			//REL::Relocation<uintptr_t> target{ REL::VariantID(80077, 82180, 0xf20820), REL::VariantOffset(0x65, 0x62, 0x65) };
			REL::Relocation<uintptr_t> target{ REL::VariantID(52249, 53140, 0x91ad70), REL::VariantOffset(0x16, 0x16, 0x23) };
			auto& trampoline = SKSE::GetTrampoline();

			//_FadeThenFastTravel = trampoline.write_call<5>(target.address(), FadeThenFastTravel);
			_FadeThenFastTravel = trampoline.write_branch<5>(target.address(), FadeThenFastTravel);
		}

	private:
		static uint64_t FadeThenFastTravel(uint64_t arg_1, uint64_t arg_2, uint64_t arg_3, uint64_t arg_4);
		static inline REL::Relocation<decltype(FadeThenFastTravel)> _FadeThenFastTravel;
	};

	void InstallHooks();
}


