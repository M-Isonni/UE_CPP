// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomBlueprintFunctionLibrary.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "DummySaveGame.h"
#include <Kismet/GameplayStatics.h>
#include "DummyInterface.h"


FVector UCustomBlueprintFunctionLibrary::GetActorWorldLocationOld(AActor* Actor)
{
	return Actor->GetActorLocation();
}


bool UCustomBlueprintFunctionLibrary::GetActorWorldLocation(AActor* Actor, FVector& Location)
{
	if (!Actor || !Actor->GetRootComponent())
	{
		return false;
	}

	Location = Actor->GetActorLocation();
	return true;
}

bool UCustomBlueprintFunctionLibrary::GetActorWorldLocationAndName(AActor* Actor, FVector& Location, FString& Name)
{
	if (!Actor || !Actor->GetRootComponent())
	{
		return false;
	}

	Location = Actor->GetActorLocation();
	Name = Actor->GetName();
	return true;
}

bool UCustomBlueprintFunctionLibrary::SetActorWorldLocation(AActor* Actor, FVector Location)
{
	return Actor->SetActorLocation(Location);
}


FVector UCustomBlueprintFunctionLibrary::GetActorScale3D(AActor* Actor)
{
	return Actor->GetActorScale();
}

bool UCustomBlueprintFunctionLibrary::SetActorScale3D(AActor* Actor, FVector Scale)
{
	if (!Actor || !Actor->GetRootComponent())
	{
		return false;
	}
	Actor->SetActorScale3D(Scale);
	return true;
}


FRotator UCustomBlueprintFunctionLibrary::GetActorWorldRotation(AActor* Actor)
{
	return Actor->GetActorRotation();
}

bool UCustomBlueprintFunctionLibrary::SetActorWorldRatation(AActor* Actor, FRotator Rotation)
{
	return Actor->SetActorRotation(Rotation);
}

bool UCustomBlueprintFunctionLibrary::CharacterJump(ACharacter* Character)
{
	if (!Character || !Character->GetRootComponent())
	{
		return false;
	}
	Character->Jump();

	return true;
}

bool UCustomBlueprintFunctionLibrary::CharacterDubleJump(ACharacter* Character, bool bCanDoubleJump ,bool& bOutCanDoubleJump ,float LaunchForce)
{
	if (!Character || !Character->GetRootComponent())
	{
		return false;
	}

	if (!Character->CanJump() && bCanDoubleJump) {


		FVector jumpvelocity(Character->GetActorUpVector() * LaunchForce);
		Character->LaunchCharacter(jumpvelocity, false, false);
		bOutCanDoubleJump = false;
	}
	else
	{
		Character->Jump();
		bOutCanDoubleJump = true;
	}


	return true;
}

bool UCustomBlueprintFunctionLibrary::SetDynamicMaterial(ACharacter* Character, UMaterial* Material,int32 MaterialIndex)
{
	if (Character)
	{
		UMaterialInstanceDynamic* DynamicMaterial = UMaterialInstanceDynamic::Create(Material, Character->GetMesh());

		if (DynamicMaterial)
		{
			if (Character->GetMesh()->GetMaterial(MaterialIndex))
			{
				Character->GetMesh()->SetMaterial(MaterialIndex, DynamicMaterial);
				return true;
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Material not found possible wrong index"));
			}
		}
	}


	return false;
}

bool UCustomBlueprintFunctionLibrary::ChangeCharacterColor(ACharacter* Character,FName ParameterName, FVector4 Color, int32 MaterialIndex)
{
	if (Character) 
	{
		UMaterialInstanceDynamic* DynamicMaterial = Cast<UMaterialInstanceDynamic> (Character->GetMesh()->GetMaterial(MaterialIndex));
		if (DynamicMaterial)
		{
			DynamicMaterial->SetVectorParameterValue(ParameterName, Color);
			return true;
		}
		else 
		{
			UE_LOG(LogTemp, Warning, TEXT("Dynamic material in null pointer material index worng or materail instance is not dynamic material instance"));
		}
	}

	return false;
}

bool UCustomBlueprintFunctionLibrary::ChangeCharacterTexture(ACharacter* Character, FName ParameterName, UTexture* Texture, int32 MaterialIndex)
{
	if (Character)
	{
		UMaterialInstanceDynamic* DynamicMaterial = Cast<UMaterialInstanceDynamic>(Character->GetMesh()->GetMaterial(MaterialIndex));
		if (DynamicMaterial)
		{
			DynamicMaterial->SetTextureParameterValue(ParameterName, Texture);
			return true;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Dynamic material in null pointer material index worng or materail instance is not dynamic material instance"));
		}
	}
	return false;
}

void UCustomBlueprintFunctionLibrary::BindInput(ACharacter* Character)
{
	UInputComponent* Input = Character->InputComponent;
	if (Input)
	{
		Input->BindAction(FName("Jump2"), EInputEvent::IE_Pressed, Character, &ACharacter::Jump);
	}
}

bool UCustomBlueprintFunctionLibrary::CallInterfaceFunction(UObject* Object)
{
	bool bResult = Object->GetClass()->ImplementsInterface(UDummyInterface::StaticClass());

	bResult = Object->Implements<UDummyInterface>();

	IDummyInterface* InterfaceCast = Cast<IDummyInterface>(Object);
	if (InterfaceCast)
	{
		InterfaceCast->DummyInterfaceFunction();
		InterfaceCast->Execute_DummyBluprintImplementable(Object); //Blueprint call
		InterfaceCast->DummyBlueprintNative_Implementation(); //CPP Implemetation
		InterfaceCast->Execute_DummyBlueprintNative(Object); //Blueprint Implentation
		return true;
	}

	return false;
}

bool UCustomBlueprintFunctionLibrary::CallDummyInterface(TScriptInterface<IDummyInterface> Interface)
{
	if (Interface)
	{
		Interface->DummyInterfaceFunction();
		return true;
	}
	return false;
}

bool UCustomBlueprintFunctionLibrary::LoadGame(AActor* Actor, FString SlotName, int32 UserIndex)
{
	UDummySaveGame* LoadGame = Cast<UDummySaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, UserIndex));
	if (LoadGame && Actor)
	{
		Actor->SetActorLocation(LoadGame->PlayerLocation);
		Actor->SetActorRotation(LoadGame->PlayerRotator);
		Actor->SetActorScale3D(LoadGame->PlayerScale);
		return true;
	}
	return false;
}

bool UCustomBlueprintFunctionLibrary::SaveGame(AActor* Actor, FString SlotName, int32 UserIndex)
{
	USaveGame* SaveGame=UGameplayStatics::CreateSaveGameObject(UDummySaveGame::StaticClass());
	if (SaveGame) 
	{
		UDummySaveGame* DummySaveGame = Cast<UDummySaveGame>(SaveGame);
		if (DummySaveGame && Actor)
		{
			DummySaveGame->PlayerLocation = Actor->GetActorLocation();
			DummySaveGame->PlayerRotator = Actor->GetActorRotation();
			DummySaveGame->PlayerScale = Actor->GetActorScale();

			UGameplayStatics::SaveGameToSlot(DummySaveGame, SlotName, UserIndex);
			return true;
		}
	}
	return false;
}
