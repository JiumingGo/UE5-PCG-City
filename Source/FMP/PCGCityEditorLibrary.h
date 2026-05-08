// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "PCGCityEditorLibrary.generated.h"

/**
 * 
 */
UCLASS()
class FMP_API UPCGCityEditorLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="PCG City|Editor Placement")
	static bool GetEditorMousePlacementLocation(
	AActor* ActorToIgnore,
	FVector& OutLocation,
	FVector& OutNormal,
	bool& bHitSurface,
	bool& bLeftMouseDown,
	float TraceDistance = 1000000.0f,
	float SurfaceOffset = 2.0f
	);

	UFUNCTION(BlueprintCallable, Category="PCG City|Editor Placement")
	static bool IsEditorViewportLeftMouseDown();
};
