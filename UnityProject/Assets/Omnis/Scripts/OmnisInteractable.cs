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
        [SerializeField, Min(0)] private int requiredRelics;

        public string DisplayName => displayName;
        public string InteractionId => interactionId;
        public int ExperienceReward => experienceReward;

        public void Configure(string id, string label, int reward, bool opensNexus, int relicRequirement = 0)
        {
            interactionId = id;
            displayName = label;
            experienceReward = Mathf.Max(0, reward);
            entersNexus = opensNexus;
            requiredRelics = Mathf.Max(0, relicRequirement);
        }

        public string PromptText
        {
            get
            {
                OmnisGameState state = OmnisGameState.Instance;
                if (state == null) return $"E · Interactuar: {displayName}";
                if (state.WasInteractionCompleted(interactionId)) return $"{displayName} · ya integrado";
                if (entersNexus && state.RelicsCollected < requiredRelics)
                    return $"{displayName} · faltan {requiredRelics - state.RelicsCollected} fragmentos";
                return $"E · Interactuar: {displayName}";
            }
        }

        public void Interact(OmnisPlayerController player)
        {
            OmnisGameState state = OmnisGameState.Instance;
            if (player == null || state == null) return;

            OmnisInteractionResult result = state.TryCompleteInteraction(
                interactionId, experienceReward, requiredRelics, entersNexus);

            switch (result)
            {
                case OmnisInteractionResult.Completed:
                    player.SetStatusMessage(entersNexus
                        ? "¡NEXO ACTIVADO! Pulsa R para iniciar el siguiente ciclo."
                        : $"{displayName} recuperado: +{experienceReward} XP");
                    Debug.Log($"[Omnis] Interaction completed: {interactionId}; cycle={state.CycleIndex}; XP={state.Experience}");
                    break;

                case OmnisInteractionResult.MissingRelics:
                    player.SetStatusMessage($"El Nexo requiere {requiredRelics} fragmentos. Encontrados: {state.RelicsCollected}/3.");
                    break;

                case OmnisInteractionResult.NexusAlreadyActive:
                    player.SetStatusMessage("El Nexo ya está activo. Pulsa R para renacer.");
                    break;

                default:
                    player.SetStatusMessage("Este recuerdo ya fue integrado en el ciclo actual.");
                    break;
            }
        }
    }
}
