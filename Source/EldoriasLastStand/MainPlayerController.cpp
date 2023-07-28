// Fill out your copyright notice in the Description page of Project Settings.


#include "MainPlayerController.h"

AMainPlayerController::AMainPlayerController()
{
	TeamId = FGenericTeamId(5);
}


FGenericTeamId AMainPlayerController::GetGenericTeamId() const
{
	return TeamId;
}
