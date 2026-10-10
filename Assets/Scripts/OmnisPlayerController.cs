using UnityEngine;
using UnityEngine.InputSystem;

namespace Omnis
{
    [RequireComponent(typeof(CharacterController))]
    public sealed class OmnisPlayerController : MonoBehaviour
    {
        [Header("Movement")]
        [SerializeField, Min(0.1f)] private float moveSpeed = 7f;
        [SerializeField, Min(0.1f)] private float sprintMultiplier = 1.55f;
        [SerializeField, Min(0.1f)] private float turnSpeed = 12f;
        [SerializeField, Min(0.1f)] private float jumpHeight = 1.6f;
        [SerializeField] private float gravity = -22f;

        private CharacterController controller;
        private Vector3 velocity;
        private Camera playerCamera;

        private void Awake()
        {
            controller = GetComponent<CharacterController>();
            playerCamera = Camera.main;
        }

        private void Update()
        {
            if (playerCamera == null) playerCamera = Camera.main;

            Vector2 rawInput = Vector2.zero;
            if (Keyboard.current != null)
            {
                if (Keyboard.current.aKey.isPressed || Keyboard.current.leftArrowKey.isPressed) rawInput.x -= 1f;
                if (Keyboard.current.dKey.isPressed || Keyboard.current.rightArrowKey.isPressed) rawInput.x += 1f;
                if (Keyboard.current.sKey.isPressed || Keyboard.current.downArrowKey.isPressed) rawInput.y -= 1f;
                if (Keyboard.current.wKey.isPressed || Keyboard.current.upArrowKey.isPressed) rawInput.y += 1f;
            }
            if (Gamepad.current != null)
                rawInput += Gamepad.current.leftStick.ReadValue();

            Vector2 input = Vector2.ClampMagnitude(rawInput, 1f);
            Vector3 cameraForward = playerCamera != null ? playerCamera.transform.forward : Vector3.forward;
            Vector3 cameraRight = playerCamera != null ? playerCamera.transform.right : Vector3.right;
            cameraForward.y = 0f;
            cameraRight.y = 0f;
            cameraForward.Normalize();
            cameraRight.Normalize();
            Vector3 direction = cameraForward * input.y + cameraRight * input.x;

            if (direction.sqrMagnitude > 0.001f)
            {
                Quaternion targetRotation = Quaternion.LookRotation(direction, Vector3.up);
                transform.rotation = Quaternion.Slerp(transform.rotation, targetRotation, turnSpeed * Time.deltaTime);
            }

            bool sprinting = (Keyboard.current != null &&
                (Keyboard.current.leftShiftKey.isPressed || Keyboard.current.rightShiftKey.isPressed)) ||
                (Gamepad.current != null && Gamepad.current.leftStickButton.isPressed);
            float speed = moveSpeed * (sprinting ? sprintMultiplier : 1f);
            controller.Move(direction * speed * Time.deltaTime);

            if (controller.isGrounded && velocity.y < 0f) velocity.y = -2f;
            bool jumpPressed = (Keyboard.current != null && Keyboard.current.spaceKey.wasPressedThisFrame) ||
                (Gamepad.current != null && Gamepad.current.buttonSouth.wasPressedThisFrame);
            if (controller.isGrounded && jumpPressed)
                velocity.y = Mathf.Sqrt(jumpHeight * -2f * gravity);

            velocity.y += gravity * Time.deltaTime;
            controller.Move(velocity * Time.deltaTime);

            if (transform.position.y < -10f) ResetToStart();
        }

        public void ResetToStart()
        {
            controller.enabled = false;
            transform.position = new Vector3(0f, 1.2f, -8f);
            transform.rotation = Quaternion.identity;
            velocity = Vector3.zero;
            controller.enabled = true;
        }
    }
}
