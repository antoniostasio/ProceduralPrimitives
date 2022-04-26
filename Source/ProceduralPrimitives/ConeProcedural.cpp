// Fill out your copyright notice in the Description page of Project Settings.


#include "ConeProcedural.h"
#include "DrawDebugHelpers.h"

// Sets default values
AConeProcedural::AConeProcedural()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	_proceduralMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("Procedural Mesh"));
	RootComponent = _proceduralMesh;

	_material = CreateDefaultSubobject<UMaterialInterface>(TEXT("Material"));
}

// Called when the game starts or when spawned
void AConeProcedural::BeginPlay()
{
	Super::BeginPlay();
}


void AConeProcedural::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	BuildMesh();
	_proceduralMesh->SetMaterial(0, _material);
}

#if WITH_EDITOR
void AConeProcedural::PostEditChangeProperty(FPropertyChangedEvent& e)
{
	FName PropertyName = (e.Property != NULL) ? e.Property->GetFName() : NAME_None;
	bool radiusHasChanged = PropertyName == GET_MEMBER_NAME_CHECKED(AConeProcedural, _radius);
	bool slicesHasChanged = PropertyName == GET_MEMBER_NAME_CHECKED(AConeProcedural, _sides);
	bool heigthHasChanged = PropertyName == GET_MEMBER_NAME_CHECKED(AConeProcedural, _height);

	if (radiusHasChanged || slicesHasChanged || heigthHasChanged)
	{
		BuildMesh();
	}

	bool materialHasChanged = PropertyName == GET_MEMBER_NAME_CHECKED(AConeProcedural, _material);
	if (materialHasChanged)
	{
		_proceduralMesh->SetMaterial(0, _material);
	}

	Super::PostEditChangeProperty(e);
}
#endif

void AConeProcedural::BuildMesh()
{

	_vertices.Empty();
	_triangles.Empty();
	_normals.Empty();
	_uv.Empty();

	// one copy of the top vertex for each side +
	// _sides times the number of segments +
	// one extra vertex in each segment for the slicing at the end of circle +
	// _sides vertices for cap + center of cap
	int32 vertexCount = (_hSegments + 2) * _sides + 1 + _hSegments+1;

	_vertices.Init(FVector(0.f, 0.f, 0.f), vertexCount);
	_normals.Init(FVector(0.0f, 0.0f, 1.0f), vertexCount);
	_uv.Init(FVector2D(0.f, 0.f), vertexCount);

	// _sides for cap + (hSegments-1)*_sides*2 + _sides
	_triangles.Reserve(3 * 2 * _sides * _hSegments);

	float angleStep = 2 * PI / _sides;
	float heightStep = _height / _hSegments;


	// Building cone base
	int32 id = 0;
	for (FVector& v : TArrayView<FVector>(_vertices).Slice(0, _sides + 1))
	{
		if (id == 0)
		{

			_uv[id] = FVector2D(0.25, 0.25);

			_normals[id] = FVector::DownVector;

			++id;
			continue;
		}

		float x = FMath::Cos(angleStep * id);
		float y = FMath::Sin(angleStep * id);

		v.X = x * _radius;
		v.Y = y * _radius;
		v.Z = 0.0f;

		uint32 id2 = (id % _sides) + 1;

		_triangles.Add(0);
		_triangles.Add(id);
		_triangles.Add(id2);

		_normals[id] = FVector::DownVector;

		_uv[id] = FVector2D((x + 1) / 4,
			(-y + 1) / 4);

		++id;
	}

	// Top surface mesh generation

	const float segmentDeltaH = _height / _hSegments;
	auto baseVertices = TArrayView<FVector>(_vertices).Slice(1, _sides);
	
	const int32 topSurfaceOffset = _sides + 1;
	const int32 segmentVertexCount = _sides + 1;

	id = 0;
	float theta = 0;

	// vertices and uv mapping
	for (uint32 segment = 0; segment <= _hSegments; ++segment)
	{
		const float currentSegmentHeight = segment * segmentDeltaH;
		const float scaleFactor = (_height - currentSegmentHeight) / _height;

		id = 0;
		int32 segmentOffset = topSurfaceOffset + (segmentVertexCount)*segment;
		auto segmentVertices = TArrayView<FVector>(_vertices).Slice(segmentOffset, segmentVertexCount);
		auto segmentUVs = TArrayView<FVector2D>(_uv).Slice(segmentOffset, segmentVertexCount);
		for (const auto& v : baseVertices)
		{


			segmentVertices[id].X = v.X * scaleFactor;
			segmentVertices[id].Y = v.Y * scaleFactor;
			segmentVertices[id].Z = currentSegmentHeight;

			segmentUVs[id] = FVector2D(1 - (1.f/_sides) * id,
									  (1 - currentSegmentHeight / _height));

			++id;
		}
		segmentVertices[id] = segmentVertices[0];
		segmentUVs[id] = FVector2D(1 - (1.f / _sides) * id,
								  (1 - currentSegmentHeight / _height));;
	}


	// triangles and normals
	for (uint32 segment = 0; segment < _hSegments; ++segment)
	{

		int32 bottomSegOffset = topSurfaceOffset + (segmentVertexCount)*segment;
		int32 topSegOffset = topSurfaceOffset + (segmentVertexCount)*(segment+1);

		auto bottomSegment = TArrayView<FVector>(_vertices).Slice(bottomSegOffset, segmentVertexCount - 1);
		auto topSegment = TArrayView<FVector>(_vertices).Slice(topSegOffset, segmentVertexCount - 1);

		id = 0;
		for (const auto& _: bottomSegment)
		{
			// subdivide square in two triangles
			int32 indexBottomRight = bottomSegOffset + id;
			int32 indexBottomLeft = indexBottomRight + 1;
			int32 indexTopRight = topSegOffset + id;
			int32 indexTopLeft = indexTopRight + 1;

			_triangles.Add(indexBottomLeft);
			_triangles.Add(indexBottomRight);
			_triangles.Add(indexTopRight);


			if (segment != _hSegments - 1)
			{
				_triangles.Add(indexBottomLeft);
				_triangles.Add(indexTopRight);
				_triangles.Add(indexTopLeft);
			}

			auto crossProduct = FVector::CrossProduct(_vertices[indexBottomLeft] - _vertices[indexBottomRight],
				_vertices[indexTopRight] - _vertices[indexBottomRight]).GetSafeNormal();

			_normals[indexBottomRight] = _vertices[indexBottomLeft];
			_normals[indexBottomRight].Z = crossProduct.Z;
			_normals[indexBottomRight].Normalize();
			_normals[indexBottomRight].X = _normals[indexBottomRight].X;
			_normals[indexBottomRight].Y = _normals[indexBottomRight].Y;
			_normals[indexBottomRight].Z = _normals[indexBottomRight].Z +0.5;

			_normals[indexTopRight] = _normals[indexBottomRight];

			++id;
		}

		int32 &firstIndex = bottomSegOffset;
		int32 lastIndex = bottomSegOffset + id;

		_normals[lastIndex] = _normals[firstIndex];
	}

	_proceduralMesh->CreateMeshSection(0, _vertices, _triangles, _normals, _uv, {}, {}, false);
}


void AConeProcedural::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
