using UnityEngine;

namespace Omnis
{
    /// <summary>Temporary built-in HUD for the first prototype; not final UI art.</summary>
    public sealed class OmnisHud : MonoBehaviour
    {
        [SerializeField] private OmnisPlayerController player;
        private GUIStyle titleStyle;
        private GUIStyle bodyStyle;
        private GUIStyle promptStyle;

        private void OnGUI()
        {
            EnsureStyles();
            OmnisGameState state = OmnisGameState.Instance;
            if (state == null) return;

            GUI.Box(new Rect(18, 18, 320, 112), GUIContent.none);
            GUI.Label(new Rect(32, 28, 300, 30), "OMNIS | EL ÚLTIMO CICLO", titleStyle);
            GUI.Label(new Rect(32, 61, 300, 24), $"Ciclo {state.CycleIndex} · {state.Phase}", bodyStyle);
            GUI.Label(new Rect(32, 86, 300, 24), $"Experiencia: {state.Experience} XP", bodyStyle);
            GUI.Label(new Rect(18, Screen.height - 44, Screen.width - 36, 28),
                "WASD mover · Ratón mirar · Espacio saltar · Shift correr · E interactuar · Esc liberar ratón", bodyStyle);

            if (player != null && player.FocusedInteractable != null)
            {
                string prompt = $"E · Interactuar: {player.FocusedInteractable.DisplayName}";
                Vector2 size = promptStyle.CalcSize(new GUIContent(prompt));
                GUI.Label(new Rect((Screen.width - size.x) * 0.5f, Screen.height * 0.68f, size.x + 16, 34), prompt, promptStyle);
            }
        }

        private void EnsureStyles()
        {
            if (titleStyle != null) return;
            titleStyle = new GUIStyle(GUI.skin.label) {
                fontSize = 17, fontStyle = FontStyle.Bold,
                normal = { textColor = new Color(0.35f, 0.95f, 0.9f) }
            };
            bodyStyle = new GUIStyle(GUI.skin.label) {
                fontSize = 14, normal = { textColor = Color.white }
            };
            promptStyle = new GUIStyle(GUI.skin.box) {
                fontSize = 16, alignment = TextAnchor.MiddleCenter, fontStyle = FontStyle.Bold,
                normal = { textColor = Color.white }
            };
        }
    }
}
