// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#define TILE_HEIGHT 100
#define TILE_WIDTH 100
#define MIN_ROOM_DIMENSION 4
#define MAX_ROOM_DIMENSION 9

#include <set>
#include <bitset>
#include <iostream>
#include <queue>
#include <vector>
#include <random>
#include <optional>
#include "Math/Axis.h"
#include "Math/Vector.h"
#include "Math/TransformNonVectorized.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelGateBase.generated.h"

struct myRoom {
	int x;
	int y;
	int doors;

	bool operator==(const myRoom& other) const {
		return x == other.x && y == other.y;
	}

	bool operator<(const myRoom& other) const {
		return x < other.x;
	}
};

enum Direction { NORTH = 0, EAST = 1, SOUTH = 2, WEST = 3 };

struct Point {
	int x, y;

	FString asString() const {
		return FString::Printf(TEXT("(%d, %d)"), x, y);
	}
};

struct Room {
	int width;		// how many tiles the room contains along X
	int height;		// how many tiles the room contains along Y
	int area;
	// const Room* parent;
	Point upperLeft;
	Point lowerRight;

	Room(Point upperLeft, int w, int h) :
		width(w), height(h), upperLeft(upperLeft), area(w * h) {
		lowerRight = Point{ upperLeft.x - ((width - 1) * TILE_WIDTH), upperLeft.y - ((height - 1) * TILE_HEIGHT) };
	}

	FString asString() const {
		return FString::Printf(TEXT("UL: %s | LR: %s | W, H: %d, %d"),
			*upperLeft.asString(), *lowerRight.asString(), width, height);
	}

	/*std::optional<Point> PickNonEdgePoint() const {
		if (width < 3 || height < 3) {
			return {};
		}
		else {
			int pointX = ALevelGateBase::GenerateRandomIntInRange(1, height - 1) * TILE_WIDTH;
			int pointY = ALevelGateBase::GenerateRandomIntInRange(1, height - 1) * TILE_HEIGHT;
			return Point{ upperLeft.x - pointX, lowerRight.y + pointY };
		}
	}*/
};

UCLASS()
class RETROFPSGAME_API ALevelGateBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALevelGateBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	int GenerateRandomRoomDimension();
	int RollDirection();
	std::vector<myRoom> GenerateRooms();
	//int GenerateDungeon(std::vector<myRoom>);
	void SpawnHallways(myRoom);

	// Up to date stuff
	std::pair<int, int> GenerateDungeon(int);
	std::vector<Room> GenerateAdjacentRooms(const Room&);
	bool CheckRoomOverlap(const Room&, const Room&);
	void SpawnRoom(const Room&);
	void SpawnTile(int, int);
	bool SpawnEnemyInRoom(const Room&);
	static int GenerateRandomIntInRange(int min, int max) {
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<int> distribution(min, max);

		return distribution(gen);
	}
	

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor>RoomToSpawn;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor>HallwayToSpawn;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor>FloorTile;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor>EnemyToSpawn;

	UFUNCTION(BlueprintCallable)
	int GenerateDungeon();
	
};

