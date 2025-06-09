#include "CGeneratedStair.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "IGameStateLightManager.h"
#include "Components/PointLightComponent.h"
#include "Components/SphereComponent.h"
#include "IPlayerOnStage.h"
#include "IPlayerControllerStage.h"
#include "IObjectPoolManager.h"
#include "Debug/DebugDrawService.h"

ACGeneratedStair::ACGeneratedStair() : Super()
{
	EntranceCollider = CreateDefaultSubobject<USphereComponent>(TEXT("Stair_EntranceCollider"));
	ExitCollider = CreateDefaultSubobject<USphereComponent>(TEXT("Stair_ExitCollider"));
	if (EntranceCollider != nullptr)
	{
		EntranceCollider->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		EntranceCollider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
	if (ExitCollider != nullptr)
	{
		ExitCollider->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		ExitCollider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
	EntranceCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);
	ExitCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);
}

void ACGeneratedStair::OnEntranceBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Log, TEXT("ACGeneratedStair : OnEntranceBeginOverlap : Overlap to Actor Named : %s"), *OtherActor->GetName());
	IIPlayerOnStage* IPOS = Cast<IIPlayerOnStage>(OtherActor);
	if (IPOS != nullptr)
	{
		if (Delegate_LightOn != nullptr) Delegate_LightOn->ExecuteIfBound();
		if (Delegate_Entered_Stage != nullptr) Delegate_Entered_Stage->ExecuteIfBound(OtherActor, IPOS->GetControllerAsObject());
	}
}

void ACGeneratedStair::OnExitBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Log, TEXT("ACGeneratedStair : OnExitBeginOverlap : Overlap to Actor Named : %s"), *OtherActor->GetName());
	IIPlayerOnStage* IPOS = Cast<IIPlayerOnStage>(OtherActor);
	if (IPOS != nullptr)
	{
		IIPlayerControllerStage* IPCS = Cast<IIPlayerControllerStage>(IPOS->GetControllerAsObject());
		if (IPCS != nullptr)
		{
			IPCS->LightsOutPrevStage();
		}
		if (ObjectPoolManager != nullptr) ObjectPoolManager->GenerateNextStage();
	}
	//if (Delegate_Entered_Stage != nullptr) Delegate_Entered_Stage->Unbind();
	//if (Delegate_LightOn != nullptr) Delegate_LightOn->Unbind();
}

void ACGeneratedStair::GenerateStair(bool bIsEntrance, FLIGHTS_ON* DeleLightOn, FENTERED_STAGE* DeleEnteredStage)
{
	if (DeleLightOn != nullptr) Delegate_LightOn = DeleLightOn;
	if (DeleEnteredStage != nullptr) Delegate_Entered_Stage = DeleEnteredStage;

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			bool im = i == 0 ? true : false;
			bool iM = i == 3 ? true : false;
			bool jm = j == 0 ? true : false;
			bool jM = j == 3 ? true : false;

			AddGrid(
				FTransform(
					FRotator(0.f, 90.f, 0.f),
					FVector(i * 400.f, j * 400.f, 0.f)
				),
				STAGE_GRID_CORRIDOR_FLOOR
			);
			if (im)
			{
				if (jm)
				{
					AddGrid(
						FTransform(
							FRotator(0.f, 90.f, 180.f),
							FVector(i * 400.f, j * 400.f, 500.f)
						),
						STAGE_GRID_CORRIDOR_FLOOR
					);

					ExitCollider->SetRelativeTransform(
						FTransform(
							FRotator(0.f, 0.f, 0.f),
							FVector(i * 400.f, j * 400.f, 100.f)
						)
					);
					ExitCollider->SetSphereRadius(200.f);

					for (int d = 0; d < 4; d++)
					{
						AddGrid(
							FTransform(
								FRotator(0.f, 90.f * d, 0.f),
								FVector(i * 400.f, j * 400.f, 0.f)
							),
							STAGE_GRID_CORRIDOR_CEILING1
						);
					}
					// Spawn Candlelabra
					AddGrid(
						FTransform(
							FRotator(0.f, 270.f, 0.f),
							FVector(
								i * 400.f - 200.f,
								j * 400.f,
								180.f)
						),
						STAGE_GRID_CORRIDOR_CANDLELABRA
					);
					CandlePosArr.Add(FVector(
						i * 400.f - 200.f + 72.f,
						j * 400.f,
						180.f + 66.f));
					CandlePosArr.Add(FVector(
						i * 400.f - 200.f + 54.f,
						j * 400.f,
						180.f + 127.f));
					CandleArr.Add(nullptr);
					CandleArr.Add(nullptr);
					//DrawDebugSphere(GetWorld(), GetActorLocation() + FVector(
					//	i * 400.f - 200.f,
					//	j * 400.f + 72.f,
					//	180.f + 66.f), 20.f, 32, FColor::Red, true);
					//DrawDebugSphere(GetWorld(), GetActorLocation() + FVector(
					//	i * 400.f - 200.f,
					//	j * 400.f + 54.f,
					//	180.f + 127.f), 20.f, 32, FColor::Red, true);
					//DrawDebugSphere(GetWorld(), GetActorLocation() + FVector(
					//		i * 400.f - 200.f,
					//		j * 400.f,
					//		180.f), 20.f, 32, FColor::Blue, true);

					AddGrid(
						FTransform(
							FRotator(0.f, FMath::FRandRange(0.f, 360.f), 0.f),
							FVector(
								i * 400.f + 200.f,
								j * 400.f + 200.f,
								0.f)
						),
						STAGE_GRID_CORRIDOR_PHILAR
					);

					AddGrid(
						FTransform(
							FRotator(0.f, FMath::FRandRange(0.f, 360.f), 0.f),
							FVector(
								i * 400.f + 200.f,
								j * 400.f - 200.f,
								0.f)
						),
						STAGE_GRID_CORRIDOR_PHILAR
					);

					AddGrid(
						FTransform(
							FRotator(0.f, FMath::FRandRange(0.f, 360.f), 0.f),
							FVector(
								i * 400.f - 200.f,
								j * 400.f + 200.f,
								0.f)
						),
						STAGE_GRID_CORRIDOR_PHILAR
					);

					AddGrid(
						FTransform(
							FRotator(0.f, FMath::FRandRange(0.f, 360.f), 0.f),
							FVector(
								i * 400.f - 200.f,
								j * 400.f - 200.f,
								0.f)
						),
						STAGE_GRID_CORRIDOR_PHILAR
					);

					AddGrid(
						FTransform(
							FRotator(0.f, 90.f, 0.f),
							FVector(
								i * 400.f - 200.f,
								j * 400.f,
								0.f)
						),
						STAGE_GRID_CORRIDOR_WALL_BASE
					);
					AddGrid(
						FTransform(
							FRotator(0.f, 90.f, 0.f),
							FVector(
								i * 400.f - 200.f,
								j * 400.f,
								0.f)
						),
						STAGE_GRID_CORRIDOR_WALL
					);
					AddGrid(
						FTransform(
							FRotator(0.f, 90.f, 0.f),
							FVector(
								i * 400.f - 200.f,
								j * 400.f,
								240.f)
						),
						STAGE_GRID_CORRIDOR_WALL_BASE
					);
					AddGrid(
						FTransform(
							FRotator(0.f, 0.f, 0.f),
							FVector(
								i * 400.f - 200.f,
								j * 400.f,
								300.f)
						),
						STAGE_GRID_CORRIDOR_CEILING_ARCH_WALL
					);
					AddGrid(
						FTransform(
							FRotator(0.f, 90.f, 0.f),
							FVector(
								i * 400.f + 200.f,
								j * 400.f,
								0.f)
						),
						STAGE_GRID_CORRIDOR_WALL_BASE
					);
					AddGrid(
						FTransform(
							FRotator(0.f, 90.f, 0.f),
							FVector(
								i * 400.f + 200.f,
								j * 400.f,
								0.f)
						),
						STAGE_GRID_CORRIDOR_WALL
					);
					AddGrid(
						FTransform(
							FRotator(0.f, 90.f, 0.f),
							FVector(
								i * 400.f + 200.f,
								j * 400.f,
								240.f)
						),
						STAGE_GRID_CORRIDOR_WALL_BASE
					);
					AddGrid(
						FTransform(
							FRotator(0.f, 0.f, 0.f),
							FVector(
								i * 400.f + 200.f,
								j * 400.f,
								300.f)
						),
						STAGE_GRID_CORRIDOR_CEILING_ARCH_WALL
					);
				}
				else if (jM)
				{
					AddGrid(
						FTransform(
							FRotator(0.f, 90.f, 180.f),
							FVector(i * 400.f, j * 400.f, 700.f)
						),
						STAGE_GRID_CORRIDOR_FLOOR
					);
					for (int d = 0; d < 4; d++)
					{
						AddGrid(
							FTransform(
								FRotator(0.f, 90.f * d, 0.f),
								FVector(i * 400.f, j * 400.f, 200.f)
							),
							STAGE_GRID_CORRIDOR_CEILING1
						);
					}
					AddGrid(
						FTransform(
							FRotator(0.f, 180.f, 0.f),
							FVector(
								i * 400.f,
								j * 400.f + 200.f,
								200.f + 180.f)
						),
						STAGE_GRID_CORRIDOR_CANDLELABRA
					);
					CandlePosArr.Add(FVector(
						i * 400.f,
						j * 400.f + 200.f - 72.f,
						200.f + 180.f + 66.f));
					CandlePosArr.Add(FVector(
						i * 400.f,
						j * 400.f + 200.f - 54.f,
						200.f + 180.f + 127.f));
					CandleArr.Add(nullptr);
					CandleArr.Add(nullptr);

					AddGrid(
						FTransform(
							FRotator(0.f, 0.f, 0.f),
							FVector(i * 400.f, j * 400.f, 200.f)
						),
						STAGE_GRID_CORRIDOR_FLOOR
					);
					AddGrid(
						FTransform(
							FRotator(0.f, 90.f, 0.f),
							FVector(
								i * 400.f - 200.f,
								j * 400.f,
								200.f)
						),
						STAGE_GRID_CORRIDOR_WALL_BASE
					);
					AddGrid(
						FTransform(
							FRotator(0.f, 90.f, 0.f),
							FVector(
								i * 400.f - 200.f,
								j * 400.f,
								200.f)
						),
						STAGE_GRID_CORRIDOR_WALL
					);
					AddGrid(
						FTransform(
							FRotator(0.f, 90.f, 0.f),
							FVector(
								i * 400.f - 200.f,
								j * 400.f,
								440.f)
						),
						STAGE_GRID_CORRIDOR_WALL_BASE
					);

					AddGrid(
						FTransform(
							FRotator(0.f, 0.f, 0.f),
							FVector(
								i * 400.f - 200.f,
								j * 400.f,
								500.f)
						),
						STAGE_GRID_CORRIDOR_CEILING_ARCH_WALL
					);

					AddGrid(
						FTransform(
							FRotator(0.f, 180.f, 0.f),
							FVector(
								i * 400.f,
								j * 400.f + 200.f,
								200.f)
						),
						STAGE_GRID_CORRIDOR_WALL_BASE
					);
					AddGrid(
						FTransform(
							FRotator(0.f, 180.f, 0.f),
							FVector(
								i * 400.f,
								j * 400.f + 200.f,
								200.f)
						),
						STAGE_GRID_CORRIDOR_WALL
					);
					AddGrid(
						FTransform(
							FRotator(0.f, 180.f, 0.f),
							FVector(
								i * 400.f,
								j * 400.f + 200.f,
								440.f)
						),
						STAGE_GRID_CORRIDOR_WALL_BASE
					);
					AddGrid(
						FTransform(
							FRotator(0.f, 90.f, 0.f),
							FVector(
								i * 400.f,
								j * 400.f + 200.f,
								500.f)
						),
						STAGE_GRID_CORRIDOR_CEILING_ARCH_WALL
					);
					AddGrid(
						FTransform(
							FRotator(0.f, FMath::FRandRange(0.f, 360.f), 0.f),
							FVector(
								i * 400.f - 200.f,
								j * 400.f + 200.f,
								200.f)
						),
						STAGE_GRID_CORRIDOR_PHILAR
					);
					AddGrid(
						FTransform(
							FRotator(0.f, FMath::FRandRange(0.f, 360.f), 0.f),
							FVector(
								i * 400.f + 200.f,
								j * 400.f + 200.f,
								200.f)
						),
						STAGE_GRID_CORRIDOR_PHILAR
					);
				}
				else
				{
					AddGrid(
						FTransform(
							FRotator(0.f, 270.f, 0.f),
							FVector(i * 400.f, j * 400.f, 100.f * j)
						),
						STAGE_GRID_CORRIDOR_FLOOR_CLAMP
					);
					AddGrid(
						FTransform(
							FRotator(0.f, 270.f, 0.f),
							FVector(
								i * 400.f - 200.f,
								j * 400.f,
								100.f * j)
						),
						STAGE_GRID_CORRIDOR_WALL_CLAMP1_BASE
					);
					AddGrid(
						FTransform(
							FRotator(0.f, 270.f, 0.f),
							FVector(
								i * 400.f - 200.f,
								j * 400.f,
								100.f * j)
						),
						STAGE_GRID_CORRIDOR_WALL_CLAMP1
					);
					AddGrid(
						FTransform(
							FRotator(0.f, 270.f, 0.f),
							FVector(
								i * 400.f - 200.f,
								j * 400.f,
								100.f * j + 240.f)
						),
						STAGE_GRID_CORRIDOR_WALL_CLAMP1_BASE
					);

					AddGrid(
						FTransform(
							FRotator(0.f, 270.f, 0.f),
							FVector(
								i * 400.f + 200.f,
								j * 400.f,
								100.f * j)
						),
						STAGE_GRID_CORRIDOR_WALL_CLAMP1_BASE
					);
					AddGrid(
						FTransform(
							FRotator(0.f, 270.f, 0.f),
							FVector(
								i * 400.f + 200.f,
								j * 400.f,
								100.f * j)
						),
						STAGE_GRID_CORRIDOR_WALL_CLAMP1
					);
					AddGrid(
						FTransform(
							FRotator(0.f, 270.f, 0.f),
							FVector(
								i * 400.f + 200.f,
								j * 400.f,
								100.f * j + 240.f)
						),
						STAGE_GRID_CORRIDOR_WALL_CLAMP1_BASE
					);


					AddGrid(
						FTransform(
							FRotator(0.f, -270.f, 0.f),
							FVector(
								i * 400.f,
								j * 400.f,
								100.f * j)
						),
						STAGE_GRID_CORRIDOR_CEILING3
					);

					AddGrid(
						FTransform(
							FRotator(0.f, FMath::FRandRange(0.f, 360.f), 0.f),
							FVector(
								i * 400.f - 200.f,
								j * 400.f + 200.f,
								100.f * j)
						),
						STAGE_GRID_CORRIDOR_PHILAR
					);
					AddGrid(
						FTransform(
							FRotator(0.f, FMath::FRandRange(0.f, 360.f), 0.f),
							FVector(
								i * 400.f + 200.f,
								j * 400.f + 200.f,
								100.f * j)
						),
						STAGE_GRID_CORRIDOR_PHILAR
					);
				}
			}
			else if (iM)
			{
				if (jM)
				{
					AddGrid(
						FTransform(
							FRotator(0.f, 90.f, 180.f),
							FVector(i * 400.f, j * 400.f, 900.f)
						),
						STAGE_GRID_CORRIDOR_FLOOR
					);
					for (int d = 0; d < 4; d++)
					{
						AddGrid(
							FTransform(
								FRotator(0.f, 90.f * d, 0.f),
								FVector(i * 400.f, j * 400.f, 400.f)
							),
							STAGE_GRID_CORRIDOR_CEILING1
						);
					}

					AddGrid(
						FTransform(
							FRotator(0.f, 90.f, 0.f),
							FVector(
								i * 400.f + 200.f,
								j * 400.f,
								400.f + 180.f)
						),
						STAGE_GRID_CORRIDOR_CANDLELABRA
					);
					CandlePosArr.Add(FVector(
						i * 400.f + 200.f - 72.f,
						j * 400.f,
						400.f + 180.f + 66.f));
					CandlePosArr.Add(FVector(
						i * 400.f + 200.f - 54.f,
						j * 400.f,
						400.f + 180.f + 127.f));
					CandleArr.Add(nullptr);
					CandleArr.Add(nullptr);

					AddGrid(
						FTransform(
							FRotator(0.f, 270.f, 0.f),
							FVector(i * 400.f, j * 400.f, 400.f)
						),
						STAGE_GRID_CORRIDOR_FLOOR
					);
					AddGrid(
						FTransform(
							FRotator(0.f, 180.f, 0.f),
							FVector(
								i * 400.f,
								j * 400.f + 200.f,
								400.f)
						),
						STAGE_GRID_CORRIDOR_WALL_BASE
					);
					AddGrid(
						FTransform(
							FRotator(0.f, 180.f, 0.f),
							FVector(
								i * 400.f,
								j * 400.f + 200.f,
								400.f)
						),
						STAGE_GRID_CORRIDOR_WALL
					);
					AddGrid(
						FTransform(
							FRotator(0.f, 90.f, 0.f),
							FVector(
								i * 400.f,
								j * 400.f + 200.f,
								700.f)
						),
						STAGE_GRID_CORRIDOR_CEILING_ARCH_WALL
					);

					AddGrid(
						FTransform(
							FRotator(0.f, 180.f, 0.f),
							FVector(
								i * 400.f,
								j * 400.f + 200.f,
								640.f)
						),
						STAGE_GRID_CORRIDOR_WALL_BASE
					);
					AddGrid(
						FTransform(
							FRotator(0.f, 270.f, 0.f),
							FVector(
								i * 400.f + 200.f,
								j * 400.f,
								400.f)
						),
						STAGE_GRID_CORRIDOR_WALL_BASE
					);
					AddGrid(
						FTransform(
							FRotator(0.f, 270.f, 0.f),
							FVector(
								i * 400.f + 200.f,
								j * 400.f,
								400.f)
						),
						STAGE_GRID_CORRIDOR_WALL
					);
					AddGrid(
						FTransform(
							FRotator(0.f, 270.f, 0.f),
							FVector(
								i * 400.f + 200.f,
								j * 400.f,
								640.f)
						),
						STAGE_GRID_CORRIDOR_WALL_BASE
					);
					AddGrid(
						FTransform(
							FRotator(0.f, 0.f, 0.f),
							FVector(
								i * 400.f + 200.f,
								j * 400.f,
								700.f)
						),
						STAGE_GRID_CORRIDOR_CEILING_ARCH_WALL
					);

					AddGrid(
						FTransform(
							FRotator(0.f, FMath::FRandRange(0.f, 360.f), 0.f),
							FVector(
								i * 400.f + 200.f,
								j * 400.f - 200.f,
								400.f)
						),
						STAGE_GRID_CORRIDOR_PHILAR
					);
					AddGrid(
						FTransform(
							FRotator(0.f, FMath::FRandRange(0.f, 360.f), 0.f),
							FVector(
								i * 400.f + 200.f,
								j * 400.f + 200.f,
								400.f)
						),
						STAGE_GRID_CORRIDOR_PHILAR
					);
				}
				else if (jm)
				{
					EntranceCollider->SetRelativeTransform(
						FTransform(
							FRotator(0.f, 0.f, 0.f),
							FVector(i * 400.f, j * 400.f, 700.f)
						)
					);
					EntranceCollider->SetSphereRadius(200.f);
					AddGrid(
						FTransform(
							FRotator(0.f, 90.f, 180.f),
							FVector(i * 400.f, j * 400.f, 1100.f)
						),
						STAGE_GRID_CORRIDOR_FLOOR
					);
					for (int d = 0; d < 4; d++)
					{
						AddGrid(
							FTransform(
								FRotator(0.f, 90.f * d, 0.f),
								FVector(i * 400.f, j * 400.f, 600.f)
							),
							STAGE_GRID_CORRIDOR_CEILING1
						);
					}
					AddGrid(
						FTransform(
							FRotator(0.f, 270.f, 0.f),
							FVector(
								i * 400.f - 200.f,
								j * 400.f,
								600.f + 180.f)
						),
						STAGE_GRID_CORRIDOR_CANDLELABRA
					);
					CandlePosArr.Add(FVector(
						i * 400.f - 200.f + 72.f,
						j * 400.f,
						600.f + 180.f + 66.f));
					CandlePosArr.Add(FVector(
						i * 400.f - 200.f + 72.f,
						j * 400.f,
						600.f + 180.f + 127.f));
					CandleArr.Add(nullptr);
					CandleArr.Add(nullptr);
					AddGrid(
						FTransform(
							FRotator(0.f, 90.f, 0.f),
							FVector(i * 400.f, j * 400.f, 600.f)
						),
						STAGE_GRID_CORRIDOR_FLOOR
					);
					AddGrid(
						FTransform(
							FRotator(0.f, 90.f, 0.f),
							FVector(
								i * 400.f + 200.f,
								j * 400.f,
								600.f)
						),
						STAGE_GRID_CORRIDOR_WALL_BASE
					);
					AddGrid(
						FTransform(
							FRotator(0.f, 90.f, 0.f),
							FVector(
								i * 400.f + 200.f,
								j * 400.f,
								600.f)
						),
						STAGE_GRID_CORRIDOR_WALL
					);
					AddGrid(
						FTransform(
							FRotator(0.f, 90.f, 0.f),
							FVector(
								i * 400.f + 200.f,
								j * 400.f,
								840.f)
						),
						STAGE_GRID_CORRIDOR_WALL_BASE
					);
					AddGrid(
						FTransform(
							FRotator(0.f, 0.f, 0.f),
							FVector(
								i * 400.f + 200.f,
								j * 400.f,
								900.f)
						),
						STAGE_GRID_CORRIDOR_CEILING_ARCH_WALL
					);

					AddGrid(
						FTransform(
							FRotator(0.f, 90.f, 0.f),
							FVector(
								i * 400.f - 200.f,
								j * 400.f,
								600.f)
						),
						STAGE_GRID_CORRIDOR_WALL_BASE
					);
					AddGrid(
						FTransform(
							FRotator(0.f, 90.f, 0.f),
							FVector(
								i * 400.f - 200.f,
								j * 400.f,
								600.f)
						),
						STAGE_GRID_CORRIDOR_WALL
					);
					AddGrid(
						FTransform(
							FRotator(0.f, 90.f, 0.f),
							FVector(
								i * 400.f - 200.f,
								j * 400.f,
								840.f)
						),
						STAGE_GRID_CORRIDOR_WALL_BASE
					);
					AddGrid(
						FTransform(
							FRotator(0.f, 0.f, 0.f),
							FVector(
								i * 400.f - 200.f,
								j * 400.f,
								900.f)
						),
						STAGE_GRID_CORRIDOR_CEILING_ARCH_WALL
					);

					AddGrid(
						FTransform(
							FRotator(0.f, FMath::FRandRange(0.f, 360.f), 0.f),
							FVector(
								i * 400.f + 200.f,
								j * 400.f - 200.f,
								600.f)
						),
						STAGE_GRID_CORRIDOR_PHILAR
					);

					AddGrid(
						FTransform(
							FRotator(0.f, FMath::FRandRange(0.f, 360.f), 0.f),
							FVector(
								i * 400.f - 200.f,
								j * 400.f - 200.f,
								600.f)
						),
						STAGE_GRID_CORRIDOR_PHILAR
					);
					if (bIsEntrance)
					{
						if (ObjectPoolManager != nullptr)ObjectPoolManager->GetDoor(this,
							FTransform(
								FRotator(0.f, 0.f, 0.f),
								FVector(
									i * 400.f,
									j * 400.f - 200.f,
									600.f)
							)
						);
						AddGrid(
							FTransform(
								FRotator(0.f, 0.f, 0.f),
								FVector(
									i * 400.f,
									j * 400.f - 200.f,
									600.f)
							),
							STAGE_GRID_CORRIDOR_WALL_DOOR
						);
						AddGrid(
							FTransform(
								FRotator(0.f, 0.f, 0.f),
								FVector(
									i * 400.f,
									j * 400.f - 200.f,
									600.f)
							),
							STAGE_GRID_CORRIDOR_WALL_DOOR_BASE
						);

						AddGrid(
							FTransform(
								FRotator(0.f, 0.f, 0.f),
								FVector(
									i * 400.f,
									j * 400.f - 200.f,
									600.f + 240.f)
							),
							STAGE_GRID_CORRIDOR_WALL_BASE
						);

						AddGrid(
							FTransform(
								FRotator(0.f, 90.f, 0.f),
								FVector(
									i * 400.f,
									j * 400.f - 200.f,
									600.f + 300.f)
							),
							STAGE_GRID_CORRIDOR_CEILING_ARCH_WALL
						);
					}
				}
				else
				{
					AddGrid(
						FTransform(
							FRotator(0.f, 90.f, 0.f),
							FVector(i * 400.f, j * 400.f, 400.f + (3 - j) * 100.f)
						),
						STAGE_GRID_CORRIDOR_FLOOR_CLAMP
					);
					AddGrid(
						FTransform(
							FRotator(0.f, 90.f, 0.f),
							FVector(
								i * 400.f + 200.f,
								j * 400.f,
								400.f + (3 - j) * 100.f)
						),
						STAGE_GRID_CORRIDOR_WALL_CLAMP1_BASE
					);
					AddGrid(
						FTransform(
							FRotator(0.f, 90.f, 0.f),
							FVector(
								i * 400.f + 200.f,
								j * 400.f,
								400.f + (3 - j) * 100.f)
						),
						STAGE_GRID_CORRIDOR_WALL_CLAMP1
					);
					AddGrid(
						FTransform(
							FRotator(0.f, 90.f, 0.f),
							FVector(
								i * 400.f + 200.f,
								j * 400.f,
								400.f + (3 - j) * 100.f + 240.f)
						),
						STAGE_GRID_CORRIDOR_WALL_CLAMP1_BASE
					);


					AddGrid(
						FTransform(
							FRotator(0.f, 90.f, 0.f),
							FVector(
								i * 400.f - 200.f,
								j * 400.f,
								400.f + (3 - j) * 100.f)
						),
						STAGE_GRID_CORRIDOR_WALL_CLAMP1_BASE
					);
					AddGrid(
						FTransform(
							FRotator(0.f, 90.f, 0.f),
							FVector(
								i * 400.f - 200.f,
								j * 400.f,
								400.f + (3 - j) * 100.f)
						),
						STAGE_GRID_CORRIDOR_WALL_CLAMP1
					);
					AddGrid(
						FTransform(
							FRotator(0.f, 90.f, 0.f),
							FVector(
								i * 400.f - 200.f,
								j * 400.f,
								400.f + (3 - j) * 100.f + 240.f)
						),
						STAGE_GRID_CORRIDOR_WALL_CLAMP1_BASE
					);

					AddGrid(
						FTransform(
							FRotator(0.f, -90.f, 0.f),
							FVector(
								i * 400.f,
								j * 400.f,
								400.f + (3 - j) * 100.f)
						),
						STAGE_GRID_CORRIDOR_CEILING3
					);
					AddGrid(
						FTransform(
							FRotator(0.f, FMath::FRandRange(0.f, 360.f), 0.f),
							FVector(
								i * 400.f + 200.f,
								j * 400.f - 200.f,
								400.f + (3 - j) * 100.f)
						),
						STAGE_GRID_CORRIDOR_PHILAR
					);
					AddGrid(
						FTransform(
							FRotator(0.f, FMath::FRandRange(0.f, 360.f), 0.f),
							FVector(
								i * 400.f - 200.f,
								j * 400.f - 200.f,
								400.f + (3 - j) * 100.f)
						),
						STAGE_GRID_CORRIDOR_PHILAR
					);
				}
			}
			else if (jM)
			{
				AddGrid(
					FTransform(
						FRotator(0.f, 180.f, 0.f),
						FVector(i * 400.f, j * 400.f, 200.f + i * 100.f)
					),
					STAGE_GRID_CORRIDOR_FLOOR_CLAMP
				);

				AddGrid(
					FTransform(
						FRotator(0.f, 180.f, 0.f),
						FVector(
							i * 400.f,
							j * 400.f + 200.f,
							200.f + i * 100.f)
					),
					STAGE_GRID_CORRIDOR_WALL_CLAMP1_BASE
				);
				AddGrid(
					FTransform(
						FRotator(0.f, 180.f, 0.f),
						FVector(
							i * 400.f,
							j * 400.f + 200.f,
							200.f + i * 100.f)
					),
					STAGE_GRID_CORRIDOR_WALL_CLAMP1
				);
				AddGrid(
					FTransform(
						FRotator(0.f, 180.f, 0.f),
						FVector(
							i * 400.f,
							j * 400.f + 200.f,
							440.f + i * 100.f)
					),
					STAGE_GRID_CORRIDOR_WALL_CLAMP1_BASE
				);

				AddGrid(
					FTransform(
						FRotator(0.f, 180.f, 0.f),
						FVector(
							i * 400.f,
							j * 400.f - 200.f,
							200.f + i * 100.f)
					),
					STAGE_GRID_CORRIDOR_WALL_CLAMP1_BASE
				);
				AddGrid(
					FTransform(
						FRotator(0.f, 180.f, 0.f),
						FVector(
							i * 400.f,
							j * 400.f - 200.f,
							200.f + i * 100.f)
					),
					STAGE_GRID_CORRIDOR_WALL_CLAMP1
				);
				AddGrid(
					FTransform(
						FRotator(0.f, 180.f, 0.f),
						FVector(
							i * 400.f,
							j * 400.f - 200.f,
							440.f + i * 100.f)
					),
					STAGE_GRID_CORRIDOR_WALL_CLAMP1_BASE
				);

				AddGrid(
					FTransform(
						FRotator(0.f, 0.f, 0.f),
						FVector(
							i * 400.f,
							j * 400.f,
							200.f + i * 100.f)
					),
					STAGE_GRID_CORRIDOR_CEILING3
				);
				AddGrid(
					FTransform(
						FRotator(0.f, FMath::FRandRange(0.f, 360.f), 0.f),
						FVector(
							i * 400.f + 200.f,
							j * 400.f - 200.f,
							200.f + i * 100.f)
					),
					STAGE_GRID_CORRIDOR_PHILAR
				);
				AddGrid(
					FTransform(
						FRotator(0.f, FMath::FRandRange(0.f, 360.f), 0.f),
						FVector(
							i * 400.f + 200.f,
							j * 400.f + 200.f,
							200.f + i * 100.f)
					),
					STAGE_GRID_CORRIDOR_PHILAR
				);
			}
		}
	}
}

void ACGeneratedStair::BeginPlay()
{
	Super::BeginPlay();

	ObjectPoolManager = Cast<IIObjectPoolManager>(GetWorld()->GetGameState());
	EntranceCollider->OnComponentBeginOverlap.AddDynamic(this, &ACGeneratedStair::OnEntranceBeginOverlap);
	ExitCollider->OnComponentBeginOverlap.AddDynamic(this, &ACGeneratedStair::OnExitBeginOverlap);
}

void ACGeneratedStair::TurnOnLights(IIGameStateLightManager* LightManager)
{
	for (int32 i = 0; i < CandlePosArr.Num(); i++)
	{
		if (CandleArr[i] == nullptr)
		{
			UPointLightComponent* PLC = LightManager->GetPointLightComponent(this);
			if (PLC != nullptr)
			{
				PLC->Activate();
				PLC->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
				PLC->SetRelativeLocation(CandlePosArr[i]);
				CandleArr[i] = PLC;
			}
		}
	}
}

void ACGeneratedStair::TurnOffLights(IIGameStateLightManager* LightManager)
{
	for (int32 i = 0; i < CandlePosArr.Num(); i++)
	{
		if (CandleArr[i] == nullptr) continue;
		LightManager->ReturnPointLightComponent(CandleArr[i]);
		CandleArr[i] = nullptr;
	}
}
