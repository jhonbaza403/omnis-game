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
        private GUIStyle pauseTitleStyle;
        private GUIStyle pauseButtonStyle;
        private Texture2D panelTexture;
        private Texture2D healthTexture;
        private Texture2D healthBackgroundTexture;
        private Texture2D resonanceTexture;
        private Texture2D resonanceBackgroundTexture;
        private GUIStyle resonanceLabelStyle;

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

            if (player.IsPaused)
            {
                GUI.DrawTexture(new Rect(0f, 0f, width, height), panelTexture);
                GUI.Label(new Rect(width * 0.5f - 180f, height * 0.31f, 360f, 54f), "OMNIS  /  PAUSA", pauseTitleStyle);
                GUI.Label(new Rect(width * 0.5f - 230f, height * 0.31f + 56f, 460f, 38f),
                    "El ciclo espera tu regreso.", promptStyle);
                float buttonX = width * 0.5f - 120f;
                float buttonY = height * 0.47f;
                if (GUI.Button(new Rect(buttonX, buttonY, 240f, 48f), "CONTINUAR", pauseButtonStyle))
                    player.ResumeGame();
                if (GUI.Button(new Rect(buttonX, buttonY + 62f, 240f, 48f), "REINICIAR ESCENA", pauseButtonStyle))
                    player.RestartGame();
                if (GUI.Button(new Rect(buttonX, buttonY + 124f, 240f, 48f), "SALIR DEL JUEGO", pauseButtonStyle))
                    player.ExitGame();
                GUI.matrix = previous;
                return;
            }

            GUI.color = Color.white;
            GUI.DrawTexture(new Rect(22f, 22f, 500f, 205f), panelTexture);
            GUI.Label(new Rect(42f, 34f, 455f, 38f), "OMNIS  /  EL ÚLTIMO CICLO", titleStyle);
            GUI.Label(new Rect(42f, 78f, 450f, 26f),
                $"CICLO {state.CycleIndex:00}   ·   {PhaseLabel(state.Phase)}", bodyStyle);
            GUI.Label(new Rect(42f, 108f, 450f, 25f),
                $"EXPERIENCIA  {state.Experience:N0} XP     ·     CENTINELAS  {state.SentinelsDefeated}", bodyStyle);
            GUI.DrawTexture(new Rect(42f, 143f, 246f, 12f), healthBackgroundTexture);
            GUI.DrawTexture(new Rect(42f, 143f, 246f * (player.CurrentHealth / (float)player.MaxHealth), 12f), healthTexture);
            GUI.Label(new Rect(300f, 137f, 175f, 24f), $"INTEGRIDAD  {player.CurrentHealth}%", smallStyle);

            OmnisResonanceCombat combat = player.GetComponent<OmnisResonanceCombat>();
            if (combat != null)
            {
                float barX = 42f;
                float barY = 170f;
                float barWidth = 430f;
                float fill = Mathf.Clamp01(combat.Resonance / (float)combat.ResonanceRequired);
                bool ready = fill >= 1f;
                float pulse = 0.72f + 0.28f * Mathf.Sin(Time.unscaledTime * 7f);
                GUI.color = ready ? new Color(0.5f, 0.95f, 1f, pulse) : Color.white;
                GUI.DrawTexture(new Rect(barX - 3f, barY - 3f, barWidth + 6f, 18f), resonanceBackgroundTexture);
                GUI.DrawTexture(new Rect(barX, barY, barWidth * fill, 12f), resonanceTexture);
                GUI.color = Color.white;
                GUI.Label(new Rect(barX, barY + 14f, 280f, 23f),
                    ready ? "RESONANCIA COMPLETA  ·  Q" : $"RESONANCIA  {combat.Resonance:000}/{combat.ResonanceRequired:000}", resonanceLabelStyle);
                GUI.Label(new Rect(barX + 290f, barY + 14f, 180f, 23f),
                    combat.IsInvulnerable ? "PASO DE GRIETA" : "CTRL · EVADIR", smallStyle);
            }

            GUI.DrawTexture(new Rect(width - 350f, 22f, 326f, 150f), panelTexture);
            GUI.Label(new Rect(width - 326f, 36f, 282f, 28f), "OBJETIVO PRINCIPAL", bodyStyle);
            GUI.Label(new Rect(width - 326f, 69f, 282f, 27f),
                $"Fragmentos del Nexo   {state.RelicsCollected}/3", bodyStyle);
            GUI.Label(new Rect(width - 326f, 100f, 282f, 24f),
                state.Phase == OmnisCyclePhase.Nexus ? "R  ·  Iniciar renacimiento" : "E  ·  Sincronizar un objeto", smallStyle);
            OmnisWorldSimulation worldSimulation = FindObjectOfType<OmnisWorldSimulation>();
            if (worldSimulation != null)
                GUI.Label(new Rect(width - 326f, 127f, 290f, 24f),
                    worldSimulation.TimeLabel + "  ·  " + worldSimulation.WeatherLabel, smallStyle);

            float controlsY = height - 56f;
            GUI.DrawTexture(new Rect(22f, controlsY - 14f, Mathf.Min(width - 44f, 940f), 43f), panelTexture);
            GUI.Label(new Rect(38f, controlsY - 5f, Mathf.Min(width - 76f, 910f), 26f),
                "WASD MOVER   ·   RATÓN MIRAR   ·   ESPACIO SALTAR   ·   SHIFT CORRER   ·   E INTERACTUAR   ·   F / CLIC ATACAR   ·   CTRL EVADIR   ·   Q PULSO   ·   ESC CURSOR",
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
            resonanceTexture = MakeTexture(new Color(0.12f, 0.78f, 1f, 1f));
            resonanceBackgroundTexture = MakeTexture(new Color(0.055f, 0.11f, 0.2f, 0.96f));

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
            resonanceLabelStyle = new GUIStyle(GUI.skin.label)
            {
                fontSize = 13, fontStyle = FontStyle.Bold,
                normal = { textColor = new Color(0.42f, 0.9f, 1f) }
            };
            pauseTitleStyle = new GUIStyle(titleStyle)
            {
                fontSize = 34, alignment = TextAnchor.MiddleCenter
            };
            pauseButtonStyle = new GUIStyle(GUI.skin.button)
            {
                fontSize = 17, fontStyle = FontStyle.Bold,
                normal = { textColor = new Color(0.42f, 1f, 0.89f) },
                hover = { textColor = Color.white }
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
            if (resonanceTexture != null) Destroy(resonanceTexture);
            if (resonanceBackgroundTexture != null) Destroy(resonanceBackgroundTexture);
        }
    }
}
