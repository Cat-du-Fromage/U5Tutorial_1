// Fill out your copyright notice in the Description page of Project Settings.


#include "BatteryMan.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h" //use to restart game

// Sets default values
//CAREFUL : ALL change made in editor by "Edit character" will override default values made here;
//If a value is changed in editor, it will display an arrow next to the variable saying "reset to default value"
ABatteryMan::ABatteryMan()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	GetCapsuleComponent() ->InitCapsuleSize(42.0f, 96.0f);

	JumpMaxHoldTime = 5.0f; //this is why stop jump is important
	
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false; //Yaw is the rotation around Z axis(Y rotation from Unity camera)
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f,540.0f,0.0f);
	GetCharacterMovement()->JumpZVelocity=600.0f;
	GetCharacterMovement()->AirControl = 0.2f;
	//Create the instance of the object USpringArmComponent(named : CameraBoom)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	//attahk the camera to the player
	CameraBoom->SetupAttachment(RootComponent);
	//How far away the camera is from the player
	CameraBoom->TargetArmLength=300.0f;
	CameraBoom->bUsePawnControlRotation = true;
	//Create the instance of the object UCameraComponent(named : FollowCamera)
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	//attack Camera(FollowCamera) to cameraBoom and take the previous setup above from cameraBoom and applay them to the Camera(named : FollowCamera)
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	//So camera do not rotate depending to the "arm" (see the arm like something like a raycast from player to the camera)
	FollowCamera->bUsePawnControlRotation = false;
	//Setting th Mesh Position relativ to the capsule component
	GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -GetCapsuleComponent()->GetScaledCapsuleHalfHeight()), false);
	bDead = false;
	Power = 100.0f;
	Power_Treshold = 3.0f;
}

// Called when the game starts or when spawned
//UFUNCTION need to be setup here
//Similare to Unity "On Start" function must be call here OR on OnUpdate (here "Tick")
void ABatteryMan::BeginPlay()
{
	Super::BeginPlay();
	//Collision event attach at the beginning (hit event)
	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &ABatteryMan::OnBeginOverlap);
	if(Player_Power_Widget_Class != nullptr)
	{
		Player_Power_Widget = CreateWidget(GetWorld(), Player_Power_Widget_Class);
		Player_Power_Widget -> AddToViewport();
	}
}

// Called every frame
void ABatteryMan::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	Power -= DeltaTime * Power_Treshold;

	if(Power <= 0)
	{
		if(!bDead)
		{
			bDead = true;

			GetMesh()->SetSimulatePhysics(true);
			GetMesh()->SetCollisionProfileName(FName("Ragdoll"));
			FTimerHandle UnusedHandle;
			GetWorldTimerManager().SetTimer(UnusedHandle, this, &ABatteryMan::RestartGame, 3.0f, false);
		}
	}
}

// Called to bind functionality to input
//Function related to Inputs must be called here
void ABatteryMan::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	//Turn from settings("input") 
	PlayerInputComponent->BindAxis("Turn", this,&APawn::AddControllerYawInput); //Z axis
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput); // X axis

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump );
	//Removing this will make your character "fly jump" until you release the space input (in this case spacebar)
	//JumpMaxHoldTime(inherit from Acharacter class) must be >0 to make a "fly jump"
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping );
	//Forward include both "forward":W and "backward":S
	PlayerInputComponent->BindAxis("MoveForward", this, &ABatteryMan::MoveForward);
	//Right include both "left":A and "right":D
	PlayerInputComponent->BindAxis("MoveRight", this, &ABatteryMan::MoveRight);
}

void ABatteryMan::MoveForward(float Axis)
{
	if(!bDead)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw,0);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Axis);
	}
}

void ABatteryMan::MoveRight(float Axis)
{
	if(!bDead)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw,0);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Axis);
	}
}

void ABatteryMan::RestartGame()
{
	UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
}

void ABatteryMan::OnBeginOverlap(class UPrimitiveComponent* HitComp,
								 class AActor* OtherActor,
								 class UPrimitiveComponent* OtherComp,
								 int32 OtherBodyIndex,
								 bool bFromSweep,
								 const FHitResult & SweepResult)
{
	if(OtherActor->ActorHasTag("Recharge"))
	{
		Power += 10.0f;
		Power = Power > 100.0f? 100.0f : Power;

		OtherActor->Destroy();
	}
}


