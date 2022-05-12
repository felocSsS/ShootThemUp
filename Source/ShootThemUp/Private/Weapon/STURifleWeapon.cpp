// Shoot Them Up Game. All Rights Reserved.


#include "Weapon/STURifleWeapon.h"
#include "Engine/World.h"
#include "Weapon/Components/STUWeaponFXComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"

ASTURifleWeapon::ASTURifleWeapon()
{
    WeaponFXComponent = CreateDefaultSubobject<USTUWeaponFXComponent>("WeaponFXComponent");
}

void ASTURifleWeapon::BeginPlay()
{
    Super::BeginPlay();
    check(WeaponFXComponent);
} 

void ASTURifleWeapon::StartFire()
{
    if (IsClipEmpty()) return;
    InitMuzzleFX();
    GetWorldTimerManager().SetTimer(ShotTimerHandle, this, &ASTURifleWeapon::MakeShot, TimeBetweenShots, true);
    MakeShot();
}

void ASTURifleWeapon::StopFire()
{
    GetWorldTimerManager().ClearTimer(ShotTimerHandle);
    SetMuzzleFXVisibility(false);
}


void ASTURifleWeapon::MakeShot()
{
    if (!GetWorld() || ISAmmoEmpty())
    {
        StopFire();
        return;
    }

    FVector TraceStart, TraceEnd, EditedDirection;
    if (!GetTraceData(TraceStart, TraceEnd, EditedDirection))
    {
        StopFire();
        return;
    }

    FHitResult HitResult;
    TArray<FHitResult> HitResults;
    //MakeHit(HitResult, TraceStart, TraceEnd);
    MakeMultiHit(HitResults, TraceStart, TraceEnd);
    FVector TraceFXEnd = TraceEnd;

    for (FHitResult Hit : HitResults)
    {
        if (GetDegreesBetweenMuzzleAndTrace(Hit, EditedDirection) < 90)
        {
            HitResult = Hit;
            break;
        }
    }

    if (HitResult.bBlockingHit)
    {
        TraceFXEnd = HitResult.ImpactPoint;
        MakeDamage(HitResult);
        WeaponFXComponent->PlayImpactFX(HitResult);
        SpawnTraceFX(GetMuzzleWorldLocation(), TraceFXEnd);
        DecreaseAmmo();
    }
    else
    {
        TraceFXEnd = TraceStart + EditedDirection * 1000;
        SpawnTraceFX(GetMuzzleWorldLocation(), TraceFXEnd);
        DecreaseAmmo();
    }
}

bool ASTURifleWeapon::GetTraceData(FVector& TraceStart, FVector& TraceEnd, FVector& EditedDir) const
{
    FVector ViewLocation;
    FRotator ViewRotation;
    if (!GetPlayerViewPoint(ViewLocation, ViewRotation)) return false;

    TraceStart = ViewLocation;
    const auto HalfRand = FMath::DegreesToRadians(BulletSpread);
    const FVector ShootDirection = FMath::VRandCone(ViewRotation.Vector(), HalfRand);
    EditedDir = ShootDirection;
    TraceEnd = TraceStart + ShootDirection * TraceMaxDistance;
    return true;
}

void ASTURifleWeapon::MakeDamage(const FHitResult& HitResult)
{
    const auto DamagedActor = HitResult.GetActor();
    if (!DamagedActor)
        return;

    DamagedActor->TakeDamage(DamageAmount, FDamageEvent{}, GetController(), this);
}

void ASTURifleWeapon::InitMuzzleFX()
{
    if (!MuzzleFXComponent)
    {
        MuzzleFXComponent = SpawnMuzzleFX();
    }
    SetMuzzleFXVisibility(true);
}

void ASTURifleWeapon::SetMuzzleFXVisibility(bool Visible)
{
    if (MuzzleFXComponent)
    {
        MuzzleFXComponent->SetPaused(!Visible);
        MuzzleFXComponent->SetVisibility(Visible, true);
    }
}

void ASTURifleWeapon::SpawnTraceFX(const FVector& TraceStart, const FVector& TraceEnd)
{
    const auto TraceFXComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), TraceFX, TraceStart);
    if (TraceFXComponent)
    {
        TraceFXComponent->SetNiagaraVariableVec3(TraceTargetName, TraceEnd);
    }
}

AController* ASTURifleWeapon::GetController() const
{
    const auto Pawn = Cast<APawn>(GetOwner());
    return Pawn ? Pawn->GetController() : nullptr;
}
