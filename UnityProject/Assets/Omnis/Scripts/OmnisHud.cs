using UnityEngine;

namespace Omnis
{
    /// <summary>Resolution-independent prototype HUD; layout scales with 4K and ultrawide displays.</summary>
    public sealed class OmnisHud : MonoBehaviour
    {
        [SerializeField] private OmnisPlayerController player;
        private GUIStyle titleStyle;
        private GUIStyle bodyStyle;
        private GUIStyle smallStyle;
        private GUIStyle promptStyle;
        private GUIStyle statusStyle;
        private Texture2D panelTexture;
        private Texture2D healthTexture;
        private Texture2D healthBackgroundTexture;

        public void Configure(OmnisPlayerController target) => player = target;

        private void OnGUI()
        {
            OmnisGameState state = OmnisGameState.Instance;
            if (state == null || player == null) return;
            EnsureStyles();

            float scale = Mathf.Clamp(Screen.height / 1080f, 0.85f, 2.4f);
            Matrix4x4 previous = GUI.matrix;
            GUI.matrix = Matrix4x4.Scale(new Vector3(scale, scale, 1f));
            float width = Screen.width / scale;
            float height = Screen.height / scale;

            GUI.color = Color.white;
            GUI.DrawTexture(new Rect(22f, 22f, 500f, 162f), panelTexture);
            GUI.Label(new Rect(42f, 34f, 455f, 38f), "OMNIS  /  EL ÚLTIMO CICLO", titleStyle);
            GUI.Label(new Rect(42f, 78f, 450f, 26f),
                $"CICLO {state.CycleIndex:00}   ·   {PhaseLabel(state.Phase)}", bodyStyle);
            GUI.Label(new Rect(42f, 108f, 450f, 25f),
                $"EXPERIENCIA  {state.Experience:N0} XP     ·     CENTINELAS  {state.SentinelsDefeated}", bodyStyle);
            GUI.DrawTexture(new Rect(42f, 143f, 246f, 12f), healthBackgroundTexture);
            GUI.DrawTexture(new Rect(42f, 143f, 246f * (player.CurrentHealth / (float)player.MaxHealth), 12f), healthTexture);
            GUI.Label(new Rect(300f, 137f, 175f, 24f), $"INTEGRIDAD  {player.CurrentHealth}%", smallStyle);

            GUI.DrawTexture(new Rect(width - 350f, 22f, 326f, 118f), panelTexture);
            GUI.Label(new Rect(width - 326f, 36f, 282f, 28f), "OBJETIVO PRINCIPAL", bodyStyle);
            GUI.Label(new Rect(width - 326f, 69f, 282f, 27f),
                $"Fragmentos del Nexo   {state.RelicsCollected}/3", bodyStyle);
            GUI.Label(new Rect(width - 326f, 100f, 282f, 24f),
                state.Phase == OmnisCyclePhase.Nexus ? "R  ·  Iniciar renacimiento" : "E  ·  Sincronizar un objeto", smallStyle);

            float controlsY = height - 56f;
            GUI.DrawTexture(new Rect(22f, controlsY - 14f, Mathf.Min(width - 44f, 940f), 43f), panelTexture);
            GUI.Label(new Rect(38f, controlsY - 5f, Mathf.Min(width - 76f, 910f), 26f),
                "WASD MOVER   ·   RATÓN MIRAR   ·   ESPACIO SALTAR   ·   SHIFT CORRER   ·   E INTERACTUAR   ·   F / CLIC PULSO   ·   ESC CURSOR",
                smallStyle);

            if (player.FocusedInteractable != null)
            {
                string prompt = player.FocusedInteractable.PromptText;
                float promptWidth = Mathf.Min(720f, width - 64f);
                GUI.DrawTexture(new Rect((width - promptWidth) * 0.5f, height * 0.74f, promptWidth, 52f), panelTexture);
                GUI.Label(new Rect((width - promptWidth) * 0.5f + 18f, height * 0.74f + 9f,
                    promptWidth - 36f, 34f), prompt, promptStyle);
            }

            if (!string.IsNullOrEmpty(player.StatusMessage))
            {
                float msgWidth = Mathf.Min(900f, width - 48f);
                GUI.DrawTexture(new Rect((width - msgWidth) * 0.5f, height * 0.84f, msgWidth, 46f), panelTexture);
                GUI.Label(new Rect((width - msgWidth) * 0.5f + 18f, height * 0.84f + 7f,
                    msgWidth - 36f, 32f), player.StatusMessage, statusStyle);
            }

            // Small center reticle
            GUI.color = new Color(0.45f, 1f, 0.9f, 0.92f);
            GUI.DrawTexture(new Rect(width * 0.5f - 1f, height * 0.5f - 9f, 2f, 18f), healthTexture);
            GUI.DrawTexture(new Rect(width * 0.5f - 9f, height * 0.5f - 1f, 18f, 2f), healthTexture);
            GUI.color = Color.white;
            GUI.matrix = previous;
        }

        private static string PhaseLabel(OmnisCyclePhase phase)
        {
            switch (phase)
            {
                case OmnisCyclePhase.Crisis: return "CRISIS";
                case OmnisCyclePhase.Collapse: return "COLAPSO";
                case OmnisCyclePhase.Nexus: return "NEXO ACTIVADO";
                case OmnisCyclePhase.Rebirth: return "RENACIMIENTO";
                default: return "DESPERTAR";
            }
        }

        private void EnsureStyles()
        {
            if (titleStyle != null) return;
            panelTexture = MakeTexture(new Color(0.015f, 0.035f, 0.07f, 0.86f));
            healthTexture = MakeTexture(new Color(0.18f, 0.96f, 0.8f, 1f));
            healthBackgroundTexture = MakeTexture(new Color(0.12f, 0.17f, 0.23f, 1f));

            titleStyle = new GUIStyle(GUI.skin.label)
            {
                fontSize = 24, fontStyle = FontStyle.Bold,
                normal = { textColor = new Color(0.36f, 1f, 0.87f) }
            };
            bodyStyle = new GUIStyle(GUI.skin.label)
            {
                fontSize = 17, fontStyle = FontStyle.Bold,
                normal = { textColor = new Color(0.89f, 0.95f, 1f) }
            };
            smallStyle = new GUIStyle(GUI.skin.label)
            {
                fontSize = 13, fontStyle = FontStyle.Bold,
                normal = { textColor = new Color(0.65f, 0.83f, 0.91f) }
            };
            promptStyle = new GUIStyle(GUI.skin.label)
            {
                fontSize = 19, alignment = TextAnchor.MiddleCenter, fontStyle = FontStyle.Bold,
                normal = { textColor = Color.white }
            };
            statusStyle = new GUIStyle(GUI.skin.label)
            {
                fontSize = 16, alignment = TextAnchor.MiddleCenter, fontStyle = FontStyle.Bold,
                wordWrap = true, normal = { textColor = new Color(0.53f, 1f, 0.88f) }
            };
        }

        private static Texture2D MakeTexture(Color color)
        {
            Texture2D texture = new Texture2D(1, 1, TextureFormat.RGBA32, false);
            texture.hideFlags = HideFlags.HideAndDontSave;
            texture.SetPixel(0, 0, color);
            texture.Apply();
            return texture;
        }

        private void OnDestroy()
        {
            if (panelTexture != null) Destroy(panelTexture);
            if (healthTexture != null) Destroy(healthTexture);
            if (healthBackgroundTexture != null) Destroy(healthBackgroundTexture);
        }
    }
}
