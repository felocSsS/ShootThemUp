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

}


void ASTUBaseWeapon::Fire()
{
    /*UE_LOG(LogBaseWeapon, Display, TEXT("Fire!"))*/

    MakeShot();
}

void ASTUBaseWeapon::MakeShot()
{
    if (!GetWorld()) return;

    const auto Player = Cast<ACharacter>(GetOwner());
    if (!Player) return;

    const auto Controller = Player->GetController<APlayerController>();
    if (!Controller) return;

    FVector ViewLocation; 
    FRotator ViewRotation; 
    Controller->GetPlayerViewPoint(ViewLocation, ViewRotation);

	const FTransform SocketTransform = WeaponMesh->GetSocketTransform(MuzzeleSocketName);
    const FVector TraceStart = ViewLocation;              // SocketTransform.GetLocation();
    const FVector ShootDirection = ViewRotation.Vector(); // SocketTransform.GetRotation().GetForwardVector();
    const FVector TraceEnd = TraceStart + ShootDirection * TraceMaxDistance;

    FCollisionQueryParams CollisionParam;
    CollisionParam.AddIgnoredActor(GetOwner());

    FHitResult HitResult;
    GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECollisionChannel::ECC_Visibility, CollisionParam);

    if (HitResult.bBlockingHit)
    {
        DrawDebugLine(GetWorld(), SocketTransform.GetLocation(), HitResult.ImpactPoint, FColor::Red, false, 3.f, 0, 3.f);
        DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 10.f, 24, FColor::Red, false, 5.f);  
        
        /*UE_LOG(LogBaseWeapon, Display, TEXT("BONE: %s"), *HitResult.BoneName.ToString());*/
    }
    else
    {
        DrawDebugLine(GetWorld(), SocketTransform.GetLocation(), TraceEnd, FColor::Red, false, 3.f, 0, 3.f);
    }
}