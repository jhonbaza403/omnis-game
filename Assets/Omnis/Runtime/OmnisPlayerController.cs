using UnityEngine;

[RequireComponent(typeof(CharacterController))]
public sealed class OmnisPlayerController : MonoBehaviour
{
    public float moveSpeed = 4f;
    public float gravity = -18f;
    public Transform cameraPivot;
    float verticalVelocity;

    void Update()
    {
        var input = new Vector2(Input.GetAxisRaw("Horizontal"), Input.GetAxisRaw("Vertical"));
        var move = (transform.right * input.x + transform.forward * input.y).normalized;
        var velocity = move * moveSpeed;
        if (GetComponent<CharacterController>().isGrounded && verticalVelocity < 0) verticalVelocity = -2f;
        verticalVelocity += gravity * Time.deltaTime;
        velocity.y = verticalVelocity;
        GetComponent<CharacterController>().Move(velocity * Time.deltaTime);
    }
}