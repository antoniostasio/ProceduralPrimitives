#include "MeshGenerationMisc.h"

TArray<FVector> CircularNGonPoints(int32 nPoints, float radius, const FVector& center, float theta, bool includeCenter)
{
    TArray<FVector> points;
    points.AddUninitialized(nPoints);

    float dt = (PI * 2) / nPoints;

    for (auto& point : points)
    {
        point.X = center.X + radius * FMath::Cos(theta);
        point.Y = center.Y + radius * FMath::Sin(theta);
        point.Z = center.Z;

        theta += dt;
    }

    return points;
}

TArray<int32> StripFromNGonCouple(const TArrayView<int32>& topNGonIds, const TArrayView<int32>& bottomNGonIds)
{
    if (topNGonIds.Num() != bottomNGonIds.Num()) {
        return {};
    }

    TArray<int32> ids;
    // Each iteration apppends 6 ids;
    ids.Reserve(topNGonIds.Num() * 6); 

    auto it1 = topNGonIds.begin();
    auto it2 = bottomNGonIds.begin();

    for (; it1 != topNGonIds.end();) {
        int32 topLeft = *it1;
        int32 topRight = *( (it1 + 1) < topNGonIds.end() ? it1 + 1 : topNGonIds.begin() );
        int32 bottomLeft = *it2;
        int32 bottomRight = *( (it2 + 1) < bottomNGonIds.end() ? it2 + 1 : bottomNGonIds.begin() );

        ids.Append(SquareTrianglesIds(topLeft, topRight, bottomLeft, bottomRight));

        it1++; it2++;
    }

    return ids;
}

TArray<int32> SquareTrianglesIds(int32 topLeft, int32 topRight, int32 bottomLeft, int32 bottomRight)
{
    return {
        topLeft,
        topRight,
        bottomLeft,
        bottomLeft,
        topRight,
        bottomRight
    };
}
