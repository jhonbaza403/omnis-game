using System.Collections;
using System.Collections.Generic;
using NUnit.Framework;
using UnityEngine;
using UnityEngine.SceneManagement;
using UnityEngine.TestTools;

namespace Omnis.Tests
{
    /// <summary>
    /// Runtime smoke and objective-flow tests for the committed Unity vertical slice.
    /// PlayerPrefs are backed up and restored so the test does not erase real progress.
    /// </summary>
    public sealed class OmnisPlayableSliceSmokeTests
    {
        private const string SceneName = "OmnisMain";
        private readonly List<PreferenceSnapshot> preferences = new List<PreferenceSnapshot>();

        [System.Serializable]
        private sealed class PreferenceSnapshot
        {
            public string key;
            public bool existed;
            public int intValue;
            public string stringValue;
            public bool isString;
        }

        private static readonly string[] TestKeys =
        {
            "Omnis.Progress.Cycle",
            "Omnis.Progress.Experience",
            "Omnis.Progress.Phase",
            "Omnis.Cycle.1.Relics",
            "Omnis.Cycle.1.Sentinels",
            "Omnis.Cycle.1.Interaction.Omnis.Relic.A",
            "Omnis.Cycle.1.Interaction.Omnis.Relic.B",
            "Omnis.Cycle.1.Interaction.Omnis.Relic.C",
            "Omnis.Cycle.1.Interaction.Omnis.Nexus.Obelisk",
            "Omnis.Cycle.1.Sentinel.Sentinel-East",
            "Omnis.Cycle.1.Sentinel.Sentinel-West",
            "Omnis.Cycle.1.Sentinel.Sentinel-North"
        };

        [UnitySetUp]
        public IEnumerator SetUp()
        {
            preferences.Clear();
            foreach (string key in TestKeys)
            {
                bool isString = key == "Omnis.Progress.Phase";
                preferences.Add(new PreferenceSnapshot
                {
                    key = key,
                    existed = PlayerPrefs.HasKey(key),
                    intValue = PlayerPrefs.GetInt(key, 0),
                    stringValue = PlayerPrefs.GetString(key, string.Empty),
                    isString = isString
                });
                PlayerPrefs.DeleteKey(key);
            }
            PlayerPrefs.Save();

            SceneManager.LoadScene(SceneName, LoadSceneMode.Single);
            yield return null;
            yield return null;
        }

        [UnityTearDown]
        public IEnumerator TearDown()
        {
            foreach (PreferenceSnapshot snapshot in preferences)
            {
                PlayerPrefs.DeleteKey(snapshot.key);
                if (!snapshot.existed) continue;
                if (snapshot.isString) PlayerPrefs.SetString(snapshot.key, snapshot.stringValue);
                else PlayerPrefs.SetInt(snapshot.key, snapshot.intValue);
            }
            PlayerPrefs.Save();

            SceneManager.LoadScene(SceneName, LoadSceneMode.Single);
            yield return null;
            yield return null;
        }

        [UnityTest]
        public IEnumerator MainSceneBuildsPlayableWorldAndCompletesRelicToNexusFlow()
        {
            GameObject world = GameObject.Find("OMNIS_RUNTIME_WORLD");
            Assert.That(world, Is.Not.Null, "Runtime world bootstrap did not materialize the environment.");

            OmnisPlayerController player = Object.FindFirstObjectByType<OmnisPlayerController>();
            Assert.That(player, Is.Not.Null, "Player controller is missing.");
            Assert.That(player.GetComponent<CharacterController>(), Is.Not.Null, "Player has no CharacterController.");
            Assert.That(player.CurrentHealth, Is.EqualTo(player.MaxHealth), "Player should start at full health.");

            Camera camera = Camera.main;
            Assert.That(camera, Is.Not.Null, "Main camera is missing.");
            Assert.That(Object.FindFirstObjectByType<OmnisHud>(), Is.Not.Null, "HUD is missing.");

            OmnisInteractable[] interactables = Object.FindObjectsByType<OmnisInteractable>(FindObjectsSortMode.None);
            List<OmnisInteractable> relics = new List<OmnisInteractable>();
            OmnisInteractable nexus = null;
            foreach (OmnisInteractable item in interactables)
            {
                if (item.InteractionId.StartsWith("Omnis.Relic.")) relics.Add(item);
                if (item.InteractionId == "Omnis.Nexus.Obelisk") nexus = item;
            }
            Assert.That(relics.Count, Is.EqualTo(3), "The vertical slice must contain three relics.");
            Assert.That(nexus, Is.Not.Null, "The Nexus interaction is missing.");
            Assert.That(Object.FindObjectsByType<OmnisSentinel>(FindObjectsSortMode.None).Length, Is.GreaterThanOrEqualTo(3),
                "Expected the three sentinel encounters.");

            OmnisGameState state = OmnisGameState.Instance;
            Assert.That(state, Is.Not.Null, "Progression state is missing.");
            foreach (OmnisInteractable relic in relics) relic.Interact(player);

            Assert.That(state.RelicsCollected, Is.EqualTo(3), "Collecting all relics should unlock the Nexus.");
            Assert.That(state.Experience, Is.EqualTo(150), "Three relics should award 150 XP.");
            nexus.Interact(player);
            Assert.That(state.Phase, Is.EqualTo(OmnisCyclePhase.Nexus), "Nexus interaction should complete the slice objective.");
            Assert.That(state.Experience, Is.EqualTo(250), "Nexus activation should award the final 100 XP.");

            yield return null;
            Debug.Log("[Omnis Smoke] PASS: scene, player, camera, HUD, three relics, three sentinels and relic-to-Nexus progression.");
        }

        [UnityTest]
        public IEnumerator PlayerDamageAndSentinelDefeatAreFunctional()
        {
            OmnisPlayerController player = Object.FindFirstObjectByType<OmnisPlayerController>();
            OmnisSentinel sentinel = Object.FindFirstObjectByType<OmnisSentinel>();
            OmnisGameState state = OmnisGameState.Instance;
            Assert.That(player, Is.Not.Null);
            Assert.That(sentinel, Is.Not.Null);
            Assert.That(state, Is.Not.Null);

            int originalHealth = player.CurrentHealth;
            player.TakeDamage(10);
            Assert.That(player.CurrentHealth, Is.EqualTo(originalHealth - 10), "Player damage did not reduce health.");

            int originalSentinels = state.SentinelsDefeated;
            sentinel.TakeDamage(1000);
            Assert.That(sentinel.IsDefeated, Is.True, "Sentinel should enter defeated state.");
            Assert.That(state.SentinelsDefeated, Is.EqualTo(originalSentinels + 1), "Defeating a sentinel should update progression.");

            yield return null;
            Debug.Log("[Omnis Smoke] PASS: player damage and sentinel defeat progression.");
        }
    }
}
