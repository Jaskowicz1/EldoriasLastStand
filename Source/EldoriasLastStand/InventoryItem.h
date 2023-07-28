// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "InventoryItem.generated.h"

USTRUCT(BlueprintType)
struct FInventoryItemStruct
{
	GENERATED_BODY()

	FInventoryItemStruct()
	{
		Name = "Enter name here";
		Description = "Enter description here.";
		Usage = "Enter usage here.";
		ItemImage = nullptr;
		AmountOfItem = 1;
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory")
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory")
	FString Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory")
	FString Usage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory")
	UTexture2D* ItemImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory")
	FGameplayTag TypeOfItem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory")
	int AmountOfItem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory")
	FGameplayTag ItemAffectsStat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory")
	int AffectAmount;

	bool IsValid()
	{
		return (Name != "Enter name here" && !Name.IsEmpty())
			&& (Description != "Enter description here." && !Description.IsEmpty())
			&& ItemImage != nullptr;
	}

	void Invalidate()
	{
		Name ="Enter name here";
		Description = "Enter description here.";
		ItemImage = nullptr;
		AmountOfItem = 0;
	}

	friend bool operator ==(const FInventoryItemStruct& x, const FInventoryItemStruct& y)
	{
		return (x.Name == y.Name) && (x.TypeOfItem == y.TypeOfItem);
	}

	friend bool operator !=(const FInventoryItemStruct& x, const FInventoryItemStruct& y)
	{
		return (x.Name != y.Name) || (x.TypeOfItem != y.TypeOfItem);
	}
};

// Possibly not needed anymore, but will keep.
inline uint32 GetTypeHash(const FInventoryItemStruct& Value)
{
	return FCrc::MemCrc32(&Value, sizeof(FInventoryItemStruct));
}