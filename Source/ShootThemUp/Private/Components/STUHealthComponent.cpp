// Shoot Them Up Game. All Rights Reserved.


#include "Components/STUHealthComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/Controller.h"
#include "Engine/World.h"
#include "TimerManager.h" 
#include "Camera/CameraShakeBase.h"
#include "STUGameModeBase.h"
#include "Physicalmaterials/PhysicalMAterial.h"
#include "Perception/AISense_Damage.h"

DEFINE_LOG_CATEGORY_STATIC(LogHealthComponent, All, All)

USTUHealthComponent::USTUHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void USTUHealthComponent::BeginPlay()
{
	Super::BeginPlay();

    check(MaxHealth > 0);
	
	SetHealth(MaxHealth);
	
	AActor* ComponentOwner = GetOwner();

	if (ComponentOwner)
    {
        ComponentOwner->OnTakeAnyDamage.AddDynamic(this, &USTUHealthComponent::OnTakeAnyDamage);
        ComponentOwner->OnTakePointDamage.AddDynamic(this, &USTUHealthComponent::OnTakePointDamege);
        ComponentOwner->OnTakeRadialDamage.AddDynamic(this, &USTUHealthComponent::OnRadialDamage);
	}
}


bool USTUHealthComponent::TryToAddHealth(float HealthAmount)
{
    if (IsDead() || IsHealthFull()) return false;

    SetHealth(Health + HealthAmount);
    return true;
}

bool USTUHealthComponent::IsHealthFull() const
{
    return FMath::IsNearlyEqual(Health, MaxHealth);
}

void USTUHealthComponent::OnTakeAnyDamage(AActor* DamageActor, float Damage, const class UDamageType* DamageType,
                                               class AController* InstigatedBy, AActor* DamageCauser)
{
}

void USTUHealthComponent::HealthUpdate()
{
    SetHealth(Health + HealModifier);

    if (IsHealthFull() && GetWorld())
    {
        GetWorld()->GetTimerManager().ClearTimer(HealTimerHandle);
    }
}

void USTUHealthComponent::SetHealth(float NewHealth)
{
    const auto NextHealth = FMath::Clamp(NewHealth, 0.f, MaxHealth);
    const auto HealthDelta = NextHealth - Health;

    Health = NextHealth;
    OnHealthChanged.Broadcast(Health, HealthDelta);
}

void USTUHealthComponent::PlayCameraShake()
{
    if (IsDead()) return;

    const auto Player = Cast<APawn>(GetOwner());
    if (!Player) return;

    const auto Controller = Player->GetController<APlayerController>();
    if (!Controller || !Controller->PlayerCameraManager) return;

    Controller->PlayerCameraManager->StartCameraShake(CameraShake);
}

void USTUHealthComponent::Killed(AController* KillerController)
{
    const auto GameMode = Cast<ASTUGameModeBase>(GetWorld()->GetAuthGameMode());
    if (!GameMode) return;

    const auto Player = Cast<APawn>(GetOwner());
    const auto VictimController = Player ? Player->Controller : nullptr;

    GameMode->Killed(KillerController, VictimController);
}

void USTUHealthComponent::ApplyDamage(float Damage, AController* InstigatedBy)
{
    if (Damage <= 0.f || IsDead() || !GetWorld())
        return;

    SetHealth(Health - Damage);

    if (IsDead())
    {
        Killed(InstigatedBy);
        OnDeath.Broadcast();
        GetWorld()->GetTimerManager().ClearTimer(HealTimerHandle);
    }
    else if (AutoHeal)
    {
        GetWorld()->GetTimerManager().SetTimer(HealTimerHandle, this, &USTUHealthComponent::HealthUpdate,
                                               HealUpdateTime, true, HealDelay);
    }

    PlayCameraShake();
    ReportDamageEvent(Damage, InstigatedBy);
}

float USTUHealthComponent::GetPointDamageModifier(AActor* DamagedActor, const FName& BoneName)
{
    const auto Character = Cast<ACharacter>(DamagedActor);
    if (!Character || !Character->GetMesh() || !Character->GetMesh()->GetBodyInstance(BoneName))
        return 1.0f;

    const auto PhysMaterial = Character->GetMesh()->GetBodyInstance(BoneName)->GetSimplePhysicalMaterial();
    if (!PhysMaterial || !DamageModifiers.Contains(PhysMaterial)) return 1.0f;

    return DamageModifiers[PhysMaterial];
}

void USTUHealthComponent::OnTakePointDamege(AActor* DamagedActor, float Damage,
                                            AController* InstigatedBy, FVector HitLocation,
                                            UPrimitiveComponent* FHitComponent, FName BoneName,
                                            FVector ShotFromDirection, const UDamageType* DamageType,
                                            AActor* DamageCauser)
{
    const auto FinalDamage = Damage * GetPointDamageModifier(DamagedActor, BoneName);
    ApplyDamage(Damage, InstigatedBy);
}

void USTUHealthComponent::OnRadialDamage(AActor* DamagedActor, float Damage,
                                         const UDamageType* DamageType, FVector Origin, FHitResult HitInfo,
                                         AController* InstigatedBy, AActor* DamageCauser)
{
    ApplyDamage(Damage, InstigatedBy);
}

void USTUHealthComponent::ReportDamageEvent(float Damage, AController* InstigatedBy)
{
    if (!GetWorld() || !InstigatedBy || !InstigatedBy->GetPawn() || !GetOwner()) return;

    UAISense_Damage::ReportDamageEvent(GetWorld(), GetOwner(), InstigatedBy->GetPawn(), Damage,
                                       InstigatedBy->GetPawn()->GetActorLocation(), GetOwner()->GetActorLocation());
}
