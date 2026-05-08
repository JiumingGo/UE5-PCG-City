// Fill out your copyright notice in the Description page of Project Settings.
#include "PCGCityEditorLibrary.h"

#if WITH_EDITOR
#include "Editor.h"
#include "EditorViewportClient.h"
#include "SceneView.h"
#include "Engine/World.h"
#include "CollisionQueryParams.h"
#include "InputCoreTypes.h"
#endif

bool UPCGCityEditorLibrary::GetEditorMousePlacementLocation(
	AActor* ActorToIgnore,
	FVector& OutLocation,
	FVector& OutNormal,
	bool& bHitSurface,
	bool& bLeftMouseDown,
	float TraceDistance,
	float SurfaceOffset
)
{
	OutLocation = FVector::ZeroVector;
	OutNormal = FVector::UpVector;
	bHitSurface = false;
	bLeftMouseDown = false;

	#if WITH_EDITOR
	if (!GEditor){return false;}

	FViewport* ActiveViewport = GEditor->GetActiveViewport();
	if (!ActiveViewport){ return false;}

	bLeftMouseDown = ActiveViewport->KeyState(EKeys::LeftMouseButton);

	FViewportClient* RawViewportClient = ActiveViewport->GetClient();
	if (!RawViewportClient){ return false;}

	FEditorViewportClient* EditorViewportClient = static_cast<FEditorViewportClient*>(RawViewportClient);
	if (!EditorViewportClient){ return false;}

	UWorld* World = EditorViewportClient->GetWorld();
	if (!World){ return false;}

	FSceneViewFamilyContext ViewFamily(
		FSceneViewFamily::ConstructionValues(
			ActiveViewport,
			EditorViewportClient->GetScene(),
			EditorViewportClient->EngineShowFlags
			)
			.SetRealtimeUpdate(EditorViewportClient->IsRealtime())
			);
	FSceneView* SceneView = EditorViewportClient->CalcSceneView(&ViewFamily);
	if (!SceneView){ return false;}
	const int32 MouseX = ActiveViewport->GetMouseX();
	const int32 MouseY = ActiveViewport->GetMouseY();

	FVector RayOrigin;
	FVector RayDirection;

	SceneView->DeprojectFVector2D(
		FVector2D(MouseX,MouseY),
		RayOrigin,
		RayDirection);
	if (RayDirection.IsNearlyZero()){return false;}

	RayDirection.Normalize();

	const FVector TraceStart = RayOrigin;
	const FVector TraceEnd = TraceStart + RayDirection * TraceDistance;

	FHitResult Hit;
	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(PCGCityEditorPlacementTrace), true);

	if (IsValid(ActorToIgnore))
	{
		QueryParams.AddIgnoredActor(ActorToIgnore);
	}

	const bool bBlockingHit = World->LineTraceSingleByChannel(
		Hit,
		TraceStart,
		TraceEnd,
		ECC_Visibility,
		QueryParams
		);

	if (bBlockingHit)
	{
		bHitSurface = true;
		OutNormal = Hit.ImpactNormal;
		OutLocation = Hit.Location + Hit.ImpactNormal * SurfaceOffset;
		return true;
	}

	//Fallback: mouse ray intersection with Z = 0 plane.
	if (FMath::IsNearlyZero(RayDirection.Z)) {return false;}

	const float T = -TraceStart.Z / RayDirection.Z;
	if (T < 0.0f){return false;}

	OutLocation = TraceStart + RayDirection * T;
	OutLocation.Z = 0.0f;
	OutNormal = FVector::UpVector;
	bHitSurface = false;
	return true;
	#else
	return false;
	#endif
}

bool UPCGCityEditorLibrary::IsEditorViewportLeftMouseDown()
{
#if WITH_EDITOR
	if (!GEditor){return false;}

	FViewport* ActiveViewport = GEditor->GetActiveViewport();
	if (!ActiveViewport){ return false;}

	return ActiveViewport->KeyState(EKeys::LeftMouseButton);
#else
	return deferred_false<>;
#endif
}