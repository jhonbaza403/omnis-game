using System;
using UnityEngine;

namespace Omnis
{
    public enum OmnisWeatherState { Clear, VeilMist, RiftStorm }

    /// <summary>
    /// Lightweight world clock and atmospheric-state prototype. It changes lighting and fog;
    /// it does not implement physical rain, seasons, NPC schedules or region streaming.
    /// </summary>
    public sealed class OmnisWorldSimulation : MonoBehaviour
    {
        [SerializeField, Min(60f)] private float fullDaySeconds = 720f;
        [SerializeField, Range(0f, 1f)] private float timeOfDay = 0.34f;
        [SerializeField, Min(20f)] private float weatherChangeMinSeconds = 75f;
        [SerializeField, Min(30f)] private float weatherChangeMaxSeconds = 145f;
        private Light keyLight;
        private float weatherTimer;
        private System.Random random;
        private Color baseSkyColor;
        private Color baseEquatorColor;
        private float baseFogEnd;
        private float baseLightIntensity = 1f;
        public OmnisWeatherState Weather { get; private set; } = OmnisWeatherState.Clear;
        public float TimeOfDay => timeOfDay;
        public string TimeLabel
        {
            get
            {
                int minutes = Mathf.FloorToInt(timeOfDay * 1440f) % 1440;
                return (minutes / 60).ToString("00") + ":" + (minutes % 60).ToString("00");
            }
        }
        public string WeatherLabel
        {
            get
            {
                switch (Weather)
                {
                    case OmnisWeatherState.VeilMist: return "BRUMA DEL VELO";
                    case OmnisWeatherState.RiftStorm: return "TORMENTA DE GRIETA";
                    default: return "CIELO DESPEJADO";
                }
            }
        }

        private void Awake()
        {
            random = new System.Random(20261012);
            GameObject lightObject = GameObject.Find("Nexus Key Light");
            if (lightObject != null) keyLight = lightObject.GetComponent<Light>();
            baseFogEnd = RenderSettings.fogEndDistance;
            baseSkyColor = RenderSettings.ambientSkyColor;
            baseEquatorColor = RenderSettings.ambientEquatorColor;
            if (keyLight != null) baseLightIntensity = keyLight.intensity;
            weatherTimer = NextWeatherInterval();
            ApplyWorldState();
        }

        private void Update()
        {
            if (fullDaySeconds > 0f) timeOfDay = Mathf.Repeat(timeOfDay + Time.deltaTime / fullDaySeconds, 1f);
            weatherTimer -= Time.deltaTime;
            if (weatherTimer <= 0f)
            {
                Weather = (OmnisWeatherState)random.Next(0, Enum.GetValues(typeof(OmnisWeatherState)).Length);
                weatherTimer = NextWeatherInterval();
            }
            ApplyWorldState();
        }

        private float NextWeatherInterval()
        {
            float max = Mathf.Max(weatherChangeMinSeconds, weatherChangeMaxSeconds);
            return (float)(weatherChangeMinSeconds + random.NextDouble() * (max - weatherChangeMinSeconds));
        }

        private void ApplyWorldState()
        {
            float daylight = Mathf.Clamp01(Mathf.Sin((timeOfDay - 0.25f) * Mathf.PI * 2f) * 0.5f + 0.5f);
            if (keyLight != null)
            {
                keyLight.transform.rotation = Quaternion.Euler(timeOfDay * 360f - 90f, -32f, 0f);
                keyLight.intensity = Mathf.Lerp(0.08f, baseLightIntensity, daylight);
                keyLight.color = Color.Lerp(new Color(0.28f, 0.36f, 0.62f), new Color(1f, 0.82f, 0.63f), Mathf.Clamp01(daylight * 1.2f));
            }
            RenderSettings.fogColor = Color.Lerp(new Color(0.018f, 0.025f, 0.075f), new Color(0.12f, 0.18f, 0.22f), daylight);
            RenderSettings.ambientSkyColor = Color.Lerp(new Color(0.035f, 0.045f, 0.12f), baseSkyColor, daylight);
            RenderSettings.ambientEquatorColor = Color.Lerp(new Color(0.025f, 0.035f, 0.08f), baseEquatorColor, daylight);
            RenderSettings.fogEndDistance = Mathf.Lerp(baseFogEnd * 0.72f, baseFogEnd, daylight);
            if (Weather == OmnisWeatherState.VeilMist)
            {
                RenderSettings.fogColor = Color.Lerp(RenderSettings.fogColor, new Color(0.23f, 0.29f, 0.36f), 0.52f);
                RenderSettings.fogEndDistance *= 0.62f;
                RenderSettings.ambientSkyColor = Color.Lerp(RenderSettings.ambientSkyColor, new Color(0.28f, 0.32f, 0.39f), 0.28f);
            }
            else if (Weather == OmnisWeatherState.RiftStorm)
            {
                RenderSettings.fogColor = Color.Lerp(RenderSettings.fogColor, new Color(0.12f, 0.035f, 0.22f), 0.62f);
                RenderSettings.fogEndDistance *= 0.48f;
                RenderSettings.ambientSkyColor = Color.Lerp(RenderSettings.ambientSkyColor, new Color(0.16f, 0.045f, 0.28f), 0.6f);
                if (keyLight != null) keyLight.intensity *= 0.62f;
            }
        }
    }
}
