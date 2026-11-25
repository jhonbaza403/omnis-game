#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "OmnisEconomySubsystem.generated.h"

USTRUCT(BlueprintType)
struct FOmnisEconomyMarketEntry
{
    GENERATED_BODY()

    UPROPERTY(SaveGame, BlueprintReadWrite, Category="Omnis|Economy")
    FName MarketId;

    UPROPERTY(SaveGame, BlueprintReadWrite, Category="Omnis|Economy")
    FName RegionId;

    UPROPERTY(SaveGame, BlueprintReadWrite, Category="Omnis|Economy")
    FName ItemId;

    UPROPERTY(SaveGame, BlueprintReadWrite, Category="Omnis|Economy")
    FName CurrencyId;

    // All prices are integer minor units (for example, cents or centimos).
    UPROPERTY(SaveGame, BlueprintReadWrite, Category="Omnis|Economy", meta=(ClampMin="1"))
    int64 BasePriceMinorUnits = 100;

    UPROPERTY(SaveGame, BlueprintReadWrite, Category="Omnis|Economy", meta=(ClampMin="0"))
    int32 StockQuantity = 0;

    UPROPERTY(SaveGame, BlueprintReadWrite, Category="Omnis|Economy", meta=(ClampMin="0"))
    int32 DailyDemand = 1;

    UPROPERTY(SaveGame, BlueprintReadWrite, Category="Omnis|Economy")
    bool bPubliclyVisible = true;
};

USTRUCT(BlueprintType)
struct FOmnisEconomyWalletEntry
{
    GENERATED_BODY()

    UPROPERTY(SaveGame, BlueprintReadWrite, Category="Omnis|Economy")
    FName AccountId;

    UPROPERTY(SaveGame, BlueprintReadWrite, Category="Omnis|Economy")
    FName CurrencyId;

    UPROPERTY(SaveGame, BlueprintReadWrite, Category="Omnis|Economy", meta=(ClampMin="0"))
    int64 BalanceMinorUnits = 0;
};

USTRUCT(BlueprintType)
struct FOmnisEconomySnapshot
{
    GENERATED_BODY()

    UPROPERTY(SaveGame, BlueprintReadWrite, Category="Omnis|Economy")
    TArray<FOmnisEconomyMarketEntry> Markets;

    UPROPERTY(SaveGame, BlueprintReadWrite, Category="Omnis|Economy")
    TArray<FOmnisEconomyWalletEntry> Wallets;
};

USTRUCT(BlueprintType)
struct FOmnisMarketQuote
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category="Omnis|Economy")
    bool bAvailable = false;

    UPROPERTY(BlueprintReadOnly, Category="Omnis|Economy")
    FName MarketId;

    UPROPERTY(BlueprintReadOnly, Category="Omnis|Economy")
    FName RegionId;

    UPROPERTY(BlueprintReadOnly, Category="Omnis|Economy")
    FName ItemId;

    UPROPERTY(BlueprintReadOnly, Category="Omnis|Economy")
    FName CurrencyId;

    UPROPERTY(BlueprintReadOnly, Category="Omnis|Economy")
    int64 UnitPriceMinorUnits = 0;

    UPROPERTY(BlueprintReadOnly, Category="Omnis|Economy")
    int32 StockQuantity = 0;

    UPROPERTY(BlueprintReadOnly, Category="Omnis|Economy")
    int32 PriceIndexBasisPoints = 10000;
};

USTRUCT(BlueprintType)
struct FOmnisTradeReceipt
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category="Omnis|Economy")
    FName AccountId;

    UPROPERTY(BlueprintReadOnly, Category="Omnis|Economy")
    FName MarketId;

    UPROPERTY(BlueprintReadOnly, Category="Omnis|Economy")
    FName ItemId;

    UPROPERTY(BlueprintReadOnly, Category="Omnis|Economy")
    FName CurrencyId;

    UPROPERTY(BlueprintReadOnly, Category="Omnis|Economy")
    int32 Quantity = 0;

    UPROPERTY(BlueprintReadOnly, Category="Omnis|Economy")
    int64 UnitPriceMinorUnits = 0;

    UPROPERTY(BlueprintReadOnly, Category="Omnis|Economy")
    int64 TotalMinorUnits = 0;

    UPROPERTY(BlueprintReadOnly, Category="Omnis|Economy")
    bool bPlayerBought = false;
};

UCLASS()
class OMNIS_API UOmnisEconomySubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="Omnis|Economy")
    void RegisterVenezuelaTriangleMarkets(int32 ThroughPhase = 1);

    UFUNCTION(BlueprintCallable, Category="Omnis|Economy")
    void AdvanceAutonomousDay(int32 DaysToSimulate = 1);

    UFUNCTION(BlueprintCallable, Category="Omnis|Economy")
    bool RegisterMarketCommodity(
        FName MarketId,
        FName RegionId,
        FName ItemId,
        FName CurrencyId,
        int64 BasePriceMinorUnits,
        int32 InitialStock,
        int32 DailyDemand,
        bool bPubliclyVisible = true);

    UFUNCTION(BlueprintPure, Category="Omnis|Economy")
    FOmnisMarketQuote GetQuote(FName MarketId, FName ItemId) const;

    UFUNCTION(BlueprintPure, Category="Omnis|Economy")
    TArray<FOmnisMarketQuote> GetPublicMarketQuotes() const;

    UFUNCTION(BlueprintCallable, Category="Omnis|Economy")
    int32 ApplyRouteSupplyShock(
        FName RouteId,
        const TArray<FName>& ItemIds,
        int32 SupplyDelta,
        int32 DemandDelta);

    UFUNCTION(BlueprintCallable, Category="Omnis|Economy")
    bool AdjustMarketConditions(
        FName MarketId,
        FName ItemId,
        int32 SupplyDelta,
        int32 DemandDelta);

    UFUNCTION(BlueprintCallable, Category="Omnis|Economy")
    void SetAccountBalance(FName AccountId, FName CurrencyId, int64 BalanceMinorUnits);

    UFUNCTION(BlueprintPure, Category="Omnis|Economy")
    int64 GetAccountBalance(FName AccountId, FName CurrencyId) const;

    UFUNCTION(BlueprintCallable, Category="Omnis|Economy")
    bool ExecuteTrade(
        FName AccountId,
        FName MarketId,
        FName ItemId,
        int32 Quantity,
        bool bPlayerBuys,
        class UOmnisInventoryComponent* Inventory,
        FOmnisTradeReceipt& OutReceipt);

    UFUNCTION(BlueprintCallable, Category="Omnis|Economy")
    bool ExecuteBarter(
        FName MarketId,
        FName OfferedItemId,
        int32 OfferedQuantity,
        FName RequestedItemId,
        int32 RequestedQuantity,
        class UOmnisInventoryComponent* Inventory);

    UFUNCTION(BlueprintPure, Category="Omnis|Economy")
    FOmnisEconomySnapshot ExportState() const;

    UFUNCTION(BlueprintCallable, Category="Omnis|Economy")
    void ImportState(const FOmnisEconomySnapshot& Snapshot);

private:
    bool HasMutationAuthority() const;
    void RefreshMarketReplication() const;
    static FName MakeMarketKey(FName MarketId, FName ItemId);
    static FName MakeWalletKey(FName AccountId, FName CurrencyId);
    static int64 CalculateUnitPrice(const FOmnisEconomyMarketEntry& Entry, int32& OutPriceIndexBasisPoints);

    UPROPERTY(SaveGame)
    TMap<FName, FOmnisEconomyMarketEntry> Markets;

    UPROPERTY(SaveGame)
    TMap<FName, int64> WalletBalances;
};
