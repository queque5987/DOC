#include "GameSystem/CHitBoxComponent.h"

UCHitBoxComponent::UCHitBoxComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCHitBoxComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UCHitBoxComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}
