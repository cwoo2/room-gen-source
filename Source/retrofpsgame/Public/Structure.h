// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AreaBase.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Structure.generated.h"

UCLASS()
class RETROFPSGAME_API AStructure : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AStructure();

	// getters
	int getWidth() const { return width; }
	int getHeight() const { return height; }
	std::shared_ptr<AreaBase> getParentArea() const { return parentArea; }

	// setters
	void setParentArea(AreaBase& area) { parentArea = std::make_shared<AreaBase>(area); }

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor>StructureToSpawn;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	int width, height; // actual width and height of the room, not in tiles
	std::shared_ptr<AreaBase> parentArea;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
