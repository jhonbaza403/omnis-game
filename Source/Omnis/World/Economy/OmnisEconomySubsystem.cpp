#include "World/Economy/OmnisEconomySubsystem.h"

#include "Gameplay/Inventory/OmnisInventoryComponent.h"
#include "GameFramework/Actor.h"
#include "EngineUtils.h"
#include "Multiplayer/World/OmnisReplicatedWorldStateActor.h"
#include "Engine/World.h"

namespace
{
    constexpr int32 MaxMarketQuantity = 1000000000;
    constexpr int64 MaxBasePriceMinorUnits = 1000000000LL;
    constexpr int64 MaxWalletBalanceMinorUnits = 1000000000000000LL;
    constexpr int32 MinimumPriceIndexBasisPoints = 2500;
    constexpr int32 MaximumPriceIndexBasisPoints = 40000;
}

bool UOmnisEconomySubsystem::HasMutationAuthority() const
{
    const UWorld* World = GetWorld();
    return !World || World->GetNetMode() != NM_Client;
}

FName UOmnisEconomySubsystem::MakeMarketKey(FName MarketId, FName ItemId)
{
    if (MarketId.IsNone() || ItemId.IsNone())
    {
        return NAME_None;
    }

    return FName(*FString::Printf(
        TEXT("%s::%s"),
        *MarketId.ToString(),
        *ItemId.ToString()));
}

FName UOmnisEconomySubsystem::MakeWalletKey(FName AccountId, FName CurrencyId)
{
    if (AccountId.IsNone() || CurrencyId.IsNone())
    {
        return NAME_None;
    }

    return FName(*FString::Printf(
        TEXT("%s::%s"),
        *AccountId.ToString(),
        *CurrencyId.ToString()));
}

void UOmnisEconomySubsystem::RegisterVenezuelaTriangleMarkets(int32 ThroughPhase)
{
    if (!HasMutationAuthority())
    {
        return;
    }

    struct FMarketSeed
    {
        const TCHAR* MarketId;
        const TCHAR* RegionId;
        const TCHAR* ItemId;
        const TCHAR* CurrencyId;
        int64 BasePriceMinorUnits;
        int32 Stock;
        int32 DailyDemand;
        bool bPublic;
        int32 Phase = 1;
    };

    // Game-economy units, not real-world price quotations.
    static const FMarketSeed Seeds[] =
    {
        {TEXT("Market.Caracas.Formal"), TEXT("Region.Caracas"), TEXT("Item.Food"), TEXT("Currency.VES"), 5000, 300, 120, true},
        {TEXT("Market.Caracas.Formal"), TEXT("Region.Caracas"), TEXT("Item.Fuel"), TEXT("Currency.VES"), 8500, 100, 160, true},
        {TEXT("Market.Caracas.Formal"), TEXT("Region.Caracas"), TEXT("Item.VehicleParts"), TEXT("Currency.VES"), 42000, 40, 80, true},
        {TEXT("Market.Caracas.Formal"), TEXT("Region.Caracas"), TEXT("Item.Medicine"), TEXT("Currency.VES"), 12500, 80, 60, true},
        {TEXT("Market.Caracas.Formal"), TEXT("Region.Caracas"), TEXT("Item.BuildingMaterials"), TEXT("Currency.VES"), 18000, 250, 90, true},
        {TEXT("Market.Caracas.Formal"), TEXT("Region.Caracas"), TEXT("Item.Electronics"), TEXT("Currency.VES"), 95000, 60, 40, true},

        {TEXT("Market.Caracas.Black"), TEXT("Region.Caracas"), TEXT("Item.Fuel"), TEXT("Currency.USD"), 9200, 35, 100, false},
        {TEXT("Market.Caracas.Black"), TEXT("Region.Caracas"), TEXT("Item.VehicleParts"), TEXT("Currency.USD"), 62000, 15, 70, false},
        {TEXT("Market.Caracas.Black"), TEXT("Region.Caracas"), TEXT("Item.Medicine"), TEXT("Currency.USD"), 28000, 8, 50, false},

        {TEXT("Market.GuarenasGuatire.Formal"), TEXT("Region.GuarenasGuatire"), TEXT("Item.Food"), TEXT("Currency.VES"), 4800, 180, 100, true},
        {TEXT("Market.GuarenasGuatire.Formal"), TEXT("Region.GuarenasGuatire"), TEXT("Item.Fuel"), TEXT("Currency.VES"), 9000, 65, 125, true},
        {TEXT("Market.GuarenasGuatire.Formal"), TEXT("Region.GuarenasGuatire"), TEXT("Item.VehicleParts"), TEXT("Currency.VES"), 46000, 22, 60, true},

        {TEXT("Market.Caucagua.Rural"), TEXT("Region.Barlovento"), TEXT("Item.AgriculturalProduce"), TEXT("Currency.VES"), 2600, 450, 90, true},
        {TEXT("Market.Caucagua.Rural"), TEXT("Region.Barlovento"), TEXT("Item.Food"), TEXT("Currency.VES"), 3800, 260, 110, true},
        {TEXT("Market.Caucagua.Rural"), TEXT("Region.Barlovento"), TEXT("Item.Fuel"), TEXT("Currency.VES"), 9800, 28, 95, true},

        {TEXT("Market.RioChico.Coastal"), TEXT("Region.Barlovento"), TEXT("Item.FreshFish"), TEXT("Currency.VES"), 4200, 280, 100, true},
        {TEXT("Market.RioChico.Coastal"), TEXT("Region.Barlovento"), TEXT("Item.Food"), TEXT("Currency.VES"), 5100, 90, 105, true},
        {TEXT("Market.RioChico.Coastal"), TEXT("Region.Barlovento"), TEXT("Item.BoatParts"), TEXT("Currency.VES"), 38000, 22, 60, true},

        {TEXT("Market.Higuerote.Coastal"), TEXT("Region.Barlovento"), TEXT("Item.FreshFish"), TEXT("Currency.VES"), 3900, 320, 110, true},
        {TEXT("Market.Higuerote.Coastal"), TEXT("Region.Barlovento"), TEXT("Item.Fuel"), TEXT("Currency.VES"), 10500, 55, 125, true},
        {TEXT("Market.Higuerote.Coastal"), TEXT("Region.Barlovento"), TEXT("Item.BoatParts"), TEXT("Currency.VES"), 36000, 30, 55, true},

        {TEXT("Market.LaGuaira.Port"), TEXT("Region.LaGuaira"), TEXT("Item.Fuel"), TEXT("Currency.USD"), 7200, 250, 130, true, 2},
        {TEXT("Market.LaGuaira.Port"), TEXT("Region.LaGuaira"), TEXT("Item.VehicleParts"), TEXT("Currency.USD"), 39000, 120, 100, true, 2},
        {TEXT("Market.LaGuaira.Port"), TEXT("Region.LaGuaira"), TEXT("Item.Vehicles"), TEXT("Currency.USD"), 3200000, 20, 25, true, 2},
        {TEXT("Market.LaGuaira.Port"), TEXT("Region.LaGuaira"), TEXT("Item.Electronics"), TEXT("Currency.USD"), 88000, 100, 70, true, 2}
    };

    const int32 SafeThroughPhase = FMath::Clamp(ThroughPhase, 1, 3);
    bool bAddedAnyMarket = false;
    for (const FMarketSeed& Seed : Seeds)
    {
        if (Seed.Phase > SafeThroughPhase)
        {
            continue;
        }
        const FName MarketId(Seed.MarketId);
        const FName RegionId(Seed.RegionId);
        const FName ItemId(Seed.ItemId);
        const FName Key = MakeMarketKey(MarketId, ItemId);
        if (Key.IsNone() || Markets.Contains(Key))
        {
            continue;
        }

        FOmnisEconomyMarketEntry Entry;
        Entry.MarketId = MarketId;
        Entry.RegionId = RegionId;
        Entry.ItemId = ItemId;
        Entry.CurrencyId = FName(Seed.CurrencyId);
        Entry.BasePriceMinorUnits = FMath::Clamp<int64>(
            Seed.BasePriceMinorUnits, 1, MaxBasePriceMinorUnits);
        Entry.StockQuantity = FMath::Clamp(Seed.Stock, 0, MaxMarketQuantity);
        Entry.DailyDemand = FMath::Clamp(Seed.DailyDemand, 0, MaxMarketQuantity);
        Entry.bPubliclyVisible = Seed.bPublic;
        Markets.Add(Key, Entry);
        bAddedAnyMarket = true;
    }

    if (bAddedAnyMarket)
    {
        RefreshMarketReplication();
    }
}

bool UOmnisEconomySubsystem::RegisterMarketCommodity(
    FName MarketId,
    FName RegionId,
    FName ItemId,
    FName CurrencyId,
    int64 BasePriceMinorUnits,
    int32 InitialStock,
    int32 DailyDemand,
    bool bPubliclyVisible)
{
    if (!HasMutationAuthority() ||
        MarketId.IsNone() ||
        RegionId.IsNone() ||
        ItemId.IsNone() ||
        CurrencyId.IsNone() ||
        BasePriceMinorUnits <= 0 ||
        InitialStock < 0 ||
        DailyDemand < 0)
    {
        return false;
    }

    const FName Key = MakeMarketKey(MarketId, ItemId);
    if (Key.IsNone())
    {
        return false;
    }

    // Registration is idempotent: rebuilding a streamed area must not reset
    // a market that has already changed through player or mission activity.
    if (Markets.Contains(Key))
    {
        return true;
    }

    FOmnisEconomyMarketEntry Entry;
    Entry.MarketId = MarketId;
    Entry.RegionId = RegionId;
    Entry.ItemId = ItemId;
    Entry.CurrencyId = CurrencyId;
    Entry.BasePriceMinorUnits = FMath::Clamp<int64>(
        BasePriceMinorUnits,
        1,
        MaxBasePriceMinorUnits);
    Entry.StockQuantity = FMath::Clamp(InitialStock, 0, MaxMarketQuantity);
    Entry.DailyDemand = FMath::Clamp(DailyDemand, 0, MaxMarketQuantity);
    Entry.bPubliclyVisible = bPubliclyVisible;

    Markets.Add(Key, Entry);
    RefreshMarketReplication();
    return true;
}


void UOmnisEconomySubsystem::AdvanceAutonomousDay(int32 DaysToSimulate)
{
    if (!HasMutationAuthority())
    {
        return;
    }

    struct FProductionFlow
    {
        const TCHAR* MarketId;
        const TCHAR* ItemId;
        int32 ProductionPerDay;
        int32 ConsumptionPerDay;
    };

    // Abstract game-economy units per in-game day, not real-world statistics.
    static const FProductionFlow Flows[] =
    {
        {TEXT("Market.Caracas.Formal"), TEXT("Item.Food"), 45, 60},
        {TEXT("Market.Caracas.Formal"), TEXT("Item.Fuel"), 0, 45},
        {TEXT("Market.Caracas.Formal"), TEXT("Item.Medicine"), 5, 8},
        {TEXT("Market.GuarenasGuatire.Formal"), TEXT("Item.Food"), 30, 25},
        {TEXT("Market.GuarenasGuatire.Formal"), TEXT("Item.Fuel"), 8, 15},
        {TEXT("Market.Caucagua.Rural"), TEXT("Item.AgriculturalProduce"), 65, 25},
        {TEXT("Market.Caucagua.Rural"), TEXT("Item.Food"), 25, 30},
        {TEXT("Market.RioChico.Coastal"), TEXT("Item.FreshFish"), 50, 35},
        {TEXT("Market.RioChico.Coastal"), TEXT("Item.Food"), 20, 22},
        {TEXT("Market.Higuerote.Coastal"), TEXT("Item.FreshFish"), 40, 30},
        {TEXT("Market.Higuerote.Coastal"), TEXT("Item.Fuel"), 5, 14},
        {TEXT("Market.LaGuaira.Port"), TEXT("Item.Fuel"), 30, 24},
        {TEXT("Market.LaGuaira.Port"), TEXT("Item.Electronics"), 12, 7}
    };

    const int32 SafeDays = FMath::Clamp(DaysToSimulate, 1, 7);
    for (int32 Day = 0; Day < SafeDays; ++Day)
    {
        for (const FProductionFlow& Flow : Flows)
        {
            const int32 NetStock = Flow.ProductionPerDay - Flow.ConsumptionPerDay;
            if (NetStock != 0)
            {
                AdjustMarketConditions(
                    FName(Flow.MarketId),
                    FName(Flow.ItemId),
                    NetStock,
                    0);
            }
        }
    }
}

int64 UOmnisEconomySubsystem::CalculateUnitPrice(
    const FOmnisEconomyMarketEntry& Entry,
    int32& OutPriceIndexBasisPoints)
{
    const int64 Stock = FMath::Clamp<int64>(
        Entry.StockQuantity,
        0,
        MaxMarketQuantity);
    const int64 Demand = FMath::Clamp<int64>(
        Entry.DailyDemand,
        0,
        MaxMarketQuantity);

    if (Demand == 0)
    {
        OutPriceIndexBasisPoints = MinimumPriceIndexBasisPoints;
    }
    else
    {
        const int64 Denominator = FMath::Max<int64>(1, Stock + Demand);
        const int64 RawIndex = (2LL * Demand * 10000LL) / Denominator;
        OutPriceIndexBasisPoints = static_cast<int32>(FMath::Clamp<int64>(
            RawIndex,
            MinimumPriceIndexBasisPoints,
            MaximumPriceIndexBasisPoints));
    }

    const int64 SafeBasePrice = FMath::Clamp<int64>(
        Entry.BasePriceMinorUnits,
        1,
        MaxBasePriceMinorUnits);

    return FMath::Max<int64>(
        1,
        (SafeBasePrice * OutPriceIndexBasisPoints + 5000LL) / 10000LL);
}

FOmnisMarketQuote UOmnisEconomySubsystem::GetQuote(FName MarketId, FName ItemId) const
{
    FOmnisMarketQuote Quote;
    const FName Key = MakeMarketKey(MarketId, ItemId);
    const FOmnisEconomyMarketEntry* Entry = Markets.Find(Key);
    if (!Entry)
    {
        return Quote;
    }

    Quote.bAvailable = true;
    Quote.MarketId = Entry->MarketId;
    Quote.RegionId = Entry->RegionId;
    Quote.ItemId = Entry->ItemId;
    Quote.CurrencyId = Entry->CurrencyId;
    Quote.StockQuantity = Entry->StockQuantity;
    Quote.UnitPriceMinorUnits = CalculateUnitPrice(
        *Entry,
        Quote.PriceIndexBasisPoints);
    return Quote;
}



TArray<FOmnisMarketQuote> UOmnisEconomySubsystem::GetPublicMarketQuotes() const
{
    TArray<FOmnisMarketQuote> Quotes;
    for (const TPair<FName, FOmnisEconomyMarketEntry>& Pair : Markets)
    {
        if (!Pair.Value.bPubliclyVisible)
        {
            continue;
        }

        Quotes.Add(GetQuote(Pair.Value.MarketId, Pair.Value.ItemId));
    }

    Quotes.Sort([](const FOmnisMarketQuote& A, const FOmnisMarketQuote& B)
    {
        if (A.MarketId != B.MarketId)
        {
            return A.MarketId.Compare(B.MarketId) < 0;
        }
        return A.ItemId.Compare(B.ItemId) < 0;
    });
    return Quotes;
}

void UOmnisEconomySubsystem::RefreshMarketReplication() const
{
    UWorld* World = GetWorld();
    if (!World || World->GetNetMode() == NM_Client)
    {
        return;
    }

    for (TActorIterator<AOmnisReplicatedWorldStateActor> It(World); It; ++It)
    {
        if (*It)
        {
            It->RefreshNarrativeState();
            break;
        }
    }
}

int32 UOmnisEconomySubsystem::ApplyRouteSupplyShock(
    FName RouteId,
    const TArray<FName>& ItemIds,
    int32 SupplyDelta,
    int32 DemandDelta)
{
    if (!HasMutationAuthority() ||
        RouteId.IsNone() ||
        ItemIds.IsEmpty() ||
        (SupplyDelta == 0 && DemandDelta == 0))
    {
        return 0;
    }

    TArray<FName> LinkedMarkets;
    if (RouteId == TEXT("Route.CaracasEast-Guarenas-Guatire-Caucagua"))
    {
        LinkedMarkets = {
            TEXT("Market.Caracas.Formal"),
            TEXT("Market.GuarenasGuatire.Formal"),
            TEXT("Market.Caucagua.Rural")
        };
    }
    else if (RouteId == TEXT("Route.Caucagua-RioChico-Higuerote"))
    {
        LinkedMarkets = {
            TEXT("Market.Caucagua.Rural"),
            TEXT("Market.RioChico.Coastal"),
            TEXT("Market.Higuerote.Coastal")
        };
    }
    else if (RouteId == TEXT("Route.Caracas-LaGuaira.TunnelHighway") ||
        RouteId == TEXT("Route.Caracas-LaGuaira.OldRoad"))
    {
        LinkedMarkets = {
            TEXT("Market.Caracas.Formal"),
            TEXT("Market.LaGuaira.Port")
        };
    }
    else if (RouteId == TEXT("Route.Naiguata-Todasana-Chuspa-Higuerote"))
    {
        LinkedMarkets = {
            TEXT("Market.LaGuaira.Port"),
            TEXT("Market.Higuerote.Coastal")
        };
    }
    else if (RouteId == TEXT("Route.LaGuaira-Higuerote.SeaLane"))
    {
        LinkedMarkets = {
            TEXT("Market.LaGuaira.Port"),
            TEXT("Market.Higuerote.Coastal")
        };
    }
    else
    {
        return 0;
    }

    int32 UpdatedMarketItems = 0;
    for (const FName MarketId : LinkedMarkets)
    {
        for (const FName ItemId : ItemIds)
        {
            if (AdjustMarketConditions(
                MarketId,
                ItemId,
                SupplyDelta,
                DemandDelta))
            {
                ++UpdatedMarketItems;
            }
        }
    }

    return UpdatedMarketItems;
}

bool UOmnisEconomySubsystem::AdjustMarketConditions(
    FName MarketId,
    FName ItemId,
    int32 SupplyDelta,
    int32 DemandDelta)
{
    if (!HasMutationAuthority())
    {
        return false;
    }

    FOmnisEconomyMarketEntry* Entry = Markets.Find(MakeMarketKey(MarketId, ItemId));
    if (!Entry)
    {
        return false;
    }

    Entry->StockQuantity = static_cast<int32>(FMath::Clamp<int64>(
        static_cast<int64>(Entry->StockQuantity) + SupplyDelta,
        0,
        MaxMarketQuantity));
    Entry->DailyDemand = static_cast<int32>(FMath::Clamp<int64>(
        static_cast<int64>(Entry->DailyDemand) + DemandDelta,
        0,
        MaxMarketQuantity));
    RefreshMarketReplication();
    return true;
}

void UOmnisEconomySubsystem::SetAccountBalance(
    FName AccountId,
    FName CurrencyId,
    int64 BalanceMinorUnits)
{
    if (!HasMutationAuthority())
    {
        return;
    }

    const FName Key = MakeWalletKey(AccountId, CurrencyId);
    if (!Key.IsNone())
    {
        WalletBalances.Add(
            Key,
            FMath::Clamp<int64>(BalanceMinorUnits, 0, MaxWalletBalanceMinorUnits));
    }
}

int64 UOmnisEconomySubsystem::GetAccountBalance(
    FName AccountId,
    FName CurrencyId) const
{
    const FName Key = MakeWalletKey(AccountId, CurrencyId);
    return Key.IsNone() ? 0 : WalletBalances.FindRef(Key);
}

bool UOmnisEconomySubsystem::ExecuteTrade(
    FName AccountId,
    FName MarketId,
    FName ItemId,
    int32 Quantity,
    bool bPlayerBuys,
    UOmnisInventoryComponent* Inventory,
    FOmnisTradeReceipt& OutReceipt)
{
    OutReceipt = FOmnisTradeReceipt{};

    if (!HasMutationAuthority() ||
        AccountId.IsNone() ||
        !Inventory ||
        !Inventory->GetOwner() ||
        !Inventory->GetOwner()->HasAuthority() ||
        Quantity <= 0 ||
        Quantity > 100000)
    {
        return false;
    }

    FOmnisEconomyMarketEntry* Entry = Markets.Find(MakeMarketKey(MarketId, ItemId));
    if (!Entry)
    {
        return false;
    }

    int32 PriceIndexBasisPoints = 10000;
    const int64 UnitPrice = CalculateUnitPrice(*Entry, PriceIndexBasisPoints);
    const int64 Total = UnitPrice * static_cast<int64>(Quantity);
    const FName WalletKey = MakeWalletKey(AccountId, Entry->CurrencyId);
    if (WalletKey.IsNone())
    {
        return false;
    }

    const int64 CurrentBalance = WalletBalances.FindRef(WalletKey);

    if (bPlayerBuys)
    {
        if (Entry->StockQuantity < Quantity ||
            CurrentBalance < Total ||
            !Inventory->AddItem(ItemId, Quantity))
        {
            return false;
        }

        WalletBalances.Add(WalletKey, CurrentBalance - Total);
        Entry->StockQuantity -= Quantity;
    }
    else
    {
        if (Inventory->GetQuantity(ItemId) < Quantity ||
            CurrentBalance > MaxWalletBalanceMinorUnits - Total ||
            !Inventory->RemoveItem(ItemId, Quantity))
        {
            return false;
        }

        WalletBalances.Add(WalletKey, CurrentBalance + Total);
        Entry->StockQuantity = static_cast<int32>(FMath::Clamp<int64>(
            static_cast<int64>(Entry->StockQuantity) + Quantity,
            0,
            MaxMarketQuantity));
    }

    OutReceipt.AccountId = AccountId;
    OutReceipt.MarketId = MarketId;
    OutReceipt.ItemId = ItemId;
    OutReceipt.CurrencyId = Entry->CurrencyId;
    OutReceipt.Quantity = Quantity;
    OutReceipt.UnitPriceMinorUnits = UnitPrice;
    OutReceipt.TotalMinorUnits = Total;
    OutReceipt.bPlayerBought = bPlayerBuys;
    RefreshMarketReplication();
    return true;
}

bool UOmnisEconomySubsystem::ExecuteBarter(
    FName MarketId,
    FName OfferedItemId,
    int32 OfferedQuantity,
    FName RequestedItemId,
    int32 RequestedQuantity,
    UOmnisInventoryComponent* Inventory)
{
    if (!HasMutationAuthority() ||
        !Inventory ||
        !Inventory->GetOwner() ||
        !Inventory->GetOwner()->HasAuthority() ||
        OfferedItemId.IsNone() ||
        RequestedItemId.IsNone() ||
        OfferedItemId == RequestedItemId ||
        OfferedQuantity <= 0 ||
        RequestedQuantity <= 0 ||
        OfferedQuantity > 100000 ||
        RequestedQuantity > 100000 ||
        Inventory->GetQuantity(OfferedItemId) < OfferedQuantity)
    {
        return false;
    }

    FOmnisEconomyMarketEntry* OfferedMarket =
        Markets.Find(MakeMarketKey(MarketId, OfferedItemId));
    FOmnisEconomyMarketEntry* RequestedMarket =
        Markets.Find(MakeMarketKey(MarketId, RequestedItemId));
    if (!OfferedMarket || !RequestedMarket ||
        OfferedMarket->CurrencyId != RequestedMarket->CurrencyId ||
        RequestedMarket->StockQuantity < RequestedQuantity)
    {
        return false;
    }

    int32 OfferedIndex = 10000;
    int32 RequestedIndex = 10000;
    const int64 OfferedValue =
        CalculateUnitPrice(*OfferedMarket, OfferedIndex) * OfferedQuantity;
    const int64 RequestedValue =
        CalculateUnitPrice(*RequestedMarket, RequestedIndex) * RequestedQuantity;

    // Barter never creates value: the offered basket must cover the requested one.
    if (OfferedValue < RequestedValue)
    {
        return false;
    }

    if (!Inventory->RemoveItem(OfferedItemId, OfferedQuantity))
    {
        return false;
    }

    if (!Inventory->AddItem(RequestedItemId, RequestedQuantity))
    {
        // Restore the offered goods if the receiving inventory cannot accept them.
        Inventory->AddItem(OfferedItemId, OfferedQuantity);
        return false;
    }

    OfferedMarket->StockQuantity = static_cast<int32>(FMath::Clamp<int64>(
        static_cast<int64>(OfferedMarket->StockQuantity) + OfferedQuantity,
        0,
        MaxMarketQuantity));
    RequestedMarket->StockQuantity -= RequestedQuantity;
    RefreshMarketReplication();
    return true;
}

FOmnisEconomySnapshot UOmnisEconomySubsystem::ExportState() const
{
    FOmnisEconomySnapshot Snapshot;
    Snapshot.Markets.Reserve(Markets.Num());
    Snapshot.Wallets.Reserve(WalletBalances.Num());

    for (const TPair<FName, FOmnisEconomyMarketEntry>& Pair : Markets)
    {
        Snapshot.Markets.Add(Pair.Value);
    }

    for (const TPair<FName, int64>& Pair : WalletBalances)
    {
        const FString KeyString = Pair.Key.ToString();
        FString AccountPart;
        FString CurrencyPart;
        if (!KeyString.Split(TEXT("::"), &AccountPart, &CurrencyPart))
        {
            continue;
        }

        FOmnisEconomyWalletEntry Wallet;
        Wallet.AccountId = FName(*AccountPart);
        Wallet.CurrencyId = FName(*CurrencyPart);
        Wallet.BalanceMinorUnits = Pair.Value;
        Snapshot.Wallets.Add(Wallet);
    }

    Snapshot.Markets.Sort([](const FOmnisEconomyMarketEntry& A, const FOmnisEconomyMarketEntry& B)
    {
        if (A.MarketId != B.MarketId)
        {
            return A.MarketId.Compare(B.MarketId) < 0;
        }
        return A.ItemId.Compare(B.ItemId) < 0;
    });
    Snapshot.Wallets.Sort([](const FOmnisEconomyWalletEntry& A, const FOmnisEconomyWalletEntry& B)
    {
        if (A.AccountId != B.AccountId)
        {
            return A.AccountId.Compare(B.AccountId) < 0;
        }
        return A.CurrencyId.Compare(B.CurrencyId) < 0;
    });
    return Snapshot;
}

void UOmnisEconomySubsystem::ImportState(const FOmnisEconomySnapshot& Snapshot)
{
    if (!HasMutationAuthority())
    {
        return;
    }

    // Older save schemas have no market snapshot. Preserve the map's initial
    // seed catalog in that case instead of wiping its stock and prices.
    if (!Snapshot.Markets.IsEmpty())
    {
        Markets.Reset();
    }
    WalletBalances.Reset();

    for (const FOmnisEconomyMarketEntry& Entry : Snapshot.Markets)
    {
        if (Entry.MarketId.IsNone() ||
            Entry.RegionId.IsNone() ||
            Entry.ItemId.IsNone() ||
            Entry.CurrencyId.IsNone() ||
            Entry.BasePriceMinorUnits <= 0 ||
            Entry.StockQuantity < 0 ||
            Entry.DailyDemand < 0)
        {
            continue;
        }

        const FName Key = MakeMarketKey(Entry.MarketId, Entry.ItemId);
        if (!Key.IsNone())
        {
            FOmnisEconomyMarketEntry SafeEntry = Entry;
            SafeEntry.BasePriceMinorUnits = FMath::Clamp<int64>(
                SafeEntry.BasePriceMinorUnits,
                1,
                MaxBasePriceMinorUnits);
            SafeEntry.StockQuantity = FMath::Clamp(
                SafeEntry.StockQuantity,
                0,
                MaxMarketQuantity);
            SafeEntry.DailyDemand = FMath::Clamp(
                SafeEntry.DailyDemand,
                0,
                MaxMarketQuantity);
            Markets.Add(Key, SafeEntry);
        }
    }

    for (const FOmnisEconomyWalletEntry& Wallet : Snapshot.Wallets)
    {
        const FName Key = MakeWalletKey(Wallet.AccountId, Wallet.CurrencyId);
        if (!Key.IsNone() && Wallet.BalanceMinorUnits >= 0)
        {
            WalletBalances.Add(
                Key,
                FMath::Clamp<int64>(
                    Wallet.BalanceMinorUnits,
                    0,
                    MaxWalletBalanceMinorUnits));
        }
    }
    RefreshMarketReplication();
}
