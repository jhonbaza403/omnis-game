using UnityEngine;

namespace Omnis
{
    /// <summary>Simple local AI threat for the playable slice: approaches, attacks, and can be defeated.</summary>
    public sealed class OmnisSentinel : MonoBehaviour
    {
        [SerializeField] private string sentinelId = "Sentinel-01";
        [SerializeField, Min(1)] private int maxHealth = 100;
        [SerializeField, Min(0.1f)] private float moveSpeed = 2.1f;
        [SerializeField, Min(0.1f)] private float detectionRadius = 24f;
        [SerializeField, Min(1)] private int contactDamage = 12;

        private OmnisPlayerController target;
        private int health;
        private float nextContactAttack;
        private Vector3 initialPosition;
        private bool defeated;
        public bool IsDefeated => defeated;

        public void Configure(string id, int healthValue = 100)
        {
            sentinelId = id;
            maxHealth = Mathf.Max(1, healthValue);
            health = maxHealth;
        }

        private void Start()
        {
            health = Mathf.Max(1, health == 0 ? maxHealth : health);
            initialPosition = transform.position;
            FindTarget();
        }

        private void Update()
        {
            if (defeated || (OmnisGameState.Instance != null && FindObjectOfType<OmnisPlayerController>()?.IsPaused == true)) return;
            if (target == null) FindTarget();
            if (target == null) return;

            Vector3 toTarget = target.transform.position - transform.position;
            toTarget.y = 0f;
            float distance = toTarget.magnitude;

            if (distance < detectionRadius && distance > 1.65f)
            {
                Vector3 direction = toTarget.normalized;
                Quaternion face = Quaternion.LookRotation(direction, Vector3.up);
                transform.rotation = Quaternion.Slerp(transform.rotation, face, Time.deltaTime * 5f);
                transform.position += direction * (moveSpeed * Time.deltaTime);
            }
            else if (distance <= 1.65f && Time.time >= nextContactAttack)
            {
                nextContactAttack = Time.time + 1.15f;
                target.TakeDamage(contactDamage);
            }

            Vector3 p = transform.position;
            p.y = initialPosition.y + Mathf.Sin(Time.time * 2.0f + initialPosition.x) * 0.12f;
            transform.position = p;
        }

        public void TakeDamage(int amount)
        {
            if (defeated || amount <= 0) return;
            health = Mathf.Max(0, health - amount);
            if (health > 0)
            {
                OmnisAudioDirector.Instance?.PlayCue(OmnisAudioCue.SentinelHit);
                return;
            }

            defeated = true;
            Collider ownCollider = GetComponent<Collider>();
            if (ownCollider != null) ownCollider.enabled = false;
            OmnisAudioDirector.Instance?.PlayCue(OmnisAudioCue.SentinelDefeated);
            OmnisGameState.Instance?.RegisterSentinelDefeated(sentinelId);
            Destroy(gameObject, 0.25f);
        }

        private void FindTarget()
        {
            target = FindObjectOfType<OmnisPlayerController>();
        }
    }
}
