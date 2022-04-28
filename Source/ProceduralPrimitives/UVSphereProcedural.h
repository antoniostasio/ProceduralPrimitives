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

    void _buildMesh();

    TArray<FVector> _makeSphereAxisPoints(float heightStep);
    void _allocateBuffers(int32 nVertices);
    void _makeSphereNormals();
    void _emptyBuffers();

public:	
	AUVSphereProcedural();
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category="Mesh")
		UProceduralMeshComponent* _proceduralMesh;
	UPROPERTY(EditAnywhere, Category="Material")
		UMaterialInterface* _material;
	UPROPERTY(EditAnywhere, meta = (ClampMin = "1", ClampMax = "1000", UIMin = "1", UIMax = "1000"))
		float _radius = 200;
	UPROPERTY(EditAnywhere, meta = (ClampMin = "3", ClampMax = "100", UIMin = "3", UIMax = "100"))
		int32 _meridians = 30;
	UPROPERTY(EditAnywhere, meta = (ClampMin = "3", ClampMax = "100", UIMin = "3", UIMax = "100"))
		int32 _parallels = 30;

protected:
	virtual void BeginPlay() override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& e) override;
#endif
};
