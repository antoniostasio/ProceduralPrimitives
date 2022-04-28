#include "UVSphereProcedural.h"
#include "MeshGenerationMisc.h"

#include "DrawDebugHelpers.h"

AUVSphereProcedural::AUVSphereProcedural()
{
	PrimaryActorTick.bCanEverTick = true;
    _proceduralMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("Procedural Mesh"));
    RootComponent = _proceduralMesh;
	_material = CreateDefaultSubobject<UMaterialInterface>(TEXT("Material"));
}

#if WITH_EDITOR
void AUVSphereProcedural::PostEditChangeProperty(FPropertyChangedEvent& e)
{
	FName PropertyName = (e.Property != NULL) ? e.Property->GetFName() : NAME_None;
	bool anyPropChanged = PropertyName == GET_MEMBER_NAME_CHECKED(AUVSphereProcedural, _radius)
	    || PropertyName == GET_MEMBER_NAME_CHECKED(AUVSphereProcedural, _parallels)
	    || PropertyName == GET_MEMBER_NAME_CHECKED(AUVSphereProcedural, _meridians);

	if (anyPropChanged)
	{
		_buildMesh();
	}

	bool materialHasChanged = PropertyName == GET_MEMBER_NAME_CHECKED(AUVSphereProcedural, _material);
	if (materialHasChanged)
	{
		_proceduralMesh->SetMaterial(0, _material);
	}

	Super::PostEditChangeProperty(e);
}
#endif

void AUVSphereProcedural::BeginPlay()
{
	Super::BeginPlay();
}

void AUVSphereProcedural::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AUVSphereProcedural::_buildMesh()
{
    _uv.Empty();
    _normals.Empty();
    _vertices.Empty();
    _triangles.Empty();

    TArray<FVector> centers;
    TArray<int32> ids;

    int32 pointsPerCircle = _parallels;
    float stripHeight = (_radius * 2) / _meridians;
    centers.Reserve(_meridians);

    int32 nVertices = _meridians * pointsPerCircle;

    ids.Reserve(nVertices);
    _vertices.Reserve(nVertices);
    _triangles.Reserve(nVertices * 6);

    for (int32 id = 0; id < nVertices; id++)
        ids.Add(id);

    _vertices.Reserve(nVertices);

    for (float currentHeight = 0.0f; currentHeight < _radius * 2; currentHeight += stripHeight)
    {
        centers.Add(FVector(0.0f, 0.0f, currentHeight - _radius));
    }

    for (const auto& center : centers)
    {
        float localRadius = FMath::Sqrt((_radius * _radius) - (FMath::Abs(center.Z) * FMath::Abs(center.Z)));
        _vertices.Append(CircularNGonPoints(pointsPerCircle, localRadius, center));
    }

    for (int32 circleStartingId = nVertices - pointsPerCircle; circleStartingId > pointsPerCircle; circleStartingId -= pointsPerCircle)
    {
        int32 bottomCircleStartingId = circleStartingId - pointsPerCircle;
        auto top = TArrayView<int32>(ids).Slice(circleStartingId, pointsPerCircle);
        auto bottom = TArrayView<int32>(ids).Slice(bottomCircleStartingId, pointsPerCircle);
        _triangles.Append(StripFromNGonCouple(top, bottom));
    }

	_proceduralMesh->CreateMeshSection(0, _vertices, _triangles, _normals, _uv, {}, {}, false);
}
