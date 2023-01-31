// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

UENUM(BlueprintType)
enum State
{
	None,
	Alarm,
	Alert,
	Suspicious,
	Patrol,
	Death,
	Aim,
	Run,
	Reloading,
};

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Components/AudioComponent.h"
#include "MyCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTakeDamageSignature);

UCLASS()
class SAVETHEPRESIDENT_API AMyCharacter : public ACharacter
{
	GENERATED_BODY()
		// Attribute
public:
	UPROPERTY(BlueprintReadOnly) TEnumAsByte<State> state;
	bool bIsCrouched;

	int DAMAGE_BODY;
	int DAMAGE_HEAD;
	USphereComponent* headCollider;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray< class USoundBase*> HurtSouds;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray< class USoundBase*> AlertSounds;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class USoundBase* shootSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		USoundBase* StepSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		USoundAttenuation* SoundAttenuation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAudioComponent* FirstAudioComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAudioComponent* SecondAudioComp;

	int randSound;
protected:
	int life;
	UCharacterMovementComponent* characterMovement;
	
	// Shoot
	FVector aimDirection;
	FVector startShoot;
	int MUN_MAX;
	int currentMun;
	FVector recoil;

	float SPEED_WALK;

	// Laser Shoot
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) UNiagaraSystem* NS_Laser;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) UNiagaraComponent* laser;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) UNiagaraSystem* NS_LaserImpact;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) UNiagaraComponent* laserImpact;

	FHitResult outHitShoot;
	bool bIsHit;
	float lastTimePlayed;

private:
	int MAX_LIFE;
	float AIM_SPEED_WALK;
	float AIM_CROUCH_SPEED_WALK;
	float SPEED_WALK_CROUCH;


		// Methode
public:	
	AMyCharacter();

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void MyTakeDamage(const int damage);

	UPROPERTY(BlueprintAssignable)
		FOnTakeDamageSignature OnTakeDamageDelegate;

	virtual void StartAim();
	virtual void EndAim();
	virtual void Shoot();
	void Reload();
	FVector GetHeadLocation();
	void UpdateLaserBeam();
	void StopShootLaser();

	void PlaySoundHurt();

	int GetLife() { return life; };

protected:

	virtual void BeginPlay() override;

	void SetLife(const int p_life);

	virtual void ComputeAimDirection();
	
	virtual void OnDeath();

	void DeactivateHeadSphereCollider();
	
	void PlaySoundShoot();

};
