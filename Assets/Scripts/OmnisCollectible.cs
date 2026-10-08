using UnityEngine;

namespace Omnis
{
    public sealed class OmnisCollectible : MonoBehaviour
    {
        private bool collected;
        private void Update()
        {
            transform.Rotate(0f, 90f * Time.deltaTime, 0f, Space.World);
            transform.localScale = Vector3.one * (1f + Mathf.Sin(Time.time * 3f) * 0.08f);
        }

        private void OnTriggerEnter(Collider other)
        {
            if (collected || !other.CompareTag("Player")) return;
            collected = true;
            OmnisBootstrap.Instance?.RegisterCollectible();
            Destroy(gameObject);
        }
    }
}
