import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]


class EmergentSystemContractTests(unittest.TestCase):
    def test_llm_bridge_is_server_authoritative_and_text_only(self):
        header = (ROOT / "Source/Omnis/AI/NPC/OmnisNPCDialogueSubsystem.h").read_text(encoding="utf-8")
        source = (ROOT / "Source/Omnis/AI/NPC/OmnisNPCDialogueSubsystem.cpp").read_text(encoding="utf-8")
        self.assertIn("RequestDialogue", header)
        self.assertIn("NPC->HasAuthority()", source)
        self.assertIn("World->GetNetMode() == NM_Client", source)
        for setting in ("OMNIS_LLM_API_KEY", "OMNIS_LLM_ENDPOINT", "OMNIS_LLM_MODEL"):
            self.assertIn(setting, source)
        for limit in ("MaxPendingRequests = 8", "MaxPlayerMessageCharacters = 1200", "MaxResponseCharacters = 1600"):
            self.assertIn(limit, header)
        self.assertIn("no tools", source.lower())
        self.assertIn("RecordMemory", source)
        npc_h = (ROOT / "Source/Omnis/AI/NPC/OmnisNPCCharacter.h").read_text(encoding="utf-8")
        npc_cpp = (ROOT / "Source/Omnis/AI/NPC/OmnisNPCCharacter.cpp").read_text(encoding="utf-8")
        self.assertIn("RequestGeneratedDialogue", npc_h)
        self.assertIn("OnGeneratedDialogue", npc_h)
        self.assertIn("GetSubsystem<UOmnisNPCDialogueSubsystem>", npc_cpp)
        self.assertIn("RequestGeneratedDialogue(", npc_cpp)
        self.assertIn("DOREPLIFETIME(AOmnisNPCCharacter, LastGeneratedDialogue)", npc_cpp)
        economy_h = (ROOT / "Source/Omnis/World/Economy/OmnisEconomySubsystem.h").read_text(encoding="utf-8")
        economy_cpp = (ROOT / "Source/Omnis/World/Economy/OmnisEconomySubsystem.cpp").read_text(encoding="utf-8")
        scheduler_h = (ROOT / "Source/Omnis/World/Economy/OmnisEconomySimulationSubsystem.h").read_text(encoding="utf-8")
        scheduler_cpp = (ROOT / "Source/Omnis/World/Economy/OmnisEconomySimulationSubsystem.cpp").read_text(encoding="utf-8")
        self.assertIn("AdvanceAutonomousDay", economy_h)
        self.assertIn("ProductionPerDay", economy_cpp)
        self.assertIn("ConsumptionPerDay", economy_cpp)
        self.assertIn("GetElapsedSeconds", scheduler_cpp)
        self.assertIn("GetNetMode() != NM_Client", scheduler_cpp)
        self.assertIn("AdvanceAutonomousDay(DaysToCatchUp)", scheduler_cpp)
        self.assertIn("SecondsPerGameDay = 86400.0", scheduler_h)
        interaction_h = (ROOT / "Source/Omnis/Gameplay/Interaction/OmnisInteractiveActor.h").read_text(encoding="utf-8")
        interaction_cpp = (ROOT / "Source/Omnis/Gameplay/Interaction/OmnisInteractiveActor.cpp").read_text(encoding="utf-8")
        self.assertIn("bOneShot = true", interaction_h)
        self.assertIn("InteractionCooldownSeconds", interaction_h)
        self.assertIn("bConsumed", interaction_cpp)
        self.assertIn("HasAuthority()", interaction_cpp)
        self.assertIn("bRewardClaimed", interaction_cpp)
        self.assertIn("OnInteractionActivated.Broadcast", interaction_cpp)
        self.assertIn("DOREPLIFETIME(AOmnisInteractiveActor, bActivated)", interaction_cpp)
        for forbidden in ("ExecuteTrade", "AddItem", "SetRelationship", "CompleteMission"):
            self.assertNotIn(forbidden, source)

    def test_llm_bridge_fails_closed_without_server_configuration(self):
        source = (ROOT / "Source/Omnis/AI/NPC/OmnisNPCDialogueSubsystem.cpp").read_text(encoding="utf-8")
        self.assertIn('!Endpoint.StartsWith(TEXT("https://")', source)
        self.assertIn("ApiKey.IsEmpty()", source)
        self.assertIn("Model.IsEmpty()", source)
        self.assertIn("provider is not configured", source)
        self.assertIn("The character has nothing more to say", source)

    def test_emergent_contract_documents_deterministic_owners_and_release_gates(self):
        doc = (ROOT / "Docs/OMNIS_EMERGENT_SYSTEMS.md").read_text(encoding="utf-8")
        for required in ("UOmnisCharacterStateComponent", "UOmnisEconomySubsystem", "Caracas",
                         "La Guaira", "Barlovento", "native certification", "Chaos Vehicles",
                         "digital elevation model"):
            self.assertIn(required.lower(), doc.lower())


if __name__ == "__main__":
    unittest.main()
