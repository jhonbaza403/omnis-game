using UnityEngine;

namespace Omnis
{
    [RequireComponent(typeof(CharacterController))]
    public sealed class OmnisPlayerController : MonoBehaviour
    {
        [SerializeField] private float moveSpeed = 7f;
        [SerializeField] private float turnSpeed = 12f;
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
            Vector3 input = new Vector3(Input.GetAxisRaw("Horizontal"), 0f, Input.GetAxisRaw("Vertical"));
            input = Vector3.ClampMagnitude(input, 1f);
            Vector3 cameraForward = playerCamera != null ? playerCamera.transform.forward : Vector3.forward;
            Vector3 cameraRight = playerCamera != null ? playerCamera.transform.right : Vector3.right;
            cameraForward.y = 0f;
            cameraRight.y = 0f;
            cameraForward.Normalize();
            cameraRight.Normalize();
            Vector3 direction = cameraForward * input.z + cameraRight * input.x;

            if (direction.sqrMagnitude > 0.001f)
            {
                Quaternion targetRotation = Quaternion.LookRotation(direction, Vector3.up);
                transform.rotation = Quaternion.Slerp(transform.rotation, targetRotation, turnSpeed * Time.deltaTime);
            }

            controller.Move(direction * moveSpeed * Time.deltaTime);
            if (controller.isGrounded && velocity.y < 0f) velocity.y = -2f;
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
