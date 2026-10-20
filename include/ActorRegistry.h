#pragma once

#include <cstdint>
#include <map>
#include <memory>
#include <mutex>
#include <set>
#include <vector>

// Protect membership only. In particular, never call into Skyrim while holding
// this mutex: an engine event can arrive with an actor write lock already held.
template <class Actor>
class ActorRegistry
{
public:
	using Strong = std::shared_ptr<Actor>;
	using Weak = std::weak_ptr<Actor>;
	using WeakSet = std::set<Weak, std::owner_less<Weak>>;
	using StrongSet = std::set<Strong, std::owner_less<Strong>>;

	bool contains(const Strong& actor) const
	{
		std::lock_guard lock(mutex);
		return entries.contains(actor);
	}

	bool insert(const Strong& actor, std::uint32_t formID)
	{
		if (!actor)
			return false;
		std::lock_guard lock(mutex);
		if (entries.contains(actor))
			return false;
		entries.emplace(actor, Registration{ ++nextGeneration, formID });
		return true;
	}

	void erase(const Strong& actor)
	{
		std::lock_guard lock(mutex);
		entries.erase(actor);
	}

	void erase(std::uint32_t formID)
	{
		std::lock_guard lock(mutex);
		for (auto it = entries.begin(); it != entries.end();) {
			if (it->second.formID == formID || it->first.expired())
				it = entries.erase(it);
			else
				++it;
		}
	}

	std::size_t size() const
	{
		std::lock_guard lock(mutex);
		return entries.size();
	}

	void clear()
	{
		std::lock_guard lock(mutex);
		entries.clear();
		// Do not reset generations: an outstanding snapshot must stay stale.
	}

	StrongSet copy()
	{
		StrongSet result;
		for (const auto& item : snapshot())
			result.insert(item.actor);
		return result;
	}

	// Validator returns false to discard an invalid actor. Selector optionally
	// excludes a valid actor from this cycle without unregistering it.
	template <class Validator, class Selector>
	WeakSet validate(Validator&& validateActor, Selector&& selectActor)
	{
		WeakSet result;
		auto items = snapshot();
		for (auto& item : items) {
			item.valid = validateActor(item.actor);
			item.selected = item.valid && selectActor(item.actor);
		}
		std::lock_guard lock(mutex);
		for (const auto& item : items) {
			auto current = entries.find(item.actor);
			if (current == entries.end() || current->second.generation != item.generation)
				continue;
			if (!item.valid)
				entries.erase(current);
			else if (item.selected)
				result.insert(item.actor);
		}
		return result;
	}

private:
	struct Registration
	{
		std::uint64_t generation;
		std::uint32_t formID;
	};
	struct SnapshotEntry
	{
		Strong actor;
		std::uint64_t generation;
		bool valid = false;
		bool selected = false;
	};

	std::vector<SnapshotEntry> snapshot()
	{
		std::vector<SnapshotEntry> result;
		std::lock_guard lock(mutex);
		result.reserve(entries.size());
		for (auto it = entries.begin(); it != entries.end();) {
			if (auto actor = it->first.lock()) {
				result.push_back({ std::move(actor), it->second.generation });
				++it;
			} else {
				it = entries.erase(it);
			}
		}
		return result;
	}

	mutable std::mutex mutex;
	std::map<Weak, Registration, std::owner_less<Weak>> entries;
	std::uint64_t nextGeneration = 0;
};
