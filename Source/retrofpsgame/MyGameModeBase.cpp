// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameModeBase.h"

int GenerateRandomIntInRange(int min, int max) {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<int> distribution(min, max);
	return distribution(gen);
}

int GenerateRandomRoomDimension() {
	return GenerateRandomIntInRange(MIN_ROOM_DIMENSION, MAX_ROOM_DIMENSION);
}
int RollHallwayLength() {
	return GenerateRandomIntInRange(3, 6);
}

int RollForHallway() {
	return GenerateRandomIntInRange(1, HALLWAY_CHANCE);
}

AMyGameModeBase::AMyGameModeBase() { }

void AMyGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) {
	Super::InitGame(MapName, Options, ErrorMessage);
	World = GetWorld();
	GenerateDungeon(30);
}

void AMyGameModeBase::GenerateDungeon(int depth) {
	std::shared_ptr<Room> startingRoom = std::make_shared<Room>(MyPoint(0, 0), 3, 3, MyPoint(-1, -1), "None");
	SpawnRoom(startingRoom);
	std::vector<std::shared_ptr<AreaBase>> allRooms;
	allRooms.push_back(std::move(startingRoom));

	int skipCount = 0;
	bool skipNext = false;

	// Generate rooms outward in a breadth-first-like manner
	for (int i = 0; i < depth; i++) {
		if (i < allRooms.size()) {              // don't go out of bounds in case room gen gets 

			// do not attempt to generate rooms from hallways
			// rooms following hallways will be generated immediately after the hallway is generated from a parent room
			if (allRooms[i]->getType() == "Hallway") continue;

			std::vector<std::shared_ptr<AreaBase>> newRooms = GenerateAdjacentRooms(allRooms[i]);

			for (std::shared_ptr<AreaBase>& nextArea : newRooms) {                // for each generated room
				if (skipNext) {                             // skipNext is true if a hallway was skipped in the previous iteration.
					skipNext = false;
					continue;
				}
				bool noOverlaps = true;
				for (const std::shared_ptr<AreaBase>& otherRoom : allRooms) {     // check it against every other existing room
					if (CheckRoomOverlap(*nextArea, *otherRoom)) {      // for overlap; if there is an overlap,
						noOverlaps = false;                     // skip spawning this new room.
						skipCount++;

						if (nextArea->getType() == "Hallway") {                     // if this room is a hallway, it will have a room attached to it that's next in newRooms.
							skipNext = true;                    // set skipNext to true to skip the connected room as well.
							skipCount++;
						}
						// GEngine->AddOnScreenDebugMessage(-1, 1000.0f, FColor::Yellow, FString::Printf(TEXT("Skipped ")) + nextArea->asString());
						break;
					}
				}

				if (noOverlaps) {
					SpawnRoom(nextArea);                              // else, spawn it and append it to allRooms
					allRooms.push_back(std::move(nextArea));
					skipNext = false;
				}
			}
		}
	}
}

// Generate new rooms with the given room as the source
std::vector<std::shared_ptr<AreaBase>> AMyGameModeBase::GenerateAdjacentRooms(const std::shared_ptr<AreaBase>& parent) const {
	std::vector<std::shared_ptr<AreaBase>> newRooms;
	std::vector<std::shared_ptr<AreaBase>> nr;
    
    nr = GenerateAdjacentRoom(*parent, Direction::NORTH);
	for (const std::shared_ptr<AreaBase>& room : nr) {
		newRooms.push_back(std::move(room));
	}
    nr = GenerateAdjacentRoom(*parent, Direction::EAST);
	for (const std::shared_ptr<AreaBase>& room : nr) {
		newRooms.push_back(std::move(room));
	}

    nr = GenerateAdjacentRoom(*parent, Direction::SOUTH);
	for (const std::shared_ptr<AreaBase>& room : nr) {
		newRooms.push_back(std::move(room));
	}
    nr = GenerateAdjacentRoom(*parent, Direction::WEST);
	for (const std::shared_ptr<AreaBase>& room : nr) {
		newRooms.push_back(std::move(room));
	}
    
    return newRooms;
}

std::vector<std::shared_ptr<AreaBase>> AMyGameModeBase::GenerateAdjacentRoom(const AreaBase& parent, Direction dir) const {
    bool isHallway = false;
	int newWidth, newHeight;
	int connectionPoint;
	int newUpperLeftX, newUpperLeftY;
	std::vector<std::shared_ptr<AreaBase>> generatedRooms;

	if (parent.getType() == "Room") {
		isHallway = (RollForHallway() == 1);

		if (!isHallway) {
			newWidth = GenerateRandomRoomDimension();
			newHeight = GenerateRandomRoomDimension();
			if (dir == Direction::NORTH) {
				connectionPoint = GenerateRandomIntInRange(1 - parent.getWidth(), newWidth - 1);
				newUpperLeftX = parent.getUpperLeft().x + (connectionPoint * TILE_WIDTH);
				newUpperLeftY = parent.getUpperLeft().y + (newHeight * TILE_HEIGHT);
			}
			else if (dir == Direction::EAST) {
				connectionPoint = GenerateRandomIntInRange(0, parent.getHeight() + newHeight - 2);
				newUpperLeftX = parent.getLowerRight().x - TILE_WIDTH;
				newUpperLeftY = parent.getLowerRight().y + (connectionPoint * TILE_HEIGHT);
			}
			else if (dir == Direction::SOUTH) {
				connectionPoint = GenerateRandomIntInRange(1 - parent.getWidth(), newWidth - 1);
				newUpperLeftX = parent.getUpperLeft().x + (connectionPoint * TILE_WIDTH);
				newUpperLeftY = parent.getLowerRight().y - TILE_HEIGHT;
			}
			else if (dir == Direction::WEST) {
				connectionPoint = GenerateRandomIntInRange(0, parent.getHeight() + newHeight - 2);
				newUpperLeftX = parent.getUpperLeft().x + (newWidth * TILE_WIDTH);
				newUpperLeftY = parent.getLowerRight().y + (connectionPoint * TILE_HEIGHT);
			}

			std::shared_ptr<Room> newRoom = std::make_shared<Room>(MyPoint(newUpperLeftX, newUpperLeftY), newWidth, newHeight, parent.getUpperLeft(), parent.getType());
			generatedRooms.push_back(std::move(newRoom));
		}
		// Rules for generating Hallway type
		else {
			if (dir == Direction::ERROR) {
				throw std::invalid_argument("direction not given for hallway generation");
			}

			if (dir == Direction::NORTH) {
				newWidth = HALLWAY_THINNESS;
				newHeight = RollHallwayLength();
				connectionPoint = GenerateRandomIntInRange(HALLWAY_THINNESS - parent.getWidth(), 0);
				newUpperLeftX = parent.getUpperLeft().x + (connectionPoint * TILE_WIDTH);
				newUpperLeftY = parent.getUpperLeft().y + (newHeight * TILE_HEIGHT);
			}
			else if (dir == Direction::EAST) {
				newWidth = RollHallwayLength();
				newHeight = HALLWAY_THINNESS;
				connectionPoint = GenerateRandomIntInRange(HALLWAY_THINNESS - parent.getHeight(), 0);
				newUpperLeftX = parent.getLowerRight().x - TILE_WIDTH;
				newUpperLeftY = parent.getUpperLeft().y + (connectionPoint * TILE_HEIGHT);
			}
			else if (dir == Direction::SOUTH) {
				newWidth = HALLWAY_THINNESS;
				newHeight = RollHallwayLength();
				connectionPoint = GenerateRandomIntInRange(HALLWAY_THINNESS - parent.getWidth(), 0);
				newUpperLeftX = parent.getUpperLeft().x + (connectionPoint * TILE_WIDTH);
				newUpperLeftY = parent.getLowerRight().y - TILE_HEIGHT;
			}
			else if (dir == Direction::WEST) {
				newWidth = RollHallwayLength();
				newHeight = HALLWAY_THINNESS;
				connectionPoint = GenerateRandomIntInRange(HALLWAY_THINNESS - parent.getHeight(), 0);
				newUpperLeftX = parent.getUpperLeft().x + (newWidth * TILE_WIDTH);
				newUpperLeftY = parent.getUpperLeft().y + (connectionPoint * TILE_HEIGHT);
			}
			Hallway newHallway(MyPoint(newUpperLeftX, newUpperLeftY), newWidth, newHeight, dir, parent.getUpperLeft(), parent.getType());
			std::shared_ptr<AreaBase> followingRoom = GenerateAdjacentRoom(newHallway, dir)[0];
			std::shared_ptr<Hallway> newHallwayPtr = std::make_shared<Hallway>(newHallway);
			generatedRooms.push_back(std::move(newHallwayPtr));
			generatedRooms.push_back(std::move(followingRoom));
		}
	}
	// This branch will only be taken if this function was called by itself after generating a hallway from a room
	else if (parent.getType() == "Hallway") {
		newWidth = GenerateRandomRoomDimension();
		newHeight = GenerateRandomRoomDimension();
		if (dir == Direction::NORTH) {
			connectionPoint = GenerateRandomIntInRange(0, newWidth - HALLWAY_THINNESS);
			newUpperLeftX = parent.getUpperLeft().x + (connectionPoint * TILE_WIDTH);
			newUpperLeftY = parent.getUpperLeft().y + (newHeight * TILE_HEIGHT);
		}
		else if (dir == Direction::EAST) {
			connectionPoint = GenerateRandomIntInRange(0, newHeight - HALLWAY_THINNESS);
			newUpperLeftX = parent.getLowerRight().x - TILE_WIDTH;
			newUpperLeftY = parent.getUpperLeft().y + (connectionPoint * TILE_HEIGHT);
		}
		else if (dir == Direction::SOUTH) {
			connectionPoint = GenerateRandomIntInRange(0, newWidth - HALLWAY_THINNESS);
			newUpperLeftX = parent.getUpperLeft().x + (connectionPoint * TILE_WIDTH);
			newUpperLeftY = parent.getLowerRight().y - TILE_HEIGHT;
		}
		else if (dir == Direction::WEST) {
			connectionPoint = GenerateRandomIntInRange(0, newHeight - HALLWAY_THINNESS);
			newUpperLeftX = parent.getUpperLeft().x + (newWidth * TILE_WIDTH);
			newUpperLeftY = parent.getUpperLeft().y + (connectionPoint * TILE_HEIGHT);
		}

		std::shared_ptr<Room> followingRoom = std::make_shared<Room>(MyPoint(newUpperLeftX, newUpperLeftY), newWidth, newHeight, parent.getUpperLeft(), parent.getType());
		generatedRooms.push_back(std::move(followingRoom));
	}
	return generatedRooms;
}
// Spawns a tile into the world at a given location
void AMyGameModeBase::SpawnTile(int tileX, int tileY) const {
	FVector TileLocation(tileX, tileY, 0);
	FTransform TileTransform(TileLocation);
	World->SpawnActor<AActor>(FloorTile, TileTransform);
}

// Spawns a room into the world
void AMyGameModeBase::SpawnRoom(const std::shared_ptr<AreaBase>& area) const {
	// int tileX, tileY;

	for (int x = area->getLowerRight().x; x <= area->getUpperLeft().x; x += TILE_WIDTH) {
		for (int y = area->getUpperLeft().y; y >= area->getLowerRight().y; y -= TILE_HEIGHT) {
			SpawnTile(x, y);
		}
	}

	if (area->getHeight() >= 10 && area->getWidth() >= 10) {
		int structureX = area->getUpperLeft().x - (area->getUpperLeft().x - area->getLowerRight().x) / 2;
		int structureY = area->getLowerRight().y + (area->getUpperLeft().y - area->getLowerRight().y) / 2;
		FTransform StructureTransform(FVector(structureX, structureY, 0));
		World->SpawnActor<AActor>(TestStructure, StructureTransform);
	}
	// GEngine->AddOnScreenDebugMessage(-1, 1000.0f, FColor::Yellow, area->asString());
	// SpawnEnemyInRoom(room);

}

// Returns true if two rooms overlap, false if not
bool AMyGameModeBase::CheckRoomOverlap(const AreaBase& area1, const AreaBase& area2) const {
	if (area1.getUpperLeft().x < area2.getLowerRight().x || area2.getUpperLeft().x < area1.getLowerRight().x) {
		return false;
	}
	if (area1.getUpperLeft().y < area2.getLowerRight().y || area2.getUpperLeft().y < area1.getLowerRight().y) {
		return false;
	}
	return true;
}

//bool AMyGameModeBase::SpawnEnemyInRoom(const Room& room) {
//    std::optional<Point> nonEdgePoint = room.PickNonEdgePoint();
//    if (nonEdgePoint.has_value()) {
//        Point point = nonEdgePoint.value();
//        FVector EnemyLocation(point.x, point.y, 100);
//        FTransform EnemyTransform(EnemyLocation);
//        GetWorld()->SpawnActor<AActor>(EnemyToSpawn, EnemyTransform);
//        // GEngine->AddOnScreenDebugMessage(-1, 1000.0f, FColor::Yellow, point.asString() + room.asString());
//        return true;
//    }       
//    return false;
//}