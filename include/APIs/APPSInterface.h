#pragma once

namespace AnimatedPotionsPoisonsSKSE
{
	enum class AnimationResult
	{
		AnimationStarted = 0,
		Error = 1,
		Busy = 2,
		NoAnimationsLoaded = 3,
		AnimationNotAllowed = 4,
	};

	class APPSInterface
	{
		APPSInterface() noexcept;
		virtual ~APPSInterface() noexcept;

	public:

		static APPSInterface* GetSingleton() noexcept
		{
			static APPSInterface singleton;
			return std::addressof(singleton);
		}

		static void Init();

		/// <summary>
		/// Applies a poison to the actors [handle] weapons
		/// </summary>
		/// <param name="handle">Handle to the actor</param>
		/// <param name="poison">Poison to apply to the actors weapons</param>
		/// <param name="dosage">Dosage that should be applied</param>
		/// <returns>Whether the animation has been started (may fail nonetheless if actor is interrupted)</returns>
		virtual AnimationResult UsePoison(RE::ActorHandle handle, RE::AlchemyItem* poison, int dosage);
		/// <summary>
		/// Makes an actor use a potion
		/// </summary>
		/// <param name="handle">Handle to the actor</param>
		/// <param name="potion">Potion to use</param>
		/// <returns>Whether the potion animation has been started (may fail nonetheless if actor is interrupted)</returns>
		virtual AnimationResult UsePotion(RE::ActorHandle handle, RE::AlchemyItem* potion);

		/// <summary>
		/// Returns whether the actor is currently in an animation
		/// </summary>
		/// <param name="handle"></param>
		/// <returns></returns>
		virtual bool IsActorBusy(RE::ActorHandle handle);

		/// <summary>
		/// Returns whether mods that provide Poison animations have been registered
		/// </summary>
		/// <returns></returns>
		virtual bool IsPoisonEnabled();
		/// <summary>
		/// Returns whether mods that provide Potion animations have been registered
		/// </summary>
		/// <returns></returns>
		virtual bool IsPotionEnabled();

	private:
		unsigned long apiAPPS = 0;
	};

	
	typedef void* (*_RequestPluginAPI)();

	[[nodiscard]] inline void* RequestPluginAPI()
	{
		std::string str = "zzAnimatedPotionsPoisonsSKSE.dll";
		std::wstring wstr = std::wstring(str.begin(), str.end());
		auto pluginHandle = GetModuleHandle(wstr.c_str());
		if (pluginHandle != 0) {
			_RequestPluginAPI requestAPIFunction = (_RequestPluginAPI)GetProcAddress(pluginHandle, "RequestPluginAPI");
			if (requestAPIFunction) {
				return requestAPIFunction();
			}
		}
		return nullptr;
	}
}
