// Shoot Them Up Game. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "STUCoreTypes.h"
#include "STUHealthComponent.generated.h"

class UPhysicalMaterial;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SHOOTTHEMUP_API USTUHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	USTUHealthComponent();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (ClampMin = "0.0", ClampMax = "1000.0"), Category = "Health")
    float MaxHealth = 100.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Heal")
    bool AutoHeal = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Heal", meta = (EditCondition = "AutoHeal"))
    float HealUpdateTime = 1.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Heal", meta = (EditCondition = "AutoHeal"))
    float HealDelay = 3.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Heal", meta = (EditCondition = "AutoHeal"))
    float HealModifier = 5.f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "VFX")
    TSubclassOf<UCameraShakeBase> CameraShake;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Health")
    TMap<UPhysicalMaterial*, float> DamageModifiers;

public:
	float GetHealth() const { return Health; }

	UFUNCTION(BlueprintCallable, Category = "Health")
    bool IsDead() const { return FMath::IsNearlyZero(Health); }

    UFUNCTION(BlueprintCallable, Category = "Health")
    float GetHealthPersent() const { return Health / MaxHealth; }

	FOnDeath OnDeath;
    FOnHealthChanged OnHealthChanged;

    bool TryToAddHealth(float HealthAmount);
    bool IsHealthFull() const;

private:
    float Health = 0.0f;
    FTimerHandle HealTimerHandle;

	UFUNCTION()
    void OnTakeAnyDamage(AActor* DamageActor, float Damage, const class UDamageType* DamageType,
                               class AController* InstigatedBy, AActor* DamageCauser);
    void HealthUpdate();
    void SetHealth(float NewHealth);
    void PlayCameraShake();
    void Killed(AController* KillerController);
    void ApplyDamage(float Damage, AController* InstigatedBy);
    float GetPointDamageModifier(AActor* DamagedActor, const FName& BoneName);
    UFUNCTION()
    void OnTakePointDamege(AActor* DamagedActor, float Damage, class AController* InstigatedBy,
                           FVector HitLocation, class UPrimitiveComponent*
                           FHitComponent, FName BoneName, FVector ShotFromDirection, const class UDamageType*
                           DamageType, AActor* DamageCauser);
    UFUNCTION()
    void OnRadialDamage(AActor* DamagedActor, float Damage,
                        const class UDamageType* DamageType, FVector Origin, FHitResult HitInfo,
                        class AController* InstigatedBy, AActor* DamageCauser);

    void ReportDamageEvent(float Damage, AController* InstigatedBy);
};
