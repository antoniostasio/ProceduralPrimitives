#pragma once

#include "CoreMinimal.h"

TArray<FVector> CircularNGonPoints(int32 nPoints, float radius, const FVector& center, float theta = 0.0f);
TArray<int32> SquareTrianglesIds(int32 topLeft, int32 topRight, int32 bottomLeft, int32 bottomRight);
TArray<int32> StripFromNGonCouple(const TArrayView<int32>& nGon1Ids, const TArrayView<int32>& nGon2Ids);