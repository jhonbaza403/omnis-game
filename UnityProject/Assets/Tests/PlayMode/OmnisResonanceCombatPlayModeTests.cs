using System.Collections;
using NUnit.Framework;
using UnityEngine;
using UnityEngine.TestTools;

namespace Omnis.Tests
{
    public sealed class OmnisResonanceCombatPlayModeTests
    {
        private GameObject playerObject;
        private OmnisPlayerController player;
        private OmnisResonanceCombat combat;

        [UnitySetUp]
        public IEnumerator SetUp()
        {
            Time.timeScale = 1f;
            playerObject = new GameObject("Combat Test Player");
            playerObject.transform.position = Vector3.zero;
            playerObject.AddComponent<CharacterController>();
            player = playerObject.AddComponent<OmnisPlayerController>();
            combat = playerObject.AddComponent<OmnisResonanceCombat>();
            yield return null;
        }

        [UnityTearDown]
        public IEnumerator TearDown()
        {
            if (playerObject != null) Object.Destroy(playerObject);
            foreach (OmnisSentinel sentinel in Object.FindObjectsOfType<OmnisSentinel>())
                if (sentinel != null) Object.Destroy(sentinel.gameObject);
            Time.timeScale = 1f;
            yield return null;
        }

        [UnityTest]
        public IEnumerator MeleeHitsChargeResonanceAndClampAtRequiredAmount()
        {
            Assert.AreEqual(0, combat.Resonance);
            for (int i = 0; i < 5; i++) combat.RegisterMeleeHit();
            Assert.AreEqual(combat.ResonanceRequired, combat.Resonance);
            Assert.AreEqual(1f, combat.ResonanceNormalized, 0.001f);
            yield return null;
        }

        [UnityTest]
        public IEnumerator FracturePulseNeedsFullChargeThenConsumesItAndDamagesTargetOnce()
        {
            GameObject enemyObject = new GameObject("Pulse Test Sentinel");
            enemyObject.transform.position = new Vector3(2f, 0f, 0f);
            enemyObject.AddComponent<SphereCollider>().radius = 0.7f;
            OmnisSentinel sentinel = enemyObject.AddComponent<OmnisSentinel>();
            sentinel.Configure("Test-Sentinel", 100);
            yield return null;

            Assert.IsFalse(combat.TryFracturePulse(), "An undercharged pulse must not fire.");
            Assert.AreEqual(100, sentinel.CurrentHealth);

            for (int i = 0; i < 5; i++) combat.RegisterMeleeHit();
            Assert.IsTrue(combat.TryFracturePulse());
            Assert.AreEqual(0, combat.Resonance);
            Assert.AreEqual(52, sentinel.CurrentHealth, "A single pulse should apply 48 damage once.");
        }

        [UnityTest]
        public IEnumerator RiftStepCreatesBriefInvulnerabilityAndHasCooldown()
        {
            Assert.IsTrue(combat.TryBeginRiftStep(Vector3.forward));
            Assert.IsTrue(combat.IsInvulnerable);
            int before = player.CurrentHealth;
            player.TakeDamage(20);
            Assert.AreEqual(before, player.CurrentHealth, "Damage must be ignored during Rift-step.");
            Assert.IsFalse(combat.TryBeginRiftStep(Vector3.right), "A second dodge must respect cooldown.");
            yield return new WaitForSeconds(0.22f);
            Assert.IsFalse(combat.IsInvulnerable);
        }
    }
}
