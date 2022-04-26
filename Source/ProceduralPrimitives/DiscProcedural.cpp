// Fill out your copyright notice in the Description page of Project Settings.


#include "DiscProcedural.h"
#include "DrawDebugHelpers.h"

// Sets default values
ADiscProcedural::ADiscProcedural()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	_proceduralMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("Procedural Mesh"));

	RootComponent = _proceduralMesh;
	//_proceduralMesh->SetupAttachment(RootComponent);

	_material = CreateDefaultSubobject<UMaterial>(TEXT("Material"));

}

// Called when the game starts or when spawned
void ADiscProcedural::BeginPlay()
{
	Super::BeginPlay();

}

#if WITH_EDITOR
void ADiscProcedural::PostEditChangeProperty(FPropertyChangedEvent& e)
{
	FName PropertyName = (e.Property != NULL) ? e.Property->GetFName() : NAME_None;
	bool radiusIsChanged = PropertyName == GET_MEMBER_NAME_CHECKED(ADiscProcedural, _radius);
	bool slicesIsChanged = PropertyName == GET_MEMBER_NAME_CHECKED(ADiscProcedural, _sides);
	if (radiusIsChanged || slicesIsChanged)
	{
		BuildMesh();
	}

	Super::PostEditChangeProperty(e);
}
#endif

void ADiscProcedural::BuildMesh()
{

	_vertices.Empty();
	_triangles.Empty();
	_normals.Empty();
	_uv.Empty();

	_vertices.Init(FVector(0.f, 0.f, 0.f), _sides + 1);
	_triangles.Init(0, _sides * 3);
	_normals.Init(FVector(0.0f, 0.0f, 1.0f), _sides + 1);
	_uv.Init(FVector2D(0.f, 0.f), _sides + 1);

	uint32 id = 0;
	float angleStep = 2 * PI / _sides;


	for (FVector& v : _vertices)
	{
		if (id == 0)
		{

			_uv[id] = FVector2D(0.5, 0.5);

			++id;
			continue;
		}

		float x = FMath::Cos(angleStep * id);
		float y = FMath::Sin(angleStep * id);

		v.X = x * _radius;
		v.Y = y * _radius;
		v.Z = 0.0f;

		_uv[id] = FVector2D((x + 1) / 2,
			(y + 1) / 2);

		_triangles.Add(0);
		_triangles.Add((id % _sides) + 1);
		_triangles.Add(id);

		//DrawDebugPoint(GetWorld(), v + GetActorLocation(), 10.0, FColor::Red, false, 300.0, 0.0);

		++id;
	}

	_proceduralMesh->CreateMeshSection(0, _vertices, _triangles, _normals, _uv, {}, {}, false);
}

void ADiscProcedural::PostLoad()
{
	Super::PostLoad();
	BuildMesh();
}

void ADiscProcedural::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	BuildMesh();
}

// Called every frame
void ADiscProcedural::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

