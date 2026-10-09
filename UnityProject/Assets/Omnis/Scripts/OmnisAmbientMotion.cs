using UnityEngine;

namespace Omnis
{
    /// <summary>Small deterministic motion for relics and the Nexus core.</summary>
    public sealed class OmnisAmbientMotion : MonoBehaviour
    {
        [SerializeField] private float turnDegreesPerSecond = 24f;
        [SerializeField] private float bobHeight = 0.22f;
        [SerializeField] private float bobFrequency = 1.1f;

        private Vector3 initialPosition;
        private float phaseOffset;

        private void Start()
        {
            initialPosition = transform.position;
            phaseOffset = Mathf.Abs(transform.position.x * 0.17f + transform.position.z * 0.31f);
        }

        private void Update()
        {
            transform.Rotate(0f, turnDegreesPerSecond * Time.deltaTime, 0f, Space.World);
            Vector3 position = initialPosition;
            position.y += Mathf.Sin((Time.time + phaseOffset) * bobFrequency) * bobHeight;
            transform.position = position;
        }
    }
}
