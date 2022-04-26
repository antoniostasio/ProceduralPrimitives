// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "DiscProcedural.generated.h"

UCLASS()
class PROCEDURALPRIMITIVES_API ADiscProcedural : public AActor
{
	GENERATED_BODY()


public:
	// Sets default values for this actor's properties
	ADiscProcedural();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& e) override;
#endif

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void PostLoad() override;

	void BuildMesh();

	UPROPERTY(EditAnywhere, Category = Procedural)
		UProceduralMeshComponent* _proceduralMesh;

	UPROPERTY(EditAnywhere, Category = Procedural)
		UMaterialInterface* _material;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.5", ClampMax = "500", UIMin = "0.5", UIMax = "100"))
		float _radius = 30;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "3", ClampMax = "100", UIMin = "3", UIMax = "100"))
		uint32 _sides = 32;

	TArray<FVector> _vertices;
	TArray<int32> _triangles;
	TArray<FVector> _normals;
	TArray<FVector2D> _uv;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};