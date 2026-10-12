using UnityEngine;
using UnityEngine.InputSystem;
using UnityEngine.SceneManagement;

namespace Omnis
{
    [RequireComponent(typeof(CharacterController))]
    public sealed class OmnisPlayerController : MonoBehaviour
    {
        [Header("Movement")]
        [SerializeField, Min(0.1f)] private float walkSpeed = 5.2f;
        [SerializeField, Min(0.1f)] private float sprintSpeed = 8.0f;
        [SerializeField, Min(0.1f)] private float jumpHeight = 1.7f;
        [SerializeField] private float gravity = -24f;

        [Header("Camera")]
        [SerializeField] private Transform cameraPivot;
        [SerializeField] private Camera playerCamera;
        [SerializeField, Range(0.02f, 0.6f)] private float mouseSensitivity = 0.12f;
        [SerializeField] private float minPitch = -28f;
        [SerializeField] private float maxPitch = 58f;
        [SerializeField, Min(1f)] private float interactionDistance = 4.2f;

        [Header("Combat")]
        [SerializeField, Min(1)] private int maxHealth = 100;
        [SerializeField, Min(1f)] private float attackRange = 2.5f;
        [SerializeField, Min(1)] private int attackDamage = 34;
        [SerializeField, Min(0.1f)] private float attackCooldown = 0.52f;

        private CharacterController characterController;
        private float verticalVelocity;
        private float pitch;
        private float nextAttackTime;
        private float nextDamageTime;
        private int currentHealth;
        private Vector3 spawnPosition;
        private OmnisInteractable focusedInteractable;
        private string statusMessage = "Explora el Nexo. Reúne tres fragmentos.";
        private float statusMessageUntil;
        private bool isPaused;
        private bool skipMouseAttackThisFrame;

        public OmnisInteractable FocusedInteractable => focusedInteractable;
        public int CurrentHealth => currentHealth;
        public int MaxHealth => maxHealth;
        public bool IsPaused => isPaused;
        public string StatusMessage => Time.unscaledTime <= statusMessageUntil ? statusMessage : string.Empty;

        private void Awake()
        {
            characterController = GetComponent<CharacterController>();
            if (playerCamera == null) playerCamera = Camera.main;
            currentHealth = maxHealth;
            spawnPosition = transform.position;
            Time.timeScale = 1f;
        }

        public void ConfigureCamera(Transform pivot, Camera camera)
        {
            cameraPivot = pivot;
            playerCamera = camera;
        }

        private void Start()
        {
            if (Mouse.current != null)
            {
                Cursor.lockState = CursorLockMode.Locked;
                Cursor.visible = false;
            }
        }

        private void Update()
        {
            skipMouseAttackThisFrame = false;
            if (Keyboard.current != null && Keyboard.current.escapeKey.wasPressedThisFrame)
            {
                SetPaused(!isPaused);
                return;
            }
            if (isPaused) return;

            ReadLook();
            ReadMovement();
            ReadInteraction();
            ReadCombat();
            ReadCycleControl();
        }

        public void ResumeGame() => SetPaused(false);

        public void RestartGame()
        {
            Time.timeScale = 1f;
            isPaused = false;
            SceneManager.LoadScene(SceneManager.GetActiveScene().name);
        }

        public void ExitGame()
        {
            Time.timeScale = 1f;
            isPaused = false;
#if UNITY_EDITOR
            UnityEditor.EditorApplication.isPlaying = false;
#else
            Application.Quit();
#endif
        }

        private void SetPaused(bool paused)
        {
            isPaused = paused;
            Time.timeScale = paused ? 0f : 1f;
            Cursor.lockState = paused ? CursorLockMode.None : CursorLockMode.Locked;
            Cursor.visible = paused;
        }

        public void SetStatusMessage(string message, float seconds = 4.5f)
        {
            statusMessage = message ?? string.Empty;
            statusMessageUntil = Time.unscaledTime + Mathf.Max(0.2f, seconds);
        }

        public void TakeDamage(int amount)
        {
            if (amount <= 0 || Time.time < nextDamageTime || currentHealth <= 0) return;
            nextDamageTime = Time.time + 0.65f;
            OmnisResonanceCombat resonanceCombat = GetComponent<OmnisResonanceCombat>();
            if (resonanceCombat != null && resonanceCombat.IsInvulnerable) return;
            currentHealth = Mathf.Max(0, currentHealth - amount);
            OmnisAudioDirector.Instance?.PlayCue(OmnisAudioCue.PlayerDamage);

            if (currentHealth <= 0)
            {
                SetStatusMessage("Has caído. El Nexo restaura tu forma; conserva la experiencia.", 5f);
                currentHealth = maxHealth;
                verticalVelocity = 0f;
                characterController.enabled = false;
                transform.position = spawnPosition;
                characterController.enabled = true;
            }
            else
            {
                SetStatusMessage("Impacto recibido · integridad " + currentHealth + "/" + maxHealth, 1.5f);
            }
        }

        private void ReadLook()
        {
            if (Mouse.current == null) return;
            if (Cursor.lockState != CursorLockMode.Locked)
            {
                if (Mouse.current.leftButton.wasPressedThisFrame)
                {
                    Cursor.lockState = CursorLockMode.Locked;
                    Cursor.visible = false;
                    skipMouseAttackThisFrame = true;
                }
                return;
            }

            Vector2 delta = Mouse.current.delta.ReadValue() * mouseSensitivity;
            transform.Rotate(0f, delta.x, 0f, Space.World);
            pitch = Mathf.Clamp(pitch - delta.y, minPitch, maxPitch);
            if (cameraPivot != null) cameraPivot.localRotation = Quaternion.Euler(pitch, 0f, 0f);
        }

        private void ReadMovement()
        {
            if (Keyboard.current == null || characterController == null) return;

            float forward = (Keyboard.current.wKey.isPressed ? 1f : 0f) -
                            (Keyboard.current.sKey.isPressed ? 1f : 0f);
            float right = (Keyboard.current.dKey.isPressed ? 1f : 0f) -
                          (Keyboard.current.aKey.isPressed ? 1f : 0f);

            Vector3 move = transform.forward * forward + transform.right * right;
            if (move.sqrMagnitude > 1f) move.Normalize();

            bool grounded = characterController.isGrounded;
            if (grounded && verticalVelocity < 0f) verticalVelocity = -2f;
            if (grounded && Keyboard.current.spaceKey.wasPressedThisFrame)
                verticalVelocity = Mathf.Sqrt(jumpHeight * -2f * gravity);

            verticalVelocity += gravity * Time.deltaTime;
            float speed = Keyboard.current.leftShiftKey.isPressed ? sprintSpeed : walkSpeed;
            Vector3 velocity = move * speed;
            velocity.y = verticalVelocity;
            characterController.Move(velocity * Time.deltaTime);
        }

        private void ReadInteraction()
        {
            focusedInteractable = null;
            if (playerCamera == null) return;

            // Start above the CharacterController to avoid the camera ray hitting the player's own collider.
            Vector3 rayOrigin = transform.position + Vector3.up * 1.4f;
            Ray ray = new Ray(rayOrigin, playerCamera.transform.forward);
            if (Physics.Raycast(ray, out RaycastHit hit, interactionDistance,
                    Physics.DefaultRaycastLayers, QueryTriggerInteraction.Ignore))
            {
                focusedInteractable = hit.collider.GetComponentInParent<OmnisInteractable>();
            }

            if (focusedInteractable != null && Keyboard.current != null &&
                Keyboard.current.eKey.wasPressedThisFrame)
            {
                focusedInteractable.Interact(this);
            }
        }

        private void ReadCombat()
        {
            if (Keyboard.current == null || Time.time < nextAttackTime) return;
            bool keyboardAttack = Keyboard.current.fKey.wasPressedThisFrame;
            bool mouseAttack = !skipMouseAttackThisFrame &&
                               Mouse.current != null &&
                               Cursor.lockState == CursorLockMode.Locked &&
                               Mouse.current.leftButton.wasPressedThisFrame;
            if (!keyboardAttack && !mouseAttack) return;

            nextAttackTime = Time.time + attackCooldown;
            Vector3 center = transform.position + Vector3.up * 1.0f + transform.forward * 1.15f;
            Collider[] hits = Physics.OverlapSphere(center, attackRange, Physics.DefaultRaycastLayers,
                QueryTriggerInteraction.Ignore);

            OmnisSentinel selected = null;
            float nearest = float.MaxValue;
            foreach (Collider hit in hits)
            {
                OmnisSentinel sentinel = hit.GetComponentInParent<OmnisSentinel>();
                if (sentinel == null || sentinel.IsDefeated) continue;
                Vector3 direction = sentinel.transform.position - transform.position;
                direction.y = 0f;
                if (direction.sqrMagnitude > 0.01f && Vector3.Dot(transform.forward, direction.normalized) < -0.25f)
                    continue;

                float distance = direction.sqrMagnitude;
                if (distance < nearest) { nearest = distance; selected = sentinel; }
            }

            if (selected != null)
            {
                selected.TakeDamage(attackDamage);
                GetComponent<OmnisResonanceCombat>()?.RegisterMeleeHit();
                SetStatusMessage("Impacto del Errante confirmado.", 1.2f);
            }
            else if (keyboardAttack)
            {
                SetStatusMessage("No hay un centinela al alcance.", 1.2f);
            }
        }

        private void OnDisable()
        {
            if (isPaused)
            {
                Time.timeScale = 1f;
                Cursor.visible = true;
                Cursor.lockState = CursorLockMode.None;
                isPaused = false;
            }
        }

        private void ReadCycleControl()
        {
            if (Keyboard.current == null || !Keyboard.current.rKey.wasPressedThisFrame) return;
            OmnisGameState state = OmnisGameState.Instance;
            if (state == null || state.Phase != OmnisCyclePhase.Nexus)
            {
                SetStatusMessage("Primero reúne los tres fragmentos y activa el Nexo.");
                return;
            }

            state.BeginRebirth();
            Time.timeScale = 1f;
            if (!string.IsNullOrEmpty(SceneManager.GetActiveScene().name))
                SceneManager.LoadScene(SceneManager.GetActiveScene().name);
        }
    }
}
