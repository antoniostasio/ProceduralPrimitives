// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "CuboidProcedural.generated.h"

UCLASS()
class PROCEDURALPRIMITIVES_API ACuboidProcedural : public AActor
{
	GENERATED_BODY()

public:
	ACuboidProcedural();

public:
	UPROPERTY(EditAnywhere, Category = "Mesh")
		UProceduralMeshComponent* _proceduralMesh;
	UPROPERTY(EditAnywhere, Category = "Material")
		UMaterialInterface* _material;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "1", ClampMax = "500", UIMin = "1", UIMax = "500"))
		float _length = 30;
#if 0
	UPROPERTY(EditAnywhere, meta = (ClampMin = "1", ClampMax = "500", UIMin = "1", UIMax = "500"))
		float _height = 30;
	UPROPERTY(EditAnywhere, meta = (ClampMin = "1", ClampMax = "500", UIMin = "1", UIMax = "500"))
		float _width = 30;
#endif

protected:
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& e) override;
#endif

	void BuildMesh();

private:
	TArray<FVector> _vertices;
	TArray<int32> _triangles;
	TArray<FVector> _normals;
	TArray<FProcMeshTangent> _tangents;
	TArray<FVector2D> _uv;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
