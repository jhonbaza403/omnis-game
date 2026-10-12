using System;
using UnityEngine;

namespace Omnis
{
    public enum OmnisAudioCue
    {
        Relic,
        Nexus,
        SentinelHit,
        SentinelDefeated,
        PlayerDamage,
        RiftStep,
        FracturePulse
    }

    /// <summary>Self-contained synthetic ambience and effects; no external audio files are faked.</summary>
    public sealed class OmnisAudioDirector : MonoBehaviour
    {
        private const int SampleRate = 22050;
        private static readonly int[] RelicNotes = { 660, 880, 990 };
        private static readonly int[] NexusNotes = { 440, 554, 659, 880, 1108 };
        public static OmnisAudioDirector Instance { get; private set; }

        private AudioSource ambientSource;
        private AudioSource effectsSource;
        private AudioClip ambientClip;
        private AudioClip relicClip;
        private AudioClip nexusClip;
        private AudioClip hitClip;
        private AudioClip defeatClip;
        private AudioClip damageClip;
        private AudioClip riftStepClip;
        private AudioClip fracturePulseClip;

        private void Awake()
        {
            if (Instance != null && Instance != this)
            {
                Destroy(gameObject);
                return;
            }

            Instance = this;
            ambientSource = gameObject.AddComponent<AudioSource>();
            ambientSource.playOnAwake = false;
            ambientSource.loop = true;
            ambientSource.spatialBlend = 0f;
            ambientSource.volume = 0.18f;
            ambientSource.clip = CreateAmbientClip();

            effectsSource = gameObject.AddComponent<AudioSource>();
            effectsSource.playOnAwake = false;
            effectsSource.loop = false;
            effectsSource.spatialBlend = 0f;
            effectsSource.volume = 0.72f;

            relicClip = CreateToneSequence("Aether Relic Chime", RelicNotes, 0.48f, 0.22f);
            nexusClip = CreateToneSequence("Nexus Activation", NexusNotes, 0.95f, 0.3f);
            hitClip = CreateImpact("Sentinel Pulse Impact", 140f, 0.22f);
            defeatClip = CreateToneSequence("Sentinel Collapse", new[] { 520, 390, 260 }, 0.65f, 0.24f);
            damageClip = CreateImpact("Wanderer Damage", 92f, 0.18f);
            riftStepClip = CreateToneSequence("Rift-step Phase Shift", new[] { 980, 740, 1320 }, 0.24f, 0.22f);
            fracturePulseClip = CreateToneSequence("Fracture Pulse", new[] { 110, 220, 440, 880 }, 0.72f, 0.34f);

            if (ambientSource.clip != null) ambientSource.Play();
        }

        public void PlayCue(OmnisAudioCue cue)
        {
            if (effectsSource == null) return;
            AudioClip clip = cue switch
            {
                OmnisAudioCue.Relic => relicClip,
                OmnisAudioCue.Nexus => nexusClip,
                OmnisAudioCue.SentinelHit => hitClip,
                OmnisAudioCue.SentinelDefeated => defeatClip,
                OmnisAudioCue.PlayerDamage => damageClip,
                OmnisAudioCue.RiftStep => riftStepClip,
                OmnisAudioCue.FracturePulse => fracturePulseClip,
                _ => null
            };

            if (clip != null) effectsSource.PlayOneShot(clip);
        }

        private AudioClip CreateAmbientClip()
        {
            const int durationSeconds = 8;
            int sampleCount = SampleRate * durationSeconds;
            float[] samples = new float[sampleCount];
            for (int i = 0; i < sampleCount; i++)
            {
                float t = i / (float)SampleRate;
                float slowBreath = 0.72f + 0.28f * Mathf.Sin(t * Mathf.PI * 2f / durationSeconds);
                float low = Mathf.Sin(t * Mathf.PI * 2f * 55f) * 0.06f;
                float mid = Mathf.Sin(t * Mathf.PI * 2f * 82.5f) * 0.035f;
                float high = Mathf.Sin(t * Mathf.PI * 2f * 110f) * 0.018f;
                samples[i] = (low + mid + high) * slowBreath;
            }

            AudioClip clip = AudioClip.Create("Omnis Aether Drone", sampleCount, 1, SampleRate, false);
            clip.SetData(samples, 0);
            return clip;
        }

        private AudioClip CreateToneSequence(string clipName, int[] notes, float duration, float volume)
        {
            int sampleCount = Mathf.CeilToInt(SampleRate * duration);
            float[] samples = new float[sampleCount];
            for (int i = 0; i < sampleCount; i++)
            {
                float t = i / (float)SampleRate;
                float normalized = t / duration;
                int noteIndex = Mathf.Min(notes.Length - 1, Mathf.FloorToInt(normalized * notes.Length));
                float noteStart = noteIndex / (float)notes.Length * duration;
                float localTime = Mathf.Max(0f, t - noteStart);
                float envelope = Mathf.Exp(-localTime * (3.8f + noteIndex * 0.35f)) * Mathf.Clamp01((1f - normalized) * 7f);
                float fundamental = Mathf.Sin(localTime * Mathf.PI * 2f * notes[noteIndex]);
                float overtone = Mathf.Sin(localTime * Mathf.PI * 4f * notes[noteIndex]) * 0.25f;
                samples[i] = (fundamental + overtone) * envelope * volume;
            }

            AudioClip clip = AudioClip.Create(clipName, sampleCount, 1, SampleRate, false);
            clip.SetData(samples, 0);
            return clip;
        }

        private AudioClip CreateImpact(string clipName, float frequency, float duration)
        {
            int sampleCount = Mathf.CeilToInt(SampleRate * duration);
            float[] samples = new float[sampleCount];
            for (int i = 0; i < sampleCount; i++)
            {
                float t = i / (float)SampleRate;
                float envelope = Mathf.Exp(-t * 24f);
                float sweep = frequency * (1f - 0.38f * t / duration);
                float body = Mathf.Sin(t * Mathf.PI * 2f * sweep);
                float click = Mathf.Sin(t * Mathf.PI * 2f * 1500f) * Mathf.Exp(-t * 65f) * 0.16f;
                samples[i] = (body * 0.45f + click) * envelope;
            }
            AudioClip clip = AudioClip.Create(clipName, sampleCount, 1, SampleRate, false);
            clip.SetData(samples, 0);
            return clip;
        }

        private void OnDestroy()
        {
            if (Instance == this) Instance = null;
            DestroyClip(ambientClip);
            DestroyClip(relicClip);
            DestroyClip(nexusClip);
            DestroyClip(hitClip);
            DestroyClip(defeatClip);
            DestroyClip(damageClip);
            DestroyClip(riftStepClip);
            DestroyClip(fracturePulseClip);
        }

        private static void DestroyClip(AudioClip clip)
        {
            if (clip != null) Destroy(clip);
        }
    }
}
