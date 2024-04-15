// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Hallway.h"
#include "Room.h"
#include "Math/Vector.h"
#include "Math/TransformNonVectorized.h"
#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MyGameModeBase.generated.h"

UCLASS()
class RETROFPSGAME_API AMyGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMyGameModeBase();
	std::vector<std::shared_ptr<AreaBase>> GenerateAdjacentRooms(const std::shared_ptr<AreaBase>&) const;
	bool CheckRoomOverlap(const AreaBase&, const AreaBase&) const;
	void SpawnRoom(const std::shared_ptr<AreaBase>&) const;
	void SpawnTile(int, int) const;
	// bool SpawnEnemyInRoom(const Room&);

	std::vector<std::shared_ptr<AreaBase>> GenerateAdjacentRoom(const AreaBase&, Direction) const;

	UFUNCTION(BlueprintCallable)
	void GenerateDungeon(int depth);

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor>FloorTile;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor>EnemyToSpawn;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor>TestStructure;

protected:
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	UWorld* World;
};



