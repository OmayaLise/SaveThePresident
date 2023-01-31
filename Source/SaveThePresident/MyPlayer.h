// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyCharacter.h"
#include "StealthLightLevelBar.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "Components/SpotLightComponent.h"
#include "MyPlayer.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChangePawnAimSignature, FColor, newColor);

UCLASS()
class SAVETHEPRESIDENT_API AMyPlayer : public AMyCharacter
{
	GENERATED_BODY()
	
		// Attribute
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseTurnRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseLookUpRate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera)
		TSubclassOf<UUserWidget> crosshairClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Grab)
		float GrabDistance;

	UUserWidget* crosshairInstance;

	FColor colorCrosshair;

	UPROPERTY(BlueprintAssignable)
		FOnChangePawnAimSignature OnChangePawnAimDelegate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = UI)
		TSubclassOf<UUserWidget> StealthLightLevelBarClass;

	UPROPERTY() TArray<float> StealthALIBuffer;
	UPROPERTY() float StealthLevel = 0.f;

	UPROPERTY(EditDefaultsOnly, Category = Laser)
		UParticleSystem* LaserBeamParticleSystem;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Awareness, meta = (AllowPrivateAccess = "true"))
		UPawnNoiseEmitterComponent* PawnNoiseEmitter;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Grab, meta = (AllowPrivateAccess = "true"))
		UPhysicsHandleComponent* PhysicsHandle;

	UPrimitiveComponent* enemyCorpse;

	FVector enemyCorpseLocation;

	//Aim light
	USpotLightComponent* aimSpotLight;

	float RUN_SPEED = 1200.f;
	float FIELD_OF_VIEW_AIM = 75.f;
	float FIELD_OF_VIEW_NONE = 90.f;

	UParticleSystemComponent* LaserBeamInstance;

		// Methode
public:
	AMyPlayer();

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	FVector GetCameraLocation();
	UFUNCTION(BlueprintCallable)
		void UpdateStealthLevel(float InStealthLevel);

	void Shoot() override;

protected:
	virtual void BeginPlay() override;

	void ComputeAimDirection() override;
	void OnDeath() override;

private:
	void MoveForward(float Value);
	void MoveRight(float Value);

	void ToggleCrouch();
	void GrabEnemy();
	void ReleaseEnemy();
	void UpdateLaserBeam();

	void PauseGame();

	void MakeSound(float loudness = 1.0f, float noiseLifeTime = 1.0f);
	virtual void StartAim() override;
	virtual void EndAim() override;

	void PlayerLose();
};
