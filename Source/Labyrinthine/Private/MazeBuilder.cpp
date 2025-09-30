#include "MazeBuilder.h"
#include "Misc/Paths.h"
#include "Misc/FileHelper.h"

AMazeBuilder::AMazeBuilder()
{
    PrimaryActorTick.bCanEverTick = false;

    USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    SetRootComponent(Root);

    Floors = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Floors"));
    Walls = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Walls"));
    Floors->SetupAttachment(RootComponent);
    Walls->SetupAttachment(RootComponent);
}

void AMazeBuilder::OnConstruction(const FTransform& Xform)
{
    Floors->ClearInstances();
    Walls->ClearInstances();

    TArray<TArray<int32>> Grid;
    if (!LoadCSV(Grid) || Grid.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("[MazeBuilder] Failed to load CSV: %s"),
            *CSVRelativePath);
        return;
    }

    if (FloorMesh) Floors->SetStaticMesh(FloorMesh);
    if (WallMesh)  Walls->SetStaticMesh(WallMesh);

    BuildFromGrid(Grid);
}

bool AMazeBuilder::LoadCSV(TArray<TArray<int32>>& OutGrid) const
{
    // Read from /Content/… (loose file on disk)
    const FString FullPath = FPaths::ProjectContentDir() / CSVRelativePath;
    FString FileData;
    if (!FFileHelper::LoadFileToString(FileData, *FullPath))
    {
        UE_LOG(LogTemp, Warning, TEXT("[MazeBuilder] Could not read file: %s"), *FullPath);
        return false;
    }

    OutGrid.Reset();
    TArray<FString> Lines;
    FileData.ParseIntoArrayLines(Lines);

    for (const FString& LineRaw : Lines)
    {
        const FString Line = LineRaw.TrimStartAndEnd();
        if (Line.IsEmpty()) continue;

        TArray<FString> Cells;
        Line.ParseIntoArray(Cells, TEXT(","), true);

        TArray<int32> Row;
        Row.Reserve(Cells.Num());
        for (FString C : Cells)
        {
            C.TrimStartAndEndInline();
            Row.Add(FCString::Atoi(*C));
        }
        OutGrid.Add(Row);
    }
    return true;
}

void AMazeBuilder::AddWallInstance(const FVector& Center, float YawDeg, float Length) const
{
    if (!WallMesh) return;
    // Assume the wall mesh is a 100x100x100 cube; scale to desired size
    const FVector Scale(Length / 100.f, WallThickness / 100.f, WallHeight / 100.f);
    const FTransform T(FRotator(0.f, YawDeg, 0.f), Center, Scale);
    Walls->AddInstance(T);
}

void AMazeBuilder::BuildFromGrid(const TArray<TArray<int32>>& Grid)
{
    const int32 H = Grid.Num();
    const int32 W = Grid[0].Num();

    const float Half = CellSize * 0.5f;
    const float Len = CellSize;
    const float Z = WallHeight * 0.5f;

    // Floors (one per cell), Z=0 plane
    if (FloorMesh)
    {
        for (int32 y = 0; y < H; ++y)
        {
            for (int32 x = 0; x < W; ++x)
            {
                const FVector Pos(x * CellSize + Half, y * CellSize + Half, 0.f);
                const FTransform T(FRotator::ZeroRotator, Pos, FVector(1.f));
                Floors->AddInstance(T);
            }
        }
    }

    // Walls: 1=N(+Y), 2=E(+X), 4=S(-Y), 8=W(-X)
    for (int32 y = 0; y < H; ++y)
    {
        for (int32 x = 0; x < W; ++x)
        {
            const int32 M = Grid[y][x];
            const float BaseX = x * CellSize;
            const float BaseY = y * CellSize;

            if (M & 1) // North edge
            {
                const FVector C(BaseX + Half, BaseY + CellSize, Z);
                AddWallInstance(C, 0.f, Len);
            }
            if (M & 2) // East edge
            {
                const FVector C(BaseX + CellSize, BaseY + Half, Z);
                AddWallInstance(C, 90.f, Len);
            }
            if (M & 4) // South edge
            {
                const FVector C(BaseX + Half, BaseY + 0.f, Z);
                AddWallInstance(C, 0.f, Len);
            }
            if (M & 8) // West edge
            {
                const FVector C(BaseX + 0.f, BaseY + Half, Z);
                AddWallInstance(C, 90.f, Len);
            }
        }
    }
}
