using System.Collections.Generic;
using UnityEngine;
using UnityEngine.InputSystem;

namespace Omnis
{
    /// <summary>
    /// Original Omnis combat layer: evade through a short Rift-step and spend earned Resonance
    /// on a radial Fracture Pulse. Local single-player prototype; not a multiplayer authority.
    /// </summary>
    [RequireComponent(typeof(CharacterController))]
    public sealed class OmnisResonanceCombat : MonoBehaviour
    {
        [Header("Rift-step")]
        [SerializeField, Min(1f)] private float dodgeDistance = 4.2f;
        [SerializeField, Min(0.1f)] private float dodgeCooldown = 0.9f;
        [SerializeField, Min(0.05f)] private float dodgeDuration = 0.18f;

        [Header("Fracture Pulse")]
        [SerializeField, Min(1f)] private float pulseRadius = 7.5f;
        [SerializeField, Min(1)] private int pulseDamage = 48;
        [SerializeField, Min(1)] private int resonanceRequired = 100;
        [SerializeField, Min(1)] private int resonancePerHit = 28;

        private CharacterController characterController;
        private OmnisPlayerController player;
        private int resonance;
        private float nextDodgeTime;
        private float dodgeEndsAt;
        private float dodgeStartedAt;
        private Vector3 dodgeDirection = Vector3.forward;
        private bool dodging;

        public int Resonance => resonance;
        public int ResonanceRequired => resonanceRequired;
        public bool IsInvulnerable => dodging && Time.time < dodgeEndsAt;

        private void Awake()
        {
            characterController = GetComponent<CharacterController>();
            player = GetComponent<OmnisPlayerController>();
        }

        private void Update()
        {
            if (player == null || player.IsPaused || Keyboard.current == null) return;

            if (Keyboard.current.leftCtrlKey.wasPressedThisFrame && Time.time >= nextDodgeTime)
                BeginDodge();

            if (Keyboard.current.qKey.wasPressedThisFrame)
                TryFracturePulse();

            if (dodging)
            {
                if (Time.time >= dodgeEndsAt)
                {
                    dodging = false;
                    return;
                }

                float duration = Mathf.Max(0.05f, dodgeDuration);
                float frameDistance = dodgeDistance * (Time.deltaTime / duration);
                characterController.Move(dodgeDirection * frameDistance);
            }
        }

        public void RegisterMeleeHit()
        {
            resonance = Mathf.Min(resonanceRequired, resonance + resonancePerHit);
        }

        private void BeginDodge()
        {
            Vector3 input = Vector3.zero;
            if (Keyboard.current.wKey.isPressed) input += transform.forward;
            if (Keyboard.current.sKey.isPressed) input -= transform.forward;
            if (Keyboard.current.dKey.isPressed) input += transform.right;
            if (Keyboard.current.aKey.isPressed) input -= transform.right;

            dodgeDirection = input.sqrMagnitude > 0.01f ? input.normalized : transform.forward;
            dodgeStartedAt = Time.time;
            dodgeEndsAt = dodgeStartedAt + dodgeDuration;
            nextDodgeTime = Time.time + dodgeCooldown;
            dodging = true;
            player.SetStatusMessage("Paso de Grieta: evasión activa.", 0.7f);
        }

        private void TryFracturePulse()
        {
            if (resonance < resonanceRequired)
            {
                player.SetStatusMessage("Resonancia " + resonance + "/" + resonanceRequired + ". Encadena impactos para cargar el Pulso.", 1.4f);
                return;
            }

            resonance = 0;
            Collider[] hits = Physics.OverlapSphere(transform.position, pulseRadius, Physics.DefaultRaycastLayers,
                QueryTriggerInteraction.Ignore);
            int affected = 0;
            var damagedSentinels = new HashSet<OmnisSentinel>();
            foreach (Collider hit in hits)
            {
                OmnisSentinel sentinel = hit.GetComponentInParent<OmnisSentinel>();
                if (sentinel == null || sentinel.IsDefeated || !damagedSentinels.Add(sentinel)) continue;
                sentinel.TakeDamage(pulseDamage);
                affected++;
            }

            player.SetStatusMessage(affected > 0
                ? "PULSO DE FRACTURA: " + affected + " centinela(s) alcanzado(s)."
                : "Pulso de Fractura liberado; no hay centinelas en el radio.", 1.6f);
        }

        private void OnDrawGizmosSelected()
        {
            Gizmos.color = new Color(0.12f, 0.82f, 1f, 0.18f);
            Gizmos.DrawWireSphere(transform.position, pulseRadius);
        }
    }
}
