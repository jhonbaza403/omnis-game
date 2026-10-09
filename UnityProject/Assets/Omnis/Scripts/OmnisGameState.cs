using System;
using UnityEngine;

namespace Omnis
{
    public enum OmnisCyclePhase { Awakening, Crisis, Collapse, Nexus, Rebirth }
    public enum OmnisInteractionResult { Completed, AlreadyCompleted, MissingRelics, NexusAlreadyActive }

    /// <summary>Persistent local vertical-slice progression. This is not a multiplayer authority.</summary>
    public sealed class OmnisGameState : MonoBehaviour
    {
        private const string CycleKey = "Omnis.Progress.Cycle";
        private const string ExperienceKey = "Omnis.Progress.Experience";
        private const string PhaseKey = "Omnis.Progress.Phase";

        public static OmnisGameState Instance { get; private set; }

        public int Experience { get; private set; }
        public int CycleIndex { get; private set; } = 1;
        public OmnisCyclePhase Phase { get; private set; } = OmnisCyclePhase.Awakening;
        public int RelicsCollected { get; private set; }
        public int SentinelsDefeated { get; private set; }

        public event Action StateChanged;

        private void Awake()
        {
            if (Instance != null && Instance != this)
            {
                Destroy(gameObject);
                return;
            }

            Instance = this;
            CycleIndex = Mathf.Max(1, PlayerPrefs.GetInt(CycleKey, 1));
            Experience = Mathf.Max(0, PlayerPrefs.GetInt(ExperienceKey, 0));

            if (!Enum.TryParse(PlayerPrefs.GetString(PhaseKey, OmnisCyclePhase.Awakening.ToString()), out OmnisCyclePhase savedPhase))
                savedPhase = OmnisCyclePhase.Awakening;

            // A build can close while the player is in the Nexus. Start safely in the saved phase.
            Phase = savedPhase;
            RelicsCollected = Mathf.Clamp(PlayerPrefs.GetInt(RelicCountKey(CycleIndex), 0), 0, 3);
            SentinelsDefeated = Mathf.Max(0, PlayerPrefs.GetInt(SentinelCountKey(CycleIndex), 0));
        }

        public bool WasInteractionCompleted(string interactionId)
        {
            if (string.IsNullOrWhiteSpace(interactionId)) return false;
            return PlayerPrefs.GetInt(InteractionKey(CycleIndex, interactionId), 0) == 1;
        }

        public bool WasSentinelDefeated(string sentinelId)
        {
            if (string.IsNullOrWhiteSpace(sentinelId)) return false;
            return PlayerPrefs.GetInt("Omnis.Cycle." + CycleIndex + ".Sentinel." + sentinelId, 0) == 1;
        }

        public OmnisInteractionResult TryCompleteInteraction(string interactionId, int reward, int requiredRelics, bool entersNexus)
        {
            if (string.IsNullOrWhiteSpace(interactionId)) return OmnisInteractionResult.AlreadyCompleted;
            if (entersNexus && Phase == OmnisCyclePhase.Nexus) return OmnisInteractionResult.AlreadyCompleted;
            if (Phase == OmnisCyclePhase.Nexus) return OmnisInteractionResult.NexusAlreadyActive;
            if (WasInteractionCompleted(interactionId)) return OmnisInteractionResult.AlreadyCompleted;
            if (entersNexus && RelicsCollected < requiredRelics) return OmnisInteractionResult.MissingRelics;

            PlayerPrefs.SetInt(InteractionKey(CycleIndex, interactionId), 1);
            Experience = checked(Experience + Mathf.Max(0, reward));

            if (interactionId.StartsWith("Omnis.Relic.", StringComparison.Ordinal))
            {
                RelicsCollected = Mathf.Min(3, RelicsCollected + 1);
                PlayerPrefs.SetInt(RelicCountKey(CycleIndex), RelicsCollected);
                Phase = RelicsCollected >= 2 ? OmnisCyclePhase.Collapse : OmnisCyclePhase.Crisis;
            }

            if (entersNexus) Phase = OmnisCyclePhase.Nexus;
            PersistCoreState();
            StateChanged?.Invoke();
            return OmnisInteractionResult.Completed;
        }

        public void RegisterSentinelDefeated(string sentinelId)
        {
            if (string.IsNullOrWhiteSpace(sentinelId)) return;
            string key = $"Omnis.Cycle.{CycleIndex}.Sentinel.{sentinelId}";
            if (PlayerPrefs.GetInt(key, 0) == 1) return;

            PlayerPrefs.SetInt(key, 1);
            SentinelsDefeated++;
            Experience = checked(Experience + 35);
            PlayerPrefs.SetInt(SentinelCountKey(CycleIndex), SentinelsDefeated);
            PersistCoreState();
            StateChanged?.Invoke();
        }

        public void BeginRebirth()
        {
            if (Phase != OmnisCyclePhase.Nexus) return;
            CycleIndex = checked(CycleIndex + 1);
            RelicsCollected = 0;
            SentinelsDefeated = 0;
            Phase = OmnisCyclePhase.Awakening;
            PlayerPrefs.SetInt(RelicCountKey(CycleIndex), 0);
            PlayerPrefs.SetInt(SentinelCountKey(CycleIndex), 0);
            PersistCoreState();
            StateChanged?.Invoke();
        }

        private static string InteractionKey(int cycle, string id) => $"Omnis.Cycle.{cycle}.Interaction.{id}";
        private static string RelicCountKey(int cycle) => $"Omnis.Cycle.{cycle}.Relics";
        private static string SentinelCountKey(int cycle) => $"Omnis.Cycle.{cycle}.Sentinels";

        private void PersistCoreState()
        {
            PlayerPrefs.SetInt(CycleKey, CycleIndex);
            PlayerPrefs.SetInt(ExperienceKey, Experience);
            PlayerPrefs.SetString(PhaseKey, Phase.ToString());
            PlayerPrefs.Save();
        }

        private void OnDestroy()
        {
            if (Instance == this) Instance = null;
        }
    }
}
