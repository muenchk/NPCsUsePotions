#pragma once
#include "Logging.h"
namespace Hooks
{

	class ActorEquipManagerLock
	{
		struct PatchLock : Xbyak::CodeGenerator
		{
			PatchLock(uintptr_t lockAddr, uintptr_t lockThreadID, uintptr_t sleepAddr, uintptr_t getCurrentThreadID)
			{
				Xbyak::Label notMatchID;
				Xbyak::Label after;
				Xbyak::Label notexchange1;
				Xbyak::Label notexchange2;
				Xbyak::Label loopbegin;
				Xbyak::Label loopbegin2;

				mov(rax, getCurrentThreadID);
				call(rax);
				mov(esi, eax);

				mov(r14, lockAddr);
				mov(r15, lockThreadID);
				xor_(rcx, rcx);

				lfence();
				xor_(r13d, r13d);
				lea(r12d, ptr[r13 + 0x1]);
				cmp(dword[r15], esi);
				jnz(notMatchID);
				lock();
				inc(dword[r14]);
				jmp(after);

				L(notMatchID);
				mov(ebx, r13d);
				xor_(eax, eax);
				lock();
				cmpxchg(dword[r14], r12d);
				setz(cl);
				test(cl, cl);
				jnz(notexchange1);
				pause();
				xor_(eax, eax);
				lock();
				cmpxchg(dword[r14], r12d);
				setz(cl);
				test(cl, cl);
				jnz(notexchange2);
				// nop(ptr[rax + rax*0x1];

				L(loopbegin);
				inc(ebx);
				xor_(rcx, rcx);

				L(loopbegin2);
				mov(rax, sleepAddr);
				call(rax);
				xor_(eax, eax);
				lock();
				cmpxchg(dword[r14], r12d);
				setz(cl);
				test(cl, cl);
				jz(notexchange2);
				cmp(ebx, 0x2710);
				jc(loopbegin);
				mov(ebx, r12d);
				jmp(loopbegin2);

				L(notexchange2);
				lfence();

				L(notexchange1);
				mov(rax, getCurrentThreadID);
				call(rax);
				mov(dword[r15], eax);
				sfence();

				L(after);
			}
		};
		struct PatchUnlock : Xbyak::CodeGenerator
		{
			PatchUnlock(uintptr_t lockAddr, uintptr_t lockThreadID, uintptr_t sleepAddr, uintptr_t getCurrentThreadID)
			{
				Xbyak::Label end;
				Xbyak::Label decrement;

				mov(rax, getCurrentThreadID);
				call(rax);
				mov(esi, eax);

				mov(r14, lockAddr);
				mov(r15, lockThreadID);

				lfence();
				cmp(dword[r15], esi);

				jnz(end);
				cmp(dword[r14], 0x1);

				jnz(decrement);
				xor_(r13d, r13d);
				mov(dword[r15], r13d);
				mfence();
				mov(eax, r12d);
				lock();
				cmpxchg(dword[r14], r13d);
				jmp(end);

				L(decrement);
				lock();
				dec(dword[r14]);

				L(end);
			}
		};

		static inline REL::Relocation<void*()> funcLock;
		static inline REL::Relocation<void*()> funcUnlock;
	public:
		static void BuildPatches()
		{
			try {
				// sleep
				REL::Relocation<uintptr_t> sleep{ REL::VariantID(227940, 175091, 0), REL::VariantOffset(0, 0, 0) };  //
				// get current thread id
				REL::Relocation<uintptr_t> getCurrentThreadID{ REL::VariantID(227915, 175063, 0), REL::VariantOffset(0, 0, 0) };  //
				// lockthreadID
				REL::Relocation<uintptr_t> lockAddr{ REL::VariantID(517458, 403987, 0), REL::VariantOffset(0, 0, 0) };
				REL::Relocation<uintptr_t> lockThreadID{ REL::VariantID(517457, 403986, 0), REL::VariantOffset(0, 0, 0) };

				PatchLock patch{ lockAddr.get(), lockThreadID.get(), sleep.get(), getCurrentThreadID .get()};
				patch.ready();

				auto& trampoline = SKSE::GetTrampoline();
				funcLock = (uintptr_t)trampoline.allocate(patch);



				PatchUnlock patchUnlock{ lockAddr.get(), lockThreadID.get(), sleep.get(), getCurrentThreadID.get() };
				patchUnlock.ready();

				funcUnlock = (uintptr_t)trampoline.allocate(patchUnlock);
			} catch (std::exception& e) {
				logger::critical("Xbyak Error: {}", e.what());
				throw e;
			}

		}

		static void Lock()
		{
			funcLock();
		}

		static void Unlock()
		{
			funcUnlock();
		}
	};

	// functions not hooks
	class Functions
	{
	private:
		using EquipBypass_t = void*(RE::ActorEquipManager* a_manager, RE::Actor* a_actor, RE::TESBoundObject* a_object, RE::ExtraDataList** a_extraData);

	public:
		static void EquipBypass(RE::Actor* actor, RE::TESBoundObject* a_object, RE::ExtraDataList** a_extraData);
		static bool DrinkPotion(RE::Actor* actor, RE::AlchemyItem* a_alch, RE::ExtraDataList* a_extralist);
	};


	// frame hook
	class OnFrameHook
	{
	public:
		static void Install()
		{
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
	/// CURRENTLY UNUSED
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

	/// <summary>
	/// Hooks for when fast travel begins / ends
	/// </summary>
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
		/// <summary>
		/// Hook executed when the player begins fast traveling
		/// </summary>
		static void FastTravelBegin();

		/// <summary>
		/// Hook executed when the fast travel ends
		/// </summary>
		static void FastTravelEnd();

	};

	/// <summary>
	/// Installs all hooks
	/// </summary>
	void InstallHooks();
}


