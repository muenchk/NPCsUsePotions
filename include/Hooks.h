#pragma once
namespace Hooks
{
	class FastTravelConfirmHook
	{
	public:
		static void InstallHook() {
			REL::Relocation<uintptr_t> target{ REL::VariantID(52236, 0, 0), REL::VariantOffset(0x31, 0, 0) };
			auto& trampoline = SKSE::GetTrampoline();

			_FastTravelConfirmHook = trampoline.write_call<5>(target.address(), FastTravelConfirmHook);
		}

	private:
		static bool FastTravelConfirmHook(uint64_t self, uint64_t menu);
		static inline REL::Relocation<decltype(FastTravelConfirmHook)> _FastTravelConfirmHook;
	};

	void InstallHooks();
}
