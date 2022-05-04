#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "ProceduralMeshComponent.h"

#include "UVSphereProcedural.generated.h"

UCLASS()
class PROCEDURALPRIMITIVES_API AUVSphereProcedural : public AActor
{
	GENERATED_BODY()

private:
	TArray<FVector> _vertices;
	TArray<int32> _triangles;
	TArray<FVector> _normals;
	TArray<FVector2D> _uv;

   int32 _pointsPerCircle = 0;

    void _buildMesh();

    TArray<FVector2D> _makeSphereUVs();
    TArray<FVector> _makeSphereNormals();
    TArray<FVector> _makeSphereAxisPoints(float heightStep);
    void _allocateBuffers(int32 nVertices);
    void _emptyBuffers();

public:	
	AUVSphereProcedural();
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category="Mesh")
	UProceduralMeshComponent* _proceduralMesh;

	UPROPERTY(EditAnywhere, Category="Material")
	UMaterialInterface* _material;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "1", ClampMax = "1000", UIMin = "1", UIMax = "1000"))
	float _radius = 90;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "3", ClampMax = "100", UIMin = "3", UIMax = "100"))
	int32 _meridians = 40;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "3", ClampMax = "100", UIMin = "3", UIMax = "100"))
	int32 _parallels = 40;

protected:
	virtual void BeginPlay() override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& e) override;
#endif
};
