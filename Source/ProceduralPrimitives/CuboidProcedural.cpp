// Fill out your copyright notice in the Description page of Project Settings.


#include "CuboidProcedural.h"

struct SurfaceMesh
{
	TArray<FVector> vertices;
	TArray<int32> triangles;
	TArray<FVector> normals;
	TArray<FProcMeshTangent> tangents;
	TArray<FVector2D> uv;
};

// Sets default values
ACuboidProcedural::ACuboidProcedural()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	_proceduralMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("Procedural Mesh"));
	RootComponent = _proceduralMesh;

	_material = CreateDefaultSubobject<UMaterialInterface>(TEXT("Material"));
}

// Called when the game starts or when spawned
void ACuboidProcedural::BeginPlay()
{
	Super::BeginPlay();

}

void ACuboidProcedural::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	BuildMesh();
	_proceduralMesh->SetMaterial(0, _material);
}

void ACuboidProcedural::PostEditChangeProperty(FPropertyChangedEvent& e)
{
	FName PropertyName = (e.Property != NULL) ? e.Property->GetFName() : NAME_None;
	bool lenghtHasChanged = PropertyName == GET_MEMBER_NAME_CHECKED(ACuboidProcedural, _length);


	if (lenghtHasChanged)
	{
		BuildMesh();
	}

	bool materialHasChanged = PropertyName == GET_MEMBER_NAME_CHECKED(ACuboidProcedural
		, _material);
	if (materialHasChanged)
	{
		_proceduralMesh->SetMaterial(0, _material);
	}

	Super::PostEditChangeProperty(e);
}

SurfaceMesh buildSquareSurface()
{
	SurfaceMesh square;
	square.vertices.Reserve(4);
	square.normals.Reserve(4);
	square.tangents.Reserve(4);
	square.uv.Reserve(4);
	square.triangles.Reserve(6);
	square.vertices.Add(FVector{ 1, 1, 1 });
	square.vertices.Add(FVector{ 1,-1, 1 });
	square.vertices.Add(FVector{-1,-1, 1 });
	square.vertices.Add(FVector{-1, 1, 1 });
	square.normals.Add(FVector::UpVector);
	square.normals.Add(FVector::UpVector);
	square.normals.Add(FVector::UpVector);
	square.normals.Add(FVector::UpVector);
	square.tangents.Add(FProcMeshTangent(FVector{ 1,0,0 }, false));
	square.tangents.Add(FProcMeshTangent(FVector{ 1,0,0 }, false));
	square.tangents.Add(FProcMeshTangent(FVector{ 1,0,0 }, false));
	square.tangents.Add(FProcMeshTangent(FVector{ 1,0,0 }, false));
	square.uv.Add(FVector2D{ 1,1 });
	square.uv.Add(FVector2D{ 1,0 });
	square.uv.Add(FVector2D{ 0,0 });
	square.uv.Add(FVector2D{ 0,1 });
	square.triangles.Add(0);
	square.triangles.Add(1);
	square.triangles.Add(2);
	square.triangles.Add(0);
	square.triangles.Add(2);
	square.triangles.Add(3);

	return square;
}

void ACuboidProcedural::BuildMesh()
{
	_vertices.Empty();
	_normals.Empty();
	_tangents.Empty();
	_uv.Empty();
	_triangles.Empty();

	const int32 verticesPerFace = 4;
	const int32 trianglesPerFace = 2;
	const int32 faceCount = 6;
	const int32 vertexCount = verticesPerFace * faceCount;
	const int32 triangleCount = trianglesPerFace * faceCount;
	_vertices.Reserve(vertexCount);
	_triangles.Reserve(triangleCount * 3);
	_normals.Reserve(vertexCount);
	_tangents.Reserve(vertexCount);
	_uv.Reserve(vertexCount);


	// building cube with a generatrix square
	auto squareSurface{ buildSquareSurface() };

	TArray<FTransform> faceTransforms;
	const FVector scale = FVector(_length/2, _length/2, _length/2);
	faceTransforms.Add(FTransform(FRotator::ZeroRotator, FVector::ZeroVector, scale));	// face oriented towards +z
	faceTransforms.Add(FTransform(FQuat(FVector(0,1,0),-PI),   FVector::ZeroVector, scale));	// -z
	faceTransforms.Add(FTransform(FQuat(FVector(1,0,0),-PI/2), FVector::ZeroVector, scale));	// +y
	faceTransforms.Add(FTransform(FQuat(FVector(0,0,1),    PI) * FQuat(FVector(1,0,0),-PI/2), FVector::ZeroVector, scale));	// -y
	faceTransforms.Add(FTransform(FQuat(FVector(0,0,1),0.5*PI) * FQuat(FVector(1,0,0),-PI/2), FVector::ZeroVector, scale));	// -x
	faceTransforms.Add(FTransform(FQuat(FVector(0,0,1),1.5*PI) * FQuat(FVector(1,0,0),-PI/2), FVector::ZeroVector, scale));	// +x

	TArray<FVector> faceNormals{
		FVector::UpVector,
		FVector::DownVector,
		FVector::RightVector,
		FVector::LeftVector,
		FVector::BackwardVector,
		FVector::ForwardVector
	};

	int32 currentFace = 0;
	for (const auto& transform : faceTransforms)
	{
		const int32 vertexStartIndex = _vertices.Num();

		int32 generatrixCorner = 0;
		for (const auto& corner : squareSurface.vertices)
		{
			_vertices.Add(transform.TransformVector(corner));

#if 1
			FVector vNormal = squareSurface.normals[generatrixCorner];
			vNormal = transform.TransformVectorNoScale(vNormal);
			FVector tangentVector = squareSurface.tangents[generatrixCorner].TangentX;
			tangentVector = transform.TransformVectorNoScale(tangentVector);
			auto vTangent = FProcMeshTangent(tangentVector, false);
#else
			FVector vNormal = faceNormals[currentFace];
#endif
			//vNormal.Z = vNormal.Z + 0.5;
			_normals.Add(vNormal);
			_tangents.Add(vTangent);
			_uv.Add(squareSurface.uv[generatrixCorner]);

			++generatrixCorner;
		}

		for (const auto& index : squareSurface.triangles)
		{
			_triangles.Add(index + vertexStartIndex);
		}

		++currentFace;
	}

	_proceduralMesh->CreateMeshSection(0, _vertices, _triangles, _normals, _uv, {}, _tangents, false);

}



// Called every frame
void ACuboidProcedural::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

