#include "CGeneratedRoom.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"

ACGeneratedRoom::ACGeneratedRoom()
{
	PrimaryActorTick.bCanEverTick = false;

}

void ACGeneratedRoom::BeginPlay()
{
	Super::BeginPlay();
}

void ACGeneratedRoom::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACGeneratedRoom::Generate(int32 GridScale, float StageScale)
{
	for (int32 i = 0; i < GridScale; i++)
	{
		for (int32 j = 0; j < GridScale; j++)
		{
			HISM_Floor1->AddInstance(
				FTransform(
					FRotator(0.f, 0.f, 0.f),
					FVector(
						i * 100.f * StageScale,
						j * 100.f * StageScale,
						0.f
					)
				)
			);

			if (i == 0)
			{
				HISM_Wall->AddInstance(
					FTransform(
						FRotator(0.f, 90.f, 0.f),
						FVector(
							i * 100.f * StageScale - 100.f * StageScale / 2.f,
							j * 100.f * StageScale,
							0.f
						)
					)
				);
				HISM_Wall_Base->AddInstance(
					FTransform(
						FRotator(0.f, 90.f, 0.f),
						FVector(
							i * 100.f * StageScale - 100.f * StageScale / 2.f,
							j * 100.f * StageScale,
							0.f
						)
					)
				);
			}
			else if (i == GridScale - 1)
			{
				HISM_Wall->AddInstance(
					FTransform(
						FRotator(0.f, 90.f, 0.f),
						FVector(
							i * 100.f * StageScale + 100.f * StageScale / 2.f,
							j * 100.f * StageScale,
							0.f
						)
					)
				);
				HISM_Wall_Base->AddInstance(
					FTransform(
						FRotator(0.f, 90.f, 0.f),
						FVector(
							i * 100.f * StageScale + 100.f * StageScale / 2.f,
							j * 100.f * StageScale,
							0.f
						)
					)
				);
			}
			if (j == 0)
			{
				HISM_Wall->AddInstance(
					FTransform(
						FRotator(0.f, 0.f, 0.f),
						FVector(
							i * 100.f * StageScale,
							j * 100.f * StageScale - 100.f * StageScale / 2.f,
							0.f
						)
					)
				);
				HISM_Wall_Base->AddInstance(
					FTransform(
						FRotator(0.f, 0.f, 0.f),
						FVector(
							i * 100.f * StageScale,
							j * 100.f * StageScale - 100.f * StageScale / 2.f,
							0.f
						)
					)
				);
			}
			else if (j == GridScale - 1)
			{
				HISM_Wall->AddInstance(
					FTransform(
						FRotator(0.f, 0.f, 0.f),
						FVector(
							i * 100.f * StageScale,
							j * 100.f * StageScale + 100.f * StageScale / 2.f,
							0.f
						)
					)
				);
				HISM_Wall_Base->AddInstance(
					FTransform(
						FRotator(0.f, 0.f, 0.f),
						FVector(
							i * 100.f * StageScale,
							j * 100.f * StageScale + 100.f * StageScale / 2.f,
							0.f
						)
					)
				);
			}
		}
	}
}


