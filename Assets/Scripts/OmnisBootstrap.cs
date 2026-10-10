using System.Collections.Generic;
using UnityEngine;
using UnityEngine.InputSystem;

namespace Omnis
{
    public sealed class OmnisBootstrap : MonoBehaviour
    {
        public static OmnisBootstrap Instance { get; private set; }
        private readonly List<Material> materials = new();
        private int collected;
        private const int Target = 8;
        private string status = "Explora el Nexus y reúne 8 fragmentos de ciclo.";
        private GameObject player;

        private void Awake()
        {
            Instance = this;
            Application.targetFrameRate = 120;
            BuildWorld();
        }

        private void Update()
        {
            if (Keyboard.current != null && Keyboard.current.rKey.wasPressedThisFrame && player != null)
            {
                player.GetComponent<OmnisPlayerController>()?.ResetToStart();
                collected = 0;
                status = "Ciclo reiniciado. Reúne los fragmentos.";
                BuildCollectibles();
            }
        }

        private void BuildWorld()
        {
            // PremiumWorldRuntime creates the terrain, city, vegetation and Nexus plaza.
            CreateLighting();
            CreatePlayer();
            CreateCamera();
            BuildCollectibles();
            CreatePortal();
        }

        private void CreateLighting()
        {
            RenderSettings.ambientIntensity = 1.2f;
            RenderSettings.reflectionIntensity = 0.8f;
            GameObject lightObject = new GameObject("Nexus Sun");
            Light light = lightObject.AddComponent<Light>();
            light.type = LightType.Directional;
            light.intensity = 1.35f;
            light.shadows = LightShadows.Soft;
            light.transform.rotation = Quaternion.Euler(48f, -30f, 0f);
        }

        private void CreateGround()
        {
            GameObject ground = GameObject.CreatePrimitive(PrimitiveType.Plane);
            ground.name = "Nexus Ground";
            ground.transform.localScale = new Vector3(7f, 1f, 7f);
            ground.GetComponent<Renderer>().sharedMaterial = MakeMaterial(new Color(0.08f, 0.12f, 0.18f), 0.2f, 0.5f);

            for (int i = 0; i < 18; i++)
            {
                float angle = i * Mathf.PI * 2f / 18f;
                float radius = 13f + (i % 3) * 2f;
                Vector3 pos = new Vector3(Mathf.Cos(angle) * radius, 1.2f, Mathf.Sin(angle) * radius);
                GameObject pillar = GameObject.CreatePrimitive(PrimitiveType.Cylinder);
                pillar.name = "Nexus Pillar " + i;
                pillar.transform.SetPositionAndRotation(pos, Quaternion.identity);
                pillar.transform.localScale = new Vector3(1.2f, 2.5f + (i % 4), 1.2f);
                pillar.GetComponent<Renderer>().sharedMaterial = MakeMaterial(new Color(0.16f, 0.32f, 0.48f), 0.35f, 0.7f);
            }
        }

        private void CreateNexus()
        {
            GameObject core = GameObject.CreatePrimitive(PrimitiveType.Sphere);
            core.name = "Nexus Core";
            core.transform.position = new Vector3(0f, 4f, 10f);
            core.transform.localScale = Vector3.one * 4f;
            core.GetComponent<Renderer>().sharedMaterial = MakeEmissionMaterial(new Color(0.15f, 0.8f, 1f));
        }

        private void CreatePlayer()
        {
            player = new GameObject("Omnis Player");
            player.tag = "Player";
            player.transform.position = new Vector3(0f, 1.2f, -8f);
            CharacterController controller = player.AddComponent<CharacterController>();
            controller.height = 1.8f;
            controller.radius = 0.45f;
            controller.center = new Vector3(0f, 0.9f, 0f);

            GameObject body = GameObject.CreatePrimitive(PrimitiveType.Capsule);
            body.name = "Player Visual";
            body.transform.SetParent(player.transform, false);
            body.transform.localPosition = new Vector3(0f, 0.9f, 0f);
            body.transform.localScale = new Vector3(0.8f, 0.9f, 0.8f);
            Destroy(body.GetComponent<Collider>());
            body.GetComponent<Renderer>().sharedMaterial = MakeMaterial(new Color(1f, 0.38f, 0.16f), 0.25f, 0.8f);

            GameObject visor = GameObject.CreatePrimitive(PrimitiveType.Sphere);
            visor.name = "Player Energy Core";
            visor.transform.SetParent(player.transform, false);
            visor.transform.localPosition = new Vector3(0f, 1.35f, 0.36f);
            visor.transform.localScale = new Vector3(0.28f, 0.2f, 0.12f);
            Destroy(visor.GetComponent<Collider>());
            visor.GetComponent<Renderer>().sharedMaterial = MakeEmissionMaterial(new Color(1f, 0.9f, 0.2f));
            player.AddComponent<OmnisPlayerController>();
        }

        private void CreateCamera()
        {
            GameObject cameraObject = new GameObject("Main Camera");
            cameraObject.tag = "MainCamera";
            cameraObject.AddComponent<Camera>();
            cameraObject.AddComponent<AudioListener>();
            cameraObject.transform.position = new Vector3(0f, 10f, -15f);
            CameraFollow follow = cameraObject.AddComponent<CameraFollow>();
            follow.Target = player.transform;
        }

        private void BuildCollectibles()
        {
            foreach (Transform child in transform)
                if (child.name.StartsWith("Fragment_")) Destroy(child.gameObject);

            Vector3[] positions =
            {
                new(-8f, 1.5f, -1f), new(8f, 1.5f, 0f),
                new(-10f, 1.5f, 8f), new(10f, 1.5f, 8f),
                new(-5f, 1.5f, 13f), new(5f, 1.5f, 13f),
                new(-14f, 1.5f, -8f), new(14f, 1.5f, -8f)
            };

            for (int i = 0; i < positions.Length; i++)
            {
                GameObject fragment = GameObject.CreatePrimitive(PrimitiveType.Sphere);
                fragment.name = "Fragment_" + i;
                fragment.transform.SetParent(transform);
                fragment.transform.position = positions[i];
                fragment.transform.localScale = Vector3.one * 0.65f;
                fragment.GetComponent<SphereCollider>().isTrigger = true;
                fragment.GetComponent<Renderer>().sharedMaterial = MakeEmissionMaterial(new Color(0.3f, 1f, 0.75f));
                fragment.AddComponent<OmnisCollectible>();
            }
        }

        private void CreatePortal()
        {
            GameObject portal = GameObject.CreatePrimitive(PrimitiveType.Cylinder);
            portal.name = "Cycle Portal";
            portal.transform.position = new Vector3(0f, 0.15f, 20f);
            portal.transform.localScale = new Vector3(3f, 0.2f, 3f);
            SphereCollider trigger = portal.AddComponent<SphereCollider>();
            trigger.isTrigger = true;
            trigger.radius = 1.2f;
            portal.GetComponent<Renderer>().sharedMaterial = MakeEmissionMaterial(new Color(0.65f, 0.25f, 1f));
            portal.AddComponent<OmnisPortal>();
        }

        public void RegisterCollectible()
        {
            collected++;
            status = collected >= Target
                ? "Nexus completo. Entra al Portal del Ciclo."
                : $"Fragmentos reunidos: {collected}/{Target}";
        }

        public void EnterNexus()
        {
            status = collected >= Target
                ? "¡Ciclo completado! Pulsa R para una nueva expedición."
                : $"El Portal necesita {Target - collected} fragmentos más.";
        }

        private Material MakeMaterial(Color color, float metallic, float smoothness)
        {
            Material material = new Material(Shader.Find("Standard"));
            material.color = color;
            material.SetFloat("_Metallic", metallic);
            material.SetFloat("_Glossiness", smoothness);
            materials.Add(material);
            return material;
        }

        private Material MakeEmissionMaterial(Color color)
        {
            Material material = MakeMaterial(color, 0.1f, 0.85f);
            material.EnableKeyword("_EMISSION");
            material.SetColor("_EmissionColor", color * 2.2f);
            return material;
        }

        private void OnGUI()
        {
            // Layout is authored at 1080p and scales cleanly up to 4K without tiny text.
            float scale = Mathf.Clamp(Mathf.Min(Screen.width / 1920f, Screen.height / 1080f), 0.55f, 2f);
            Matrix4x4 previousMatrix = GUI.matrix;
            Color previousColor = GUI.color;
            GUI.matrix = Matrix4x4.TRS(Vector3.zero, Quaternion.identity, Vector3.one * scale);

            GUIStyle panel = new GUIStyle(GUI.skin.box)
            {
                alignment = TextAnchor.UpperLeft,
                padding = new RectOffset(20, 20, 14, 14)
            };
            GUIStyle title = new GUIStyle(GUI.skin.label)
            {
                fontSize = 28,
                fontStyle = FontStyle.Bold,
                normal = { textColor = new Color(0.45f, 0.91f, 1f) }
            };
            GUIStyle body = new GUIStyle(GUI.skin.label)
            {
                fontSize = 18,
                wordWrap = true,
                normal = { textColor = new Color(0.93f, 0.96f, 1f) }
            };
            GUIStyle objective = new GUIStyle(body)
            {
                fontSize = 20,
                fontStyle = FontStyle.Bold,
                alignment = TextAnchor.MiddleRight
            };

            GUI.color = new Color(0.025f, 0.045f, 0.075f, 0.94f);
            GUI.Box(new Rect(24, 24, 650, 170), GUIContent.none, panel);
            GUI.color = Color.white;
            GUI.Label(new Rect(46, 38, 600, 42), "OMNIS  /  EL ÚLTIMO CICLO", title);
            GUI.Label(new Rect(46, 86, 600, 28), "WASD o flechas: moverse   •   Mayús: correr   •   Espacio: saltar", body);
            GUI.Label(new Rect(46, 118, 600, 28), "Mantén clic derecho y mueve el ratón: cámara   •   R: reiniciar", body);
            GUI.Label(new Rect(46, 150, 600, 28), status, body);

            GUI.color = new Color(0.025f, 0.045f, 0.075f, 0.94f);
            GUI.Box(new Rect(1420, 24, 470, 106), GUIContent.none, panel);
            GUI.color = Color.white;
            GUI.Label(new Rect(1440, 40, 430, 32), "OBJETIVO DEL CICLO", title);
            GUI.Label(new Rect(1440, 78, 430, 30), $"Fragmentos: {Mathf.Min(collected, Target)} / {Target}", objective);

            GUI.matrix = previousMatrix;
            GUI.color = previousColor;
        }
    }

    public sealed class CameraFollow : MonoBehaviour
    {
        public Transform Target { get; set; }
        [SerializeField] private float distance = 13f;
        [SerializeField] private float sensitivity = 0.12f;
        private float yaw;
        private float pitch = 24f;

        private void LateUpdate()
        {
            if (Target == null) return;

            if (Mouse.current != null && Mouse.current.rightButton.isPressed)
            {
                Vector2 delta = Mouse.current.delta.ReadValue();
                yaw += delta.x * sensitivity;
                pitch = Mathf.Clamp(pitch - delta.y * sensitivity, 8f, 55f);
                Cursor.lockState = CursorLockMode.Locked;
            }
            else
            {
                Cursor.lockState = CursorLockMode.None;
            }

            Quaternion orbit = Quaternion.Euler(pitch, yaw, 0f);
            Vector3 desired = Target.position + orbit * new Vector3(0f, 0f, -distance);
            transform.position = Vector3.Lerp(transform.position, desired, 8f * Time.deltaTime);
            transform.LookAt(Target.position + Vector3.up * 1.1f);
        }

        private void OnDisable() => Cursor.lockState = CursorLockMode.None;
    }
}
