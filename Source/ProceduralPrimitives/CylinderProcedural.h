// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "CylinderProcedural.generated.h"

UCLASS()
class PROCEDURALPRIMITIVES_API ACylinderProcedural : public AActor
{
	GENERATED_BODY()

public:
	ACylinderProcedural();

protected:
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& e) override;
#endif

	void BuildMesh();

public:
	UPROPERTY(EditAnywhere, Category = "Mesh")
		UProceduralMeshComponent* _proceduralMesh;
	UPROPERTY(EditAnywhere, Category = "Material")
		UMaterialInterface* _material;
	UPROPERTY(EditAnywhere, meta = (ClampMin = "1", ClampMax = "1000", UIMin = "1", UIMax = "1000"))
		float _radius = 30;
	UPROPERTY(EditAnywhere, meta = (ClampMin = "1", ClampMax = "500", UIMin = "1", UIMax = "500"))
		float _height = 30;
	UPROPERTY(EditAnywhere, meta = (ClampMin = "3", ClampMax = "100", UIMin = "3", UIMax = "100"))
		uint32 _sides = 32;
	UPROPERTY(EditAnywhere, meta = (ClampMin = "1", ClampMax = "30", UIMin = "1", UIMax = "30"))
		uint32 _hSegments = 1;

private:
	TArray<FVector> _vertices;
	TArray<int32> _triangles;
	TArray<FVector> _normals;
	TArray<FVector2D> _uv;

public:
	virtual void Tick(float DeltaTime) override;
};