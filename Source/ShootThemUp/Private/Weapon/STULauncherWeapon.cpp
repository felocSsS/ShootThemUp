// Shoot Them Up Game. All Rights Reserved.


#include "Weapon/STULauncherWeapon.h"
#include "Weapon/STUProjectile.h"

DEFINE_LOG_CATEGORY_STATIC(LogRifleWeapon, All, All);

void ASTULauncherWeapon::StartFire()
{
    if (IsClipEmpty()) return;
    MakeShot();
}

void ASTULauncherWeapon::MakeShot()
{
    if (!GetWorld() || ISAmmoEmpty()) return;

    FVector TraceStart, TraceEnd, EditedDirection;
    if (!GetTraceData(TraceStart, TraceEnd, EditedDirection)) return;

    FHitResult HitResult;
    MakeHit(HitResult, TraceStart, TraceEnd);

    if (GetDegreesBetweenMuzzleAndTrace(HitResult, EditedDirection) < 90)
    {
        const FVector EndPoint = HitResult.bBlockingHit ? HitResult.ImpactPoint : TraceEnd;
        const FVector Direction = (EndPoint - GetMuzzleWorldLocation()).GetSafeNormal();

        const FTransform SpawnTransform(FRotator::ZeroRotator, GetMuzzleWorldLocation());
        ASTUProjectile* Projectile = GetWorld()->SpawnActorDeferred<ASTUProjectile>(ProjectileClass, SpawnTransform);
    
        if (Projectile)
        {
            Projectile->SetShotDirection(Direction);
            Projectile->FinishSpawning(SpawnTransform);
            Projectile->SetOwner(GetOwner());
        }
    }
    DecreaseAmmo();
}
