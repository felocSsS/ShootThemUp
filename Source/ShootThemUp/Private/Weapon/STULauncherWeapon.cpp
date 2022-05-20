// Shoot Them Up Game. All Rights Reserved.


#include "Weapon/STULauncherWeapon.h"
#include "Weapon/STUProjectile.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY_STATIC(LogRifleWeapon, All, All);

void ASTULauncherWeapon::StartFire()
{
    if (IsClipEmpty())
    {
        UGameplayStatics::SpawnSoundAtLocation(GetWorld(), NoAmmoSound, GetActorLocation());   
        UE_LOG(LogTemp, Display, TEXT("Text"));
        return;
    }
    MakeShot();
}

void ASTULauncherWeapon::MakeShot()
{
    if (!GetWorld() || ISAmmoEmpty()) return;

    FVector TraceStart, TraceEnd, EditedDirection;
    if (!GetTraceData(TraceStart, TraceEnd, EditedDirection)) return;

    FHitResult HitResult;
    TArray<FHitResult> HitResults;
    //MakeHit(HitResult, TraceStart, TraceEnd);
    MakeMultiHit(HitResults, TraceStart, TraceEnd);

    for (FHitResult Hit : HitResults)
    {
        if (GetDegreesBetweenMuzzleAndTrace(Hit, EditedDirection) < 90)
        {
            HitResult = Hit;
            break;
        }
    }

    const FVector EndPoint = HitResult.bBlockingHit ? HitResult.ImpactPoint : TraceEnd;
    const FVector Direction = (EndPoint - GetMuzzleWorldLocation()).GetSafeNormal();

    const FTransform SpawnTransform(FRotator::ZeroRotator, GetMuzzleWorldLocation());
    ASTUProjectile* Projectile = GetWorld()->SpawnActorDeferred<ASTUProjectile>(ProjectileClass, SpawnTransform);
    
    if (Projectile)
    {
        Projectile->SetShotDirection(Direction);
        Projectile->FinishSpawning(SpawnTransform);
        Projectile->SetOwner(GetOwner());
        DecreaseAmmo();
        SpawnMuzzleFX();
        UGameplayStatics::SpawnSoundAttached(FireSound, WeaponMesh, MuzzeleSocketName);
    }
}
