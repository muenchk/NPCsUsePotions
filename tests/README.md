# Actor registry regression tests

These tests compile independently of Skyrim, SKSE and CommonLib:

```sh
cmake -S tests -B build-tests -DCMAKE_BUILD_TYPE=Debug
cmake --build build-tests --config Debug
ctest --test-dir build-tests -C Debug --output-on-failure
```

Use a C++20 compiler and CMake 3.22 or newer. On Windows, run from a
Visual Studio developer command prompt.

## Regression

A captured NPCs Use Potions 3.3 hang had this circular wait:

1. An engine event thread owned an actor write lock and waited to enter
   `Main::ACSetRegisterAndReset`'s registry mutex.
2. `Main::CheckActors` owned that registry mutex in `ValidateActorSets`
   and waited for the actor read lock through
   `Data::UpdateActorInfo` / `ActorInfo::UpdateMetrics` /
   `RE::Actor::IsHostileToActor`.

Two samples five seconds apart showed the same stacks, and inspecting
the locks confirmed both owners. Current source retains the same
validation lock scope; its alternate-registration setting can change
whether the event path is exercised.

The negative control models the original lock order with a timed engine
lock, so it demonstrates the blocked cycle without leaving a hung test.
The fixed-case test repeats the same interleaving 100 times and requires
the simulated engine event to finish while actor validation is pending.
CTest also enforces a 30-second overall timeout.

Additional tests cover expired references, duplicate registration,
form-ID removal, invalid versus temporarily unloaded actors, unregister
and re-register during validation, clear during validation, stale results,
exceptions, snapshot lifetime, destructor reentry and concurrent membership
changes.

The tests exercise the real `ActorRegistry` implementation with fake actor
objects. They do not execute the game, prove that engine calls are safe on
every thread, or replace in-game testing of the full plugin.
