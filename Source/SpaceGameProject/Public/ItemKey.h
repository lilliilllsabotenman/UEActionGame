#pragma once
#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ItemKey.generated.h"

UCLASS(BlueprintType)
class SPACEGAMEPROJECT_API UItemKey : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "KEY")
    FName ItemID;
};