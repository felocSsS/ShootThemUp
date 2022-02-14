// Shoot Them Up Game. All Rights Reserved.


#include "Components/STUWeaponComponent.h"
#include "Weapon/STUBaseWeapon.h"
#include "GameFramework/Character.h"

USTUWeaponComponent::USTUWeaponComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void USTUWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	SpawnWeapon();
}


void USTUWeaponComponent::SpawnWeapon()
{
    if (!GetWorld()) return;
    
    ACharacter* Character = Cast<ACharacter>(GetOwner());
    if (!Character) return;

    CurrentWeapon = GetWorld()->SpawnActor<ASTUBaseWeapon>(WeaponClass);
    if (!CurrentWeapon) return;

    FAttachmentTransformRules AttachmentsRoles(EAttachmentRule::SnapToTarget, false);
    CurrentWeapon->AttachToComponent(Character->GetMesh(), AttachmentsRoles, WeaponAttachPointName);
    CurrentWeapon->SetOwner(Character);
}

void USTUWeaponComponent::Fire()
{
    if (!CurrentWeapon) return;

    CurrentWeapon->Fire();
}


