// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameMode.h"
#include "CharacterPawn.h"

AMyGameMode::AMyGameMode()
{
	DefaultPawnClass = ACharacterPawn::StaticClass();
}
