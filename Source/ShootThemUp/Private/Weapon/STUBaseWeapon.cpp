// Shoot Them Up Game. All Rights Reserved.


#include "Weapon/STUBaseWeapon.h"
#include "Components/SkeletalmeshComponent.h"
#include "Engine/World.h"
#include "DrawDebughelpers.h"
#include "GameFramework/Character.h"
#include "GameFramework/Controller.h"

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

    GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECollisionChannel::ECC_Visibility, CollisionParam);
}

void ASTUBaseWeapon::DecreaseAmmo()
{
    if(CurrentAmmo.Bullets == 0) return;

    CurrentAmmo.Bullets--;
    LogAmmo();

    if (IsClipEmpty() && !ISAmmoEmpty())
    {
        StopFire();
        OnClipEmpty.Broadcast();
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

void ASTUBaseWeapon::LogAmmo()
{
    FString AmmoInfo = "Ammo " + FString::FromInt(CurrentAmmo.Bullets) + " / ";
    AmmoInfo += CurrentAmmo.Infinite ? "Infinite" : FString::FromInt(CurrentAmmo.Clips);
    UE_LOG(LogBaseWeapon, Display, TEXT("%s"), *AmmoInfo);
}

APlayerController* ASTUBaseWeapon::GetPlayerController() const
{
    const auto Player = Cast<ACharacter>(GetOwner());
    if (!Player) return nullptr;

    return Player->GetController<APlayerController>();
}

bool ASTUBaseWeapon::GetPlayerViewPoint(FVector& ViewLocation, FRotator& ViewRotation) const 
{
    const auto Controller = GetPlayerController();
    if (!Controller) return false;

    Controller->GetPlayerViewPoint(ViewLocation, ViewRotation);
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
    //if (!GetPlayerViewPoint(CamLoc, CamRot)) return 0.f; // исходя из логики игры продумать, что будет возвращать
    //FVector CameraForwardVec = CamRot.Vector();

    const FVector AdjustedDirection = (ImpactPoint.ImpactPoint - GetMuzzleWorldLocation()).GetSafeNormal();
    float DegreesBetween = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(AdjustedDirection, ShotDirection))); 
    
    return DegreesBetween;
}

FVector ASTUBaseWeapon::GetMuzzleWorldLocation() const 
{
    return WeaponMesh->GetSocketLocation(MuzzeleSocketName);
}