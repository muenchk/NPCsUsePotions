#pragma once

#include <chrono>
#include <set>
#include <memory>

class ActorInfo;

using Window = LibImGuiUI::UserInterface::Window;
using Tab = LibImGuiUI::UserInterface::Tab;
using Texture = LibSUtils::UserInterface::Texture;

namespace NPCsUsePotions
{
	namespace UserInterface
	{
		class ActorMonitor : public Window
		{
		private:
			std::set<std::shared_ptr<ActorInfo>> _actors;
			std::chrono::steady_clock::time_point _lastActorsUpdate = std::chrono::steady_clock::now();

			void UpdateActors();

			virtual void OnWindowOpened();
			virtual void OnWindowClosed();

			/// <summary>
			/// Callback on reverting the game. Disables processing and stops all handlers
			/// </summary>
			/// <param name=""></param>
			void RevertGameCallback(SKSE::SerializationInterface* a_intfc);

		protected:
			Window::KeyInformation* _openKeys;
			Window::KeyInformation* _closeKeys;

			std::chrono::steady_clock::time_point _opened;

		public:
			static ActorMonitor* GetSingleton();
			static std::shared_ptr<ActorMonitor> GetSingletonShared();

			void Init()
			{
				if (!_openKeys)
					_openKeys = new Window::KeyInformation;
				if (!_openKeys->keys) {
					_openKeys->numKeys = 2;
					_openKeys->keys = new uint32_t[_openKeys->numKeys];
					_openKeys->keys[0] = 0x31;  // N
					_openKeys->keys[1] = 0x2A;  // LShift
					_openKeys->alternateKeyEsacape = false;
				}

				if (!_closeKeys)
					_closeKeys = new Window::KeyInformation;
				if (!_closeKeys->keys) {
					_closeKeys->numKeys = 2;
					_closeKeys->keys = new uint32_t[_closeKeys->numKeys];
					_closeKeys->keys[0] = 0x31;  // N
					_closeKeys->keys[1] = 0x2A;  // LShift
					_closeKeys->alternateKeyEsacape = true;
				}
			}

			virtual void Draw() override;

			virtual bool Open() override;
			virtual bool Close() override;

			virtual const char* GetName() override { return "NUP Actor Monitor"; }
			virtual KeyInformation* GetCloseKeys() override;
			virtual KeyInformation* GetOpenKeys() override;

			void Register();

			virtual bool IsSideWindow() { return true; }
		};
	}
}
