# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

Void Crew is a cooperative space-horror simulation game built in C++17/20. The project is currently in the planning/early infrastructure phase (Phase 0 of a 14-phase roadmap). See [docs/01-first_iteration.md](docs/01-first_iteration.md) for the game concept and design pillars, [docs/07-roadmap.md](docs/07-roadmap.md) for the full roadmap.

The game uses a **thin-client server-authoritative architecture**: the server owns all game state and logic; the client is a rendering/audio/input terminal only. No game logic runs on the client. See [docs/02-network_pattern.md](docs/02-network_pattern.md) for details.

## Build System

See [docs/03-technology_stack.md](docs/03-technology_stack.md) for the full technology stack.

- **CMake 3.25+** with separate targets: `server`, `client`, `common` (shared static library), `tests`
- **vcpkg** (manifest mode) for dependency management via `vcpkg.json`
- **Configurations:** Debug, Release, RelWithDebInfo
- **Compilers:** MSVC (Windows), GCC & Clang (Linux), Apple Clang (macOS)

```bash
# Expected build workflow (once infrastructure exists)
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=[vcpkg-root]/scripts/buildsystems/vcpkg.cmake
cmake --build build --config Debug

# Run tests
ctest --test-dir build

# Format code
clang-format -i src/**/*.cpp src/**/*.hpp

# Static analysis
clang-tidy src/**/*.cpp
```

## Architecture

### Three-Target Structure

- **`src/server/`** - Server executable. Contains all game logic: ECS systems, simulation (atmosphere, energy, medical, culinary), AI, procedural generation, physics, networking, GameMaster tools. Server ticks at 30-60 Hz.
- **`src/client/`** - Client executable. Rendering (Vulkan + PSX-style pipeline), spatial audio (miniaudio + Steam Audio), voice chat (Opus), input capture, UI (Dear ImGui for debug, custom for game), snapshot interpolation. No game logic.
- **`src/common/`** - Shared static library. ECS component definitions, FlatBuffers network message schemas, typed identifiers, math utilities (GLM), containers, logging (spdlog).

### Key Architectural Rules

1. **Server is the single source of truth.** All state mutations happen server-side. Client sends input commands, never state changes.
2. **No game logic in client code.** Damage calculations, AI behavior, crafting rules, inventory validation - server only. These modules must not be compiled into the client binary.
3. **All client data is untrusted.** Server must validate every input: check tick ranges, clamp movement speed, verify interaction distance, confirm inventory contents.
4. **Per-role data filtering.** Server sends each client only the data their role can access. Mechanic sees reactor details but not medical stats. Hidden parameters (true O2 levels, mutation traits) may differ from displayed values.
5. **ECS components are data-only.** No methods, no logic. Systems (functions) operate on components via `entt::registry`.

### Network Model

- **Transport:** GameNetworkingSockets (UDP with reliability layer, encryption, NAT traversal)
- **Serialization:** FlatBuffers for network protocol and save files; nlohmann/json for human-readable configs; toml++ for user settings
- **Snapshots:** Server forms per-client visibility snapshots with delta compression. Area-of-interest system based on ship compartments.
- **Voice:** Opus-encoded voice routed through server for spatial positioning and effect application (radio distortion, creature voice imitation)

### Core Libraries

See [docs/04-libraries.md](docs/04-libraries.md) for the full library matrix, alternatives, and usage per target.

| Component | Library | Notes |
|-----------|---------|-------|
| ECS | EnTT | Header-only, components = data only |
| Physics | Jolt Physics | Server-side only |
| Rendering | Vulkan (+ SDL2 windowing) | PSX-style: vertex snapping, affine texturing, dithering, fog |
| Audio | miniaudio + Steam Audio | HRTF, occlusion, reverb |
| Networking | GameNetworkingSockets | UDP + reliability + encryption |
| Serialization | FlatBuffers | Zero-copy reads, versioned schemas |
| Navigation | Recast/Detour | Server-side navmesh + pathfinding |
| Parallelism | taskflow + concurrentqueue | Job graph + lock-free queues between threads |
| Logging | spdlog | Async mode, tagged per-subsystem |
| Testing | Catch2 | Unit + integration tests |
| Profiling | Tracy | Real-time, per-subsystem markers |

## Coding Conventions

See [docs/05-conventions.md](docs/05-conventions.md) for the full conventions, code examples, and code review checklist. See [docs/06-best_practices.md](docs/06-best_practices.md) for game development best practices.

### Naming

| Element | Style | Example |
|---------|-------|---------|
| Classes/structs | PascalCase | `PlayerCharacter`, `AtmosphereSystem` |
| Functions/methods | camelCase | `calculateDamage()`, `updatePosition()` |
| Variables | camelCase | `playerHealth`, `currentTick` |
| Constants | SCREAMING_SNAKE_CASE | `MAX_PLAYERS`, `DEFAULT_TICK_RATE` |
| Enum values | PascalCase | `DamageType::Sharp` |
| Namespaces | lowercase | `namespace atmosphere` |
| Files | snake_case | `player_character.cpp`, `atmosphere_system.hpp` |
| Private fields | m_ prefix | `m_health`, `m_position` |
| Static fields | s_ prefix | `s_instanceCount` |
| Template params | PascalCase | `template<typename EntityType>` |

Booleans: `isAlive`, `hasWeapon`, `canMove`, `wasProcessed`. Getters: `health()` not `getHealth()` (unless side effects). Setters: `setHealth()`. Factories: `createEntity()`, `makePacket()`. Abbreviations as words: `HttpConnection` not `HTTPConnection` (exception: two-letter like `IO`, `UI`).

### Formatting

- 4 spaces, no tabs. K&R braces (opening brace on same line). Always use braces, even for single-line blocks.
- Line limits: 100 soft, 120 hard. Wrap parameters one per line with extra indent.
- `#pragma once` for header guards.

### Include Order

1. Corresponding header (for `.cpp`)
2. _(blank line)_
3. C++ system headers (`<vector>`, `<string>`)
4. C headers (`<cstdint>`, `<cmath>`)
5. _(blank line)_
6. External libraries
7. _(blank line)_
8. Project headers

Alphabetical within each group.

### Key Idioms

- **RAII always.** No manual `new`/`delete`. Prefer `unique_ptr` > `shared_ptr` > raw pointers. Raw pointers only for non-owning references.
- **`enum class` always**, never bare `enum`.
- **`constexpr` for constants**, never `#define`.
- **Error handling:** Exceptions for invariant violations / initialization failures. `std::optional` / `std::expected` for expected failures (file not found, entity missing).
- **Logging:** Use `LOG_TRACE/DEBUG/INFO/WARN/ERROR/CRITICAL` with context: `LOG_ERROR("Failed to load texture '{}': {}", path, error.message())`.
- **TODOs must reference an issue:** `// TODO(#123): description`. No bare TODOs in main.

## Git Workflow

- Branches: `main` (stable), `develop` (integration), `feature/XXX-desc`, `fix/XXX-desc`, `refactor/desc`
- Commit format: `type(scope): description` where type is `feat`, `fix`, `refactor`, `docs`, `test`, `chore`
- PRs squash-merged to main, require CI green + one review
- CI runs: multi-platform build, tests, clang-format, clang-tidy, AddressSanitizer, UBSanitizer

## Testing

- **Framework:** Catch2
- **Unit tests:** Isolated system tests (atmosphere, medical, AI)
- **Integration tests:** Server + test client in-process, verifying protocol and state sync
- **Load tests:** Multi-client stress testing
- **Sanitizers in CI:** ASan, UBSan, TSan for multithreaded code
