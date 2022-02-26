// Shoot Them Up Game. All Rights Reserved.


#include "Weapon/STURifleWeapon.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

void ASTURifleWeapon::StartFire()
{
    if (IsClipEmpty()) return;
    GetWorldTimerManager().SetTimer(ShotTimerHandle, this, &ASTURifleWeapon::MakeShot, TimeBetweenShots, true);
    MakeShot();
}

void ASTURifleWeapon::StopFire()
{
    GetWorldTimerManager().ClearTimer(ShotTimerHandle);
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
    MakeHit(HitResult, TraceStart, TraceEnd);

    if (GetDegreesBetweenMuzzleAndTrace(HitResult, EditedDirection) < 90)
    {
        if (HitResult.bBlockingHit)
        {
            MakeDamage(HitResult);
            DrawDebugLine(GetWorld(), GetMuzzleWorldLocation(), HitResult.ImpactPoint, FColor::Red, false, 3.f, 0, 3.f);
            DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 10.f, 24, FColor::Red, false, 5.f);
        }
        else
        {
            // логика, если трейс не задел ничего
        }
    }
    else
    {
        // локига, выстрела сквозь объект
    }
    DecreaseAmmo();
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

    DamagedActor->TakeDamage(DamageAmount, FDamageEvent{}, GetPlayerController(), this);
}