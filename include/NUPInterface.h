#pragma once
#include <functional>
#include <stdint.h>
#include <windows.h> 

namespace NPCsUsePotions
{
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
		

	private:

		unsigned long apiTID = 0;


		// private stuff

	};

	typedef void* (*_RequestPluginAPI)();

	[[nodiscard]] inline void* RequestPluginAPI()
	{
		auto pluginHandle = GetModuleHandle("NPCsUsePotions.dll");
		_RequestPluginAPI requestAPIFunction = (_RequestPluginAPI)GetProcAddress(pluginHandle, "RequestPluginAPI");
		if (requestAPIFunction) {
			return requestAPIFunction();
		}
		return nullptr;
	}
}
