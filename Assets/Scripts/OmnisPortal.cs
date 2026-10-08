using UnityEngine;

namespace Omnis
{
    public sealed class OmnisPortal : MonoBehaviour
    {
        private void Update() => transform.Rotate(0f, 25f * Time.deltaTime, 0f, Space.World);

        private void OnTriggerEnter(Collider other)
        {
            if (other.CompareTag("Player")) OmnisBootstrap.Instance?.EnterNexus();
        }
    }
}
