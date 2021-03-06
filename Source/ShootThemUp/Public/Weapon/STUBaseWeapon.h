// Shoot Them Up Game. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "STUCoreTypes.h"
#include "STUBaseWeapon.generated.h"

class USkeletalMeshComponent;
class UNiagaraSystem;
class UNiagaraComponent;
class USoundCue;

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

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
    FWeaponUIData UIData;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "VFX")
    UNiagaraSystem* MuzzleFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sound")
    USoundCue* FireSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sound")
    USoundCue* NoAmmoSound;

	virtual void BeginPlay() override;

	virtual void MakeShot();

    virtual bool GetTraceData(FVector& TraceStart, FVector& TraceEnd, FVector& EditedDir) const;

    void MakeHit(FHitResult& HitResult, const FVector& TraceStart, const FVector& TraceEnd);
    void MakeMultiHit(TArray<FHitResult>& HitResult, const FVector& TraceStart, const FVector& TraceEnd);
    void DecreaseAmmo();

	bool GetPlayerViewPoint(FVector& ViewLocation, FRotator& ViewRotation) const;
    bool IsClipEmpty() const;

	float GetDegreesBetweenMuzzleAndTrace(FHitResult ImpactPoint, FVector ShotDirection) const;

	FVector GetMuzzleWorldLocation() const;

	UNiagaraComponent* SpawnMuzzleFX();

public:	
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	bool AutoReload = false;

	virtual void StartFire();
	virtual void StopFire();

	void ChangeClip();

	bool CanReload() const;
    bool TryToAddAmmo(int32 ClipsAmount);
    bool ISAmmoEmpty() const;
    bool IsAmmoFull() const;

	virtual void Zoom(bool Enabled){};

	FOnClipEmptySignature OnClipEmpty;

	FWeaponUIData GetUIData() const { return UIData; }
    FAmmoData GetAmmoData() const { return CurrentAmmo; }

private:
    FAmmoData CurrentAmmo;
};
