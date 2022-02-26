// Shoot Them Up Game. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "STUBaseWeapon.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnClipEmptySignature);

class USkeletalMeshComponent;

USTRUCT(BlueprintType)
struct FAmmoData
{
	GENERATED_USTRUCT_BODY()

		UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
		int32 Bullets;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon", meta = (EditCondition = "!Infinite"))
		int32 Clips;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
		bool Infinite;
};


UCLASS()
class SHOOTTHEMUP_API ASTUBaseWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASTUBaseWeapon();

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
    USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    FName MuzzeleSocketName = "MuzzleSocket"; 

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    float TraceMaxDistance = 15000.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    FAmmoData DefaultAmmo{15, 10, false};

	virtual void BeginPlay() override;

	virtual void MakeShot();

    virtual bool GetTraceData(FVector& TraceStart, FVector& TraceEnd, FVector& EditedDir) const;

    void MakeHit(FHitResult& HitResult, const FVector& TraceStart, const FVector& TraceEnd);
    void DecreaseAmmo();
    void LogAmmo();

	bool GetPlayerViewPoint(FVector& ViewLocation, FRotator& ViewRotation) const;
    bool ISAmmoEmpty() const;
    bool IsClipEmpty() const;

	float GetDegreesBetweenMuzzleAndTrace(FHitResult ImpactPoint, FVector ShotDirection) const;

	APlayerController* GetPlayerController() const;

	FVector GetMuzzleWorldLocation() const;


public:	
	virtual void StartFire();
	virtual void StopFire();

	void ChangeClip();

	bool CanReload() const;

	FOnClipEmptySignature OnClipEmpty;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	bool AutoReload = false;

private:
    FAmmoData CurrentAmmo;
};
