using UnityEngine;
using UnityEngine.InputSystem;

namespace Omnis
{
    [RequireComponent(typeof(CharacterController))]
    public sealed class OmnisPlayerController : MonoBehaviour
    {
        [Header("Movement")]
        [SerializeField] private float walkSpeed = 5f;
        [SerializeField] private float sprintSpeed = 7.5f;
        [SerializeField] private float jumpHeight = 1.6f;
        [SerializeField] private float gravity = -22f;
        [Header("Camera")]
        [SerializeField] private Transform cameraPivot;
        [SerializeField] private Camera playerCamera;
        [SerializeField] private float mouseSensitivity = 0.12f;
        [SerializeField] private float minPitch = -25f;
        [SerializeField] private float maxPitch = 55f;
        [Header("Interaction")]
        [SerializeField] private float interactionDistance = 3f;

        private CharacterController characterController;
        private float verticalVelocity;
        private float pitch;
        private OmnisInteractable focusedInteractable;
        public OmnisInteractable FocusedInteractable => focusedInteractable;

        private void Awake()
        {
            characterController = GetComponent<CharacterController>();
            if (playerCamera == null) playerCamera = Camera.main;
        }

        private void Start()
        {
            if (Mouse.current != null) Cursor.lockState = CursorLockMode.Locked;
        }

        private void Update()
        {
            ReadLook();
            ReadMovement();
            ReadInteraction();
        }

        private void ReadLook()
        {
            if (Mouse.current == null) return;
            if (Keyboard.current != null && Keyboard.current.escapeKey.wasPressedThisFrame)
                Cursor.lockState = CursorLockMode.None;
            if (Mouse.current.leftButton.wasPressedThisFrame)
                Cursor.lockState = CursorLockMode.Locked;
            if (Cursor.lockState != CursorLockMode.Locked) return;

            Vector2 delta = Mouse.current.delta.ReadValue() * mouseSensitivity;
            transform.Rotate(0f, delta.x, 0f, Space.World);
            pitch = Mathf.Clamp(pitch - delta.y, minPitch, maxPitch);
            if (cameraPivot != null) cameraPivot.localRotation = Quaternion.Euler(pitch, 0f, 0f);
        }

        private void ReadMovement()
        {
            if (Keyboard.current == null) return;
            float forward = (Keyboard.current.wKey.isPressed ? 1f : 0f) - (Keyboard.current.sKey.isPressed ? 1f : 0f);
            float right = (Keyboard.current.dKey.isPressed ? 1f : 0f) - (Keyboard.current.aKey.isPressed ? 1f : 0f);
            Vector3 move = transform.forward * forward + transform.right * right;
            if (move.sqrMagnitude > 1f) move.Normalize();

            bool grounded = characterController.isGrounded;
            if (grounded && verticalVelocity < 0f) verticalVelocity = -2f;
            if (grounded && Keyboard.current.spaceKey.wasPressedThisFrame)
                verticalVelocity = Mathf.Sqrt(jumpHeight * -2f * gravity);
            verticalVelocity += gravity * Time.deltaTime;

            Vector3 velocity = move * (Keyboard.current.leftShiftKey.isPressed ? sprintSpeed : walkSpeed);
            velocity.y = verticalVelocity;
            characterController.Move(velocity * Time.deltaTime);
        }

        private void ReadInteraction()
        {
            focusedInteractable = null;
            if (playerCamera == null) return;
            Ray ray = new Ray(playerCamera.transform.position, playerCamera.transform.forward);
            if (Physics.Raycast(ray, out RaycastHit hit, interactionDistance,
                    Physics.DefaultRaycastLayers, QueryTriggerInteraction.Ignore))
                focusedInteractable = hit.collider.GetComponentInParent<OmnisInteractable>();

            if (focusedInteractable != null && Keyboard.current != null &&
                Keyboard.current.eKey.wasPressedThisFrame)
                focusedInteractable.Interact(this);
        }
    }
}
