# OMNIS — AI Architecture

**Engine target:** Unreal Engine 5.8  
**Current state:** deterministic NPC life/memory foundations exist; no production LLM provider is connected.

## 1. Authoritative simulation

AI is divided into perception, navigation, behavior, NPC life state, memory, social graph and an encounter director. The server owns goals that mutate the world. NPC simulation is budgeted and distance-aware; off-screen simulation updates needs and time in bounded steps without full navigation.

NPC identity uses a stable `PersistentId`; an archetype `CharacterId` is not an individual. The saved context includes needs, relationships, a bounded memory log and the current life goal. Mission events can seed public witness memories and rumours. Rumours are deduplicated and lose strength as they spread.

## 2. Generative dialogue provider boundary

A provider is a conversational text capability, not a gameplay authority. It receives a bounded context assembled by the server:

- stable NPC identity and authored biography/occupation;
- current needs and life goal;
- the speaking player's stable account identity and relationship;
- a short list of relevant memories and recent events;
- nearby context and the current mission/dialogue topic;
- locale, allowed tone and output length.

It may return candidate dialogue text and a small enumerated conversational intent such as `Answer`, `AskQuestion`, `Refuse` or `EndConversation`. It must never return arbitrary commands, inventory changes, money, quest completion, damage, property ownership or authoritative faction mutations. Any gameplay action is separately checked by C++ rules on the server.

## 3. Provider lifecycle

The adapter contract must be asynchronous and cancellable. A request has a maximum input budget, a per-player/per-NPC rate limit, a maximum output length, a deadline and a correlation ID. Responses are validated before display. A cache key includes NPC identity, conversation topic, relevant memory version and locale; cache entries expire after world-state changes.

Provider selection is deployment configuration: local runtime model, dedicated inference service or a cloud provider may implement the same contract. Credentials are held in server secret storage, never inside assets, SaveGame, client packages or mod manifests. If no provider is available, the dialogue system uses authored fallback lines and does not block interaction.

## 4. Safety, cost and reliability

The system needs input/output moderation, prompt-injection isolation, refusal handling, output sanitization, timeouts, circuit breakers, token/cost quotas and observability. Players cannot use NPC conversation to make a model perform arbitrary tools or mutate game state. NPCs only know what their authored knowledge and recorded memories permit.

## 5. Current implementation vs remaining work

Implemented: deterministic life needs, day/week schedule, stable identity, bounded memories, relationships, witness memories, mission rumours and social spread.

Not yet implemented: a runtime LLM adapter/provider, asynchronous request cache, moderation service, generation budgets and actual in-game generated dialogue. The existing dialogue line system remains the fallback. These features must not be reported as production-ready until a provider is connected and a real Unreal 5.8 playtest passes.
