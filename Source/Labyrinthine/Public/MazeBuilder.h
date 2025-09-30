#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "MazeBuilder.generated.h"

// Bitmask per cell: 1=N, 2=E, 4=S, 8=W
UCLASS()
class AMazeBuilder : public AActor
{
    GENERATED_BODY()

public:
    AMazeBuilder();

    // Path relative to /Content (do NOT import as asset)
    UPROPERTY(EditAnywhere, Category = "Maze")
    FString CSVRelativePath = TEXT("Mazes/maze_20x20.csv");

    // Size of one cell in centimeters (e.g., 400 = 4m)
    UPROPERTY(EditAnywhere, Category = "Maze|Scale")
    float CellSize = 400.f;

    // Wall dimensions (in cm)
    UPROPERTY(EditAnywhere, Category = "Maze|Scale")
    float WallThickness = 10.f;

    UPROPERTY(EditAnywhere, Category = "Maze|Scale")
    float WallHeight = 300.f;

    // Meshes
    UPROPERTY(EditAnywhere, Category = "Maze|Meshes")
    UStaticMesh* FloorMesh = nullptr;

    UPROPERTY(EditAnywhere, Category = "Maze|Meshes")
    UStaticMesh* WallMesh = nullptr;

protected:
    virtual void OnConstruction(const FTransform& Transform) override;

private:
    UPROPERTY()
    UInstancedStaticMeshComponent* Floors;

    UPROPERTY()
    UInstancedStaticMeshComponent* Walls;

    bool LoadCSV(TArray<TArray<int32>>& OutGrid) const;
    void BuildFromGrid(const TArray<TArray<int32>>& Grid);
    void AddWallInstance(const FVector& Center, float YawDeg, float Length) const;
};
