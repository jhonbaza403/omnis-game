using UnityEngine;

namespace Omnis
{
    [RequireComponent(typeof(Collider))]
    public sealed class OmnisInteractable : MonoBehaviour
    {
        [SerializeField] private string interactionId = "Omnis.Nexus.Obelisk";
        [SerializeField] private string displayName = "Obelisco del Nexo";
        [SerializeField, Min(0)] private int experienceReward = 25;
        [SerializeField] private bool entersNexus = true;

        public string DisplayName => displayName;
        public string InteractionId => interactionId;

        public void Interact(OmnisPlayerController player)
        {
            if (player == null || OmnisGameState.Instance == null) return;
            OmnisGameState.Instance.GrantExperience(experienceReward);
            if (entersNexus) OmnisGameState.Instance.EnterNexus();
            Debug.Log($"[Omnis] Interacción '{interactionId}' completada; +{experienceReward} XP.");
        }
    }
}
