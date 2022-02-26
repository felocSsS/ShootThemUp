// Shoot Them Up Game. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "STUProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;

UCLASS()
class SHOOTTHEMUP_API ASTUProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	ASTUProjectile();

	void SetShotDirection(const FVector& Direction) { ShotDirection = Direction; }

protected:

	UPROPERTY(VisibleAnywhere, Category = "Weapon")
    USphereComponent* CollisionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon")
    UProjectileMovementComponent* MovementComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    float DamageRadius = 200.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    float DamageAmount = 50.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    bool DoFullDamage = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    float LifeSeconds = 5.f;

	virtual void BeginPlay() override;

public:	

private:
    FVector ShotDirection;

	UFUNCTION()
    void OnProjectileHit(UPrimitiveComponent* HitComponent,
                         AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse,
                         const FHitResult& Hit);

	AController* GetController() const;
};
