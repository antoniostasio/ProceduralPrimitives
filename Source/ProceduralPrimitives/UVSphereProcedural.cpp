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

void AUVSphereProcedural::_makeSphereNormals()
{
    for (const auto& vertex : _vertices)
    {
        auto normal = vertex - FVector::ZeroVector;
        normal.Normalize();
        _normals.Add(normal);
    }
}

TArray<FVector> AUVSphereProcedural::_makeSphereAxisPoints(float angleStep)
{
    TArray<FVector> centers;
    centers.Reserve(_meridians);

    for (float currentAngle = -PI; currentAngle < PI; currentAngle += angleStep)
    {
        float unitHeight = FMath::Cos(currentAngle);
        centers.Add(FVector(0.0f, 0.0f, (unitHeight * _radius)));
    }

    centers.Add(FVector(0.0f, 0.0f, -_radius));

    return std::move(centers);
}

TArray<FVector2D> _makeSphereUVs()
{
    TArray<FVector2D> uvs;
    return std::move(uvs);
}

void AUVSphereProcedural::_buildMesh()
{
    int32 pointsPerCircle = _parallels;
    int32 nVertices = _meridians * pointsPerCircle;

    float meridianAngleStep = PI / _meridians;

    _emptyBuffers();
    _allocateBuffers(nVertices);

    TArray<int32> ids;
    ids.Reserve(nVertices);
    for (int32 id = 0; id < nVertices; id++) ids.Add(id);

    for (const auto& center : _makeSphereAxisPoints(meridianAngleStep))
    {
        float localRadius = FMath::Sqrt((_radius * _radius) - (FMath::Abs(center.Z) * FMath::Abs(center.Z)));
        UE_LOG(LogTemp, Warning, TEXT("radius: %f with center: %f"), localRadius, center.Z);
        _vertices.Append(CircularNGonPoints(pointsPerCircle, localRadius, center));
    }

    for (int32 circleStartingId = nVertices - pointsPerCircle;
            circleStartingId - pointsPerCircle >= 0;
            circleStartingId -= pointsPerCircle)
    {
        int32 bottomCircleStartingId = circleStartingId - pointsPerCircle;
        auto top = TArrayView<int32>(ids).Slice(circleStartingId, pointsPerCircle);
        auto bottom = TArrayView<int32>(ids).Slice(bottomCircleStartingId, pointsPerCircle);
        _triangles.Append(StripFromNGonCouple(top, bottom));
    }

    _makeSphereNormals();

    _proceduralMesh->CreateMeshSection(0, _vertices, _triangles, _normals, _uv, {}, {}, false);
}

