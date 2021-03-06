// Shoot Them Up Game. All Rights Reserved.


#include "Weapon/STUBaseWeapon.h"
#include "Components/SkeletalmeshComponent.h"
#include "Engine/World.h"
#include "DrawDebughelpers.h"
#include "GameFramework/Character.h"
#include "GameFramework/Controller.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"

DEFINE_LOG_CATEGORY_STATIC(LogBaseWeapon, All, All);

ASTUBaseWeapon::ASTUBaseWeapon()
{
	PrimaryActorTick.bCanEverTick = false;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>("WeaponMesh");
    SetRootComponent(WeaponMesh);

}

void ASTUBaseWeapon::BeginPlay()
{
	Super::BeginPlay();
	
	check(WeaponMesh);
    checkf(DefaultAmmo.Bullets > 0, TEXT("Bullets count couldn't be less or equal zero"));
    checkf(DefaultAmmo.Clips > 0, TEXT("Clips count couldn't be less or equal zero"));
    CurrentAmmo = DefaultAmmo;
}

void ASTUBaseWeapon::StartFire()
{
    
}

void ASTUBaseWeapon::StopFire()
{
  
}

void ASTUBaseWeapon::MakeShot()
{
    
}

void ASTUBaseWeapon::MakeHit(FHitResult& HitResult, const FVector& TraceStart, const FVector& TraceEnd)
{
    if (!GetWorld()) return;

    FCollisionQueryParams CollisionParam;
    CollisionParam.AddIgnoredActor(GetOwner());
    CollisionParam.bReturnPhysicalMaterial = true;

    GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECollisionChannel::ECC_Visibility, CollisionParam);
}

void ASTUBaseWeapon::MakeMultiHit(TArray<FHitResult>& HitResult, const FVector& TraceStart, const FVector& TraceEnd)
{
    if(!GetWorld()) return;

    FCollisionObjectQueryParams CollisionObjectTypes;
    CollisionObjectTypes.AddObjectTypesToQuery(ECollisionChannel::ECC_PhysicsBody);
    CollisionObjectTypes.AddObjectTypesToQuery(ECollisionChannel::ECC_Pawn);
    CollisionObjectTypes.AddObjectTypesToQuery(ECollisionChannel::ECC_WorldStatic);
    
    FCollisionQueryParams CollisionParam;
    CollisionParam.AddIgnoredActor(GetOwner());
    CollisionParam.bReturnPhysicalMaterial = true;

    GetWorld()->LineTraceMultiByObjectType(HitResult, TraceStart, TraceEnd, CollisionObjectTypes, CollisionParam);
}

void ASTUBaseWeapon::DecreaseAmmo()
{
    if(CurrentAmmo.Bullets == 0) return;

    CurrentAmmo.Bullets--;

    if (IsClipEmpty() && !ISAmmoEmpty())
    {
        StopFire();
        OnClipEmpty.Broadcast(this);
    }
}

void ASTUBaseWeapon::ChangeClip()
{
    if (!CurrentAmmo.Infinite)
    {
        if (CurrentAmmo.Clips == 0) return;
        CurrentAmmo.Clips -= 1;    
    }
    CurrentAmmo.Bullets = DefaultAmmo.Bullets;
}

bool ASTUBaseWeapon::CanReload() const
{
    return CurrentAmmo.Bullets < DefaultAmmo.Bullets && CurrentAmmo.Clips > 0;
}

bool ASTUBaseWeapon::TryToAddAmmo(int32 ClipsAmount)
{
    if (CurrentAmmo.Infinite || IsAmmoFull() || ClipsAmount <= 0) return false;

    if (ISAmmoEmpty())
    {
        CurrentAmmo.Clips = FMath::Clamp(ClipsAmount, 0, DefaultAmmo.Clips + 1);
        OnClipEmpty.Broadcast(this);
    }
    else if (CurrentAmmo.Clips < DefaultAmmo.Clips)
    {
        const auto NextClipsAmount = CurrentAmmo.Clips + ClipsAmount;
        if (DefaultAmmo.Clips - NextClipsAmount >= 0)
        {
            CurrentAmmo.Clips = NextClipsAmount;
        }
        else
        {
            CurrentAmmo.Clips = DefaultAmmo.Clips;
            CurrentAmmo.Bullets = DefaultAmmo.Bullets;
        }
    }
    else
    {
        CurrentAmmo.Clips = DefaultAmmo.Clips;
        CurrentAmmo.Bullets = DefaultAmmo.Bullets;
    }

    return true;
}

bool ASTUBaseWeapon::GetPlayerViewPoint(FVector& ViewLocation, FRotator& ViewRotation) const 
{
    const auto STUCharacter = Cast<ACharacter>(GetOwner());
    if (!STUCharacter) return false;

    if (STUCharacter->IsPlayerControlled())
    {
        const auto Controller = STUCharacter->GetController<APlayerController>();
        if (!Controller) return false;

        Controller->GetPlayerViewPoint(ViewLocation, ViewRotation);
    }
    else
    {
        ViewLocation = GetMuzzleWorldLocation();
        ViewRotation = WeaponMesh->GetSocketRotation(MuzzeleSocketName);
    }

    return true;
}

bool ASTUBaseWeapon::ISAmmoEmpty() const
{
    return !CurrentAmmo.Infinite && CurrentAmmo.Clips == 00 && IsClipEmpty();
}

bool ASTUBaseWeapon::IsClipEmpty() const
{
    return CurrentAmmo.Bullets == 0;
}

bool ASTUBaseWeapon::IsAmmoFull() const
{
    return CurrentAmmo.Clips == DefaultAmmo.Clips && 
           CurrentAmmo.Bullets == DefaultAmmo.Bullets;
}

bool ASTUBaseWeapon::GetTraceData(FVector& TraceStart, FVector& TraceEnd, FVector& EditedDir) const
{
    FVector ViewLocation; 
    FRotator ViewRotation; 
    if(!GetPlayerViewPoint(ViewLocation, ViewRotation)) return false;

    TraceStart = ViewLocation;
    const FVector ShootDirection = ViewRotation.Vector();
    EditedDir = ShootDirection;
    TraceEnd = TraceStart + ShootDirection * TraceMaxDistance;
    return true;
}

float ASTUBaseWeapon::GetDegreesBetweenMuzzleAndTrace(FHitResult ImpactPoint, FVector ShotDirection) const
{
    //FVector CamLoc;
    //FRotator CamRot;
    //if (!GetPlayerViewPoint(CamLoc, CamRot)) return 0.f; // ?????? ?? ?????? ???? ?????????, ??? ????? ??????????
    //FVector CameraForwardVec = CamRot.Vector();

    const FVector AdjustedDirection = (ImpactPoint.ImpactPoint - GetMuzzleWorldLocation()).GetSafeNormal();
    float DegreesBetween = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(AdjustedDirection, ShotDirection))); 
    
    return DegreesBetween;
}

FVector ASTUBaseWeapon::GetMuzzleWorldLocation() const 
{
    return WeaponMesh->GetSocketLocation(MuzzeleSocketName);
}

UNiagaraComponent* ASTUBaseWeapon::SpawnMuzzleFX()
{
    return UNiagaraFunctionLibrary::SpawnSystemAttached(MuzzleFX, WeaponMesh, MuzzeleSocketName, FVector::ZeroVector,
                                                   FRotator::ZeroRotator, EAttachLocation::SnapToTarget, true);
}
