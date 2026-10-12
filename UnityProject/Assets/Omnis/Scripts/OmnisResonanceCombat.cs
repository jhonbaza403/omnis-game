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
        public float ResonanceNormalized => Mathf.Clamp01(resonance / (float)resonanceRequired);

        private void Awake()
        {
            characterController = GetComponent<CharacterController>();
            player = GetComponent<OmnisPlayerController>();
        }

        private void Update()
        {
            if (player == null || player.IsPaused || Keyboard.current == null) return;

            if (Keyboard.current.leftCtrlKey.wasPressedThisFrame)
            {
                Vector3 direction = Vector3.zero;
                if (Keyboard.current.wKey.isPressed) direction += transform.forward;
                if (Keyboard.current.sKey.isPressed) direction -= transform.forward;
                if (Keyboard.current.dKey.isPressed) direction += transform.right;
                if (Keyboard.current.aKey.isPressed) direction -= transform.right;
                TryBeginRiftStep(direction);
            }

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

        public bool TryBeginRiftStep(Vector3 direction)
        {
            if (player == null || player.IsPaused || Time.time < nextDodgeTime) return false;
            dodgeDirection = direction.sqrMagnitude > 0.01f ? direction.normalized : transform.forward;
            dodgeStartedAt = Time.time;
            dodgeEndsAt = dodgeStartedAt + dodgeDuration;
            nextDodgeTime = Time.time + dodgeCooldown;
            dodging = true;
            player.SetStatusMessage("Paso de Grieta: evasión activa.", 0.7f);
            OmnisAudioDirector.Instance?.PlayCue(OmnisAudioCue.RiftStep);
            SpawnCombatBurst(transform.position + Vector3.up * 0.85f, new Color(0.15f, 0.9f, 1f), 24, 3.8f);
            return true;
        }

        public bool TryFracturePulse()
        {
            if (resonance < resonanceRequired)
            {
                player?.SetStatusMessage("Resonancia " + resonance + "/" + resonanceRequired + ". Encadena impactos para cargar el Pulso.", 1.4f);
                return false;
            }

            resonance = 0;
            OmnisAudioDirector.Instance?.PlayCue(OmnisAudioCue.FracturePulse);
            SpawnCombatBurst(transform.position + Vector3.up * 0.35f, new Color(0.2f, 0.75f, 1f), 72, 8f);
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
            return true;
        }

        private static void SpawnCombatBurst(Vector3 position, Color color, int count, float speed)
        {
            GameObject burstObject = new GameObject("Omnis Combat Resonance Burst");
            burstObject.transform.position = position;
            ParticleSystem particles = burstObject.AddComponent<ParticleSystem>();
            var main = particles.main;
            main.duration = 0.65f;
            main.loop = false;
            main.playOnAwake = false;
            main.startLifetime = new ParticleSystem.MinMaxCurve(0.28f, 0.9f);
            main.startSpeed = new ParticleSystem.MinMaxCurve(speed * 0.35f, speed);
            main.startSize = new ParticleSystem.MinMaxCurve(0.045f, 0.16f);
            main.startColor = color;
            main.simulationSpace = ParticleSystemSimulationSpace.World;
            main.maxParticles = Mathf.Max(32, count);
            var emission = particles.emission;
            emission.enabled = false;
            var shape = particles.shape;
            shape.enabled = true;
            shape.shapeType = ParticleSystemShapeType.Sphere;
            shape.radius = 0.18f;
            var colorOverLifetime = particles.colorOverLifetime;
            colorOverLifetime.enabled = true;
            Gradient gradient = new Gradient();
            gradient.SetKeys(
                new[] { new GradientColorKey(color, 0f), new GradientColorKey(Color.white, 0.18f), new GradientColorKey(color, 1f) },
                new[] { new GradientAlphaKey(0f, 0f), new GradientAlphaKey(0.95f, 0.12f), new GradientAlphaKey(0f, 1f) });
            colorOverLifetime.color = gradient;
            particles.Play();
            particles.Emit(count);
            Destroy(burstObject, 2f);
        }

        private void OnDrawGizmosSelected()
        {
            Gizmos.color = new Color(0.12f, 0.82f, 1f, 0.18f);
            Gizmos.DrawWireSphere(transform.position, pulseRadius);
        }
    }
}
