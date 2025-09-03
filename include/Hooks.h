#pragma once
namespace Hooks
{
	// frame hook
	class OnFrameHook
	{
	public:
		static void Install()
		{
			//REL::Relocation<uintptr_t> target{ REL::VariantID(35564, 36563, 0x5baa00), REL::VariantOffset(0x24, 0x24, 0x27) };
			REL::Relocation<uintptr_t> target{ REL::VariantID(35564, 36563, 0x5bb1fa), REL::VariantOffset(0x24, 0x24, 0) };
			auto& trampoline = SKSE::GetTrampoline();

			switch (REL::Module::GetRuntime()) {
			case REL::Module::Runtime::AE:
				_OnFrame = trampoline.write_call<5>(target.address(), OnFrame);
				break;
			case REL::Module::Runtime::SE:
				_OnFrame = trampoline.write_call<5>(target.address(), OnFrame);
				break;
			case REL::Module::Runtime::VR:
				//_OnFrame = trampoline.write_call<6>(target.address(), OnFrame);
				_OnFrame = trampoline.write_call<5>(target.address(), OnFrame);
				break;
			}

			logger::info("Hooked Main::FrameUpdate");
		}

	private:
		static uint64_t OnFrame(void* unk);
		static inline REL::Relocation<decltype(OnFrame)> _OnFrame;
	};

	/// <summary>
	/// executed when the player uses a potion
	/// </summary>
	class PlayerUsePotionHook
	{
	public:
		static void InstallHook()
		{
			REL::Relocation<uintptr_t> target{ REL::VariantID(39604, 40690, 0x6d7b00), REL::VariantOffset(0x15, 0x15, 0x15) };
			auto& trampoline = SKSE::GetTrampoline();

			_PlayerUsePotion = trampoline.write_call<5>(target.address(), PlayerUsePotion);
		}

	private:
		static bool PlayerUsePotion(uint64_t self, RE::AlchemyItem* alch, uint64_t extralist);
		static inline REL::Relocation<decltype(PlayerUsePotion)> _PlayerUsePotion;
	};

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


	class Papyrus_FastTravelHook
	{
	private:
		static inline uint64_t remainder_1;
		static inline uint64_t remainder_2;

	public:
		static void InstallHook()
		{
			REL::Relocation<uintptr_t> targetbegin{ REL::VariantID(54824, 55457, 0), REL::VariantOffset(0x78, 0x78, 0) };
			REL::Relocation<uintptr_t> targetend{ REL::VariantID(54824, 55457, 0), REL::VariantOffset(0xD7, 0xD7, 0) };
			auto& trampoline = SKSE::GetTrampoline();

			
			struct Patch : Xbyak::CodeGenerator
			{
				Patch(uintptr_t a_remainder, uintptr_t a_fastTravelBegin)
				{
					Xbyak::Label fdec;

					mov(ptr[rsp + 0x50], rcx);
					//mov(rdi, rdx);
					//mov(rsi, rcx);
					call(ptr[rip + fdec]);

					mov(rax, qword[a_remainder]);
					jmp(rax);

					L(fdec);
					dq(a_fastTravelBegin);
				}
			};

			Patch patch{ (uintptr_t)(&remainder_1), reinterpret_cast<uintptr_t>(FastTravelBegin) };
			patch.ready();

			remainder_1 = trampoline.write_branch<5>(targetbegin.address(), trampoline.allocate(patch));
			remainder_1 = targetbegin.address() + 0x5;


			/* struct Patch_end : Xbyak::CodeGenerator
			{
				Patch_end(uintptr_t a_remainder, uintptr_t a_fastTravelBegin)
				{
					Xbyak::Label fdec;

					call(ptr[rip + fdec]);

					add(rsp, 0x70);
					pop(rbx);

					mov(rax, qword[a_remainder]);
					jmp(rax);

					L(fdec);
					dq(a_fastTravelBegin);
				}
			};

			Patch_end patch_end{ (uintptr_t)(&remainder_2), reinterpret_cast<uintptr_t>(FastTravelEnd) };
			patch_end.ready();

			remainder_2 = trampoline.write_branch<5>(targetend.address(), trampoline.allocate(patch_end));
			remainder_2 = targetend.address() + 0x5;*/
		}

	private:
		static void FastTravelBegin();

		static void FastTravelEnd();

	};

	void InstallHooks();
}


