#include "UVSphereProcedural.h"
#include "MeshGenerationMisc.h"

#include "DrawDebugHelpers.h"

#include "Util.h"

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
        _pointsPerCircle = _meridians + 1;
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

void AUVSphereProcedural::_allocateBuffers(int32 nVertices)
{
    _vertices.Reserve(nVertices);
    _triangles.Reserve(nVertices * 6);
    _normals.Reserve(nVertices);
    _uv.Reserve(nVertices);
}

void AUVSphereProcedural::_emptyBuffers()
{
    _uv.Empty();
    _normals.Empty();
    _vertices.Empty();
    _triangles.Empty();
}

TArray<FVector> AUVSphereProcedural::_makeSphereNormals()
{
    TArray<FVector> normals;

    for (const auto& vertex : _vertices)
    {
        auto normal = vertex - FVector::ZeroVector;
        normal.Normalize();
        normals.Add(normal);
    }

    return std::move(normals);
}

TArray<FVector> AUVSphereProcedural::_makeSphereAxisPoints(float angleStep)
{
    TArray<FVector> centers;
    centers.Reserve(_parallels);

    for (int32 i = 0; i < _parallels; ++i)
    {
        float currentAngle = i * angleStep;
        float unitHeight = FMath::Cos(currentAngle);
        centers.Add(FVector(0.0f, 0.0f, - (unitHeight * _radius)));
    }

    return centers;
}

TArray<FVector2D> AUVSphereProcedural::_makeSphereUVs()
{
    int32 size = _parallels * _pointsPerCircle;
    TArray<FVector2D> uvs;
    uvs.SetNum(size);

    float xStep = 1.0 / _meridians;
    float yStep = 1.0 / _parallels;
    for (int32 cy = 0; cy < _parallels; cy++)
    {
        for (int32 cx = 0; cx < _pointsPerCircle; cx++)
        {
            uvs[--size] = FVector2D(xStep * cx, yStep * cy);
        }
    }

    return uvs;
}

void AUVSphereProcedural::_buildMesh()
{
    int32 nVertices = _pointsPerCircle * _parallels;
    float parallelsAngleStep = PI / (_parallels - 1);

    _emptyBuffers();
    _allocateBuffers(nVertices);

    TArray<int32> ids;
    ids.Reserve(nVertices);

    for (const auto& center : _makeSphereAxisPoints(parallelsAngleStep))
    {
        float localRadius = FMath::Sqrt((_radius * _radius) - (FMath::Abs(center.Z) * FMath::Abs(center.Z)));
        _vertices.Append(CircularNGonPoints(_meridians, localRadius, center));
    }

    for (int32 id = 0; id < _vertices.Num(); ++id) ids.Add(id);

    for (int32 circleStartingId = _vertices.Num() - _pointsPerCircle;
            circleStartingId - _pointsPerCircle >= 0;
            circleStartingId -= _pointsPerCircle)
    {
        int32 bottomCircleStartingId = circleStartingId - _pointsPerCircle;
        auto top = TArrayView<int32>(ids).Slice(circleStartingId, _pointsPerCircle);
        auto bottom = TArrayView<int32>(ids).Slice(bottomCircleStartingId, _pointsPerCircle);

        _triangles.Append(StripFromNGonCouple(top, bottom));
    }

    _normals.Append(_makeSphereNormals());
    _uv.Append(_makeSphereUVs());

    _proceduralMesh->CreateMeshSection(0, _vertices, _triangles, _normals, _uv, {}, {}, false);
}

