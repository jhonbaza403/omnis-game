using System;
using UnityEngine;

namespace Omnis
{
    public enum OmnisCyclePhase { Awakening, Crisis, Collapse, Nexus, Rebirth }

    /// <summary>Local prototype state only; no persistence or multiplayer authority implied.</summary>
    public sealed class OmnisGameState : MonoBehaviour
    {
        public static OmnisGameState Instance { get; private set; }
        public int Experience { get; private set; }
        public int CycleIndex { get; private set; } = 1;
        public OmnisCyclePhase Phase { get; private set; } = OmnisCyclePhase.Awakening;
        public event Action StateChanged;

        private void Awake()
        {
            if (Instance != null && Instance != this) { Destroy(gameObject); return; }
            Instance = this;
        }

        public void GrantExperience(int amount)
        {
            if (amount <= 0) return;
            Experience = checked(Experience + amount);
            StateChanged?.Invoke();
        }

        public void EnterNexus() => SetPhase(OmnisCyclePhase.Nexus);

        public void BeginRebirth()
        {
            CycleIndex = checked(CycleIndex + 1);
            SetPhase(OmnisCyclePhase.Rebirth);
        }

        private void SetPhase(OmnisCyclePhase phase)
        {
            if (Phase == phase) return;
            Phase = phase;
            StateChanged?.Invoke();
        }
    }
}
