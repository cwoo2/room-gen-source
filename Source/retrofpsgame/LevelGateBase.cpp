// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelGateBase.h"

// Sets default values
ALevelGateBase::ALevelGateBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ALevelGateBase::BeginPlay()
{
	Super::BeginPlay();
    std::srand(time(0));
    //std::vector<myRoom> Rooms = GenerateRooms();
    /*int NumRoomsSpawned = GenerateDungeon();*/

    std::pair<int, int> res = GenerateDungeon(20);
    GEngine->AddOnScreenDebugMessage(-1, 1000.0f, FColor::Yellow, FString::Printf(TEXT("Gen: %d, Skipped: %d"), res.first, res.second));
}

// Called every frame
void ALevelGateBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

int ALevelGateBase::GenerateRandomRoomDimension() {
    return GenerateRandomIntInRange(MIN_ROOM_DIMENSION, MAX_ROOM_DIMENSION);
}

int ALevelGateBase::RollDirection() {
    int res = 0;

    for (int i = 0; i < 4; i++) {
        if (GenerateRandomIntInRange(1, 4) == 1) {
            switch (i) {
            case 0:  // N door open
                res |= 0b1000;
                break;
            case 1:  // E door open
                res |= 0b0100;
                break;
            case 2:  // S door open
                res |= 0b0010;
                break;
            case 3:  // W door open
                res |= 0b0001;
                break;
            }
        }
    }
    return res;
}

void printRoom(myRoom r) {
    if (GEngine)
        GEngine->AddOnScreenDebugMessage(-1, 1000.0f, FColor::Yellow, FString::Printf(TEXT("%d %d %d"), r.x, r.y, r.doors));
};

std::vector<myRoom> ALevelGateBase::GenerateRooms() {
    myRoom startingRoom{ 0, 0, 0b1111 };
    std::vector<myRoom> visited;
    std::set<std::pair<int, int>> occupiedSpaces;
    std::queue<myRoom> queue;
    queue.push(startingRoom);

    while (!queue.empty()) {
        myRoom currRoom = queue.front();
        queue.pop();
        std::pair<int, int> currPos(currRoom.x, currRoom.y);
        if (occupiedSpaces.find(currPos) == occupiedSpaces.end()) {
            int doors = currRoom.doors;
            int newX, newY, newDoors;

            // Check each bit to see if door is open
            if (doors & 0b1000) {
                newX = currRoom.x;
                newY = currRoom.y + 1;
                newDoors = RollDirection() | 0b0010;  // connect currRoom's N door to Room's S door

                myRoom Room{ newX, newY, newDoors };

                queue.push(Room);
            }
            if (doors & 0b0100) {
                newX = currRoom.x + 1;
                newY = currRoom.y;
                newDoors = RollDirection() | 0b0001;

                myRoom Room{ newX, newY, newDoors };

                queue.push(Room);
            }
            if (doors & 0b0010) {
                newX = currRoom.x;
                newY = currRoom.y - 1;
                newDoors = RollDirection() | 0b1000;

                myRoom Room{ newX, newY, newDoors };

                queue.push(Room);
            }
            if (doors & 0b0001) {
                newX = currRoom.x - 1;
                newY = currRoom.y;
                newDoors = RollDirection() | 0b0100;

                myRoom Room{ newX, newY, newDoors };

                queue.push(Room);
            }
            visited.push_back(currRoom);
            occupiedSpaces.insert(currPos);
            //printRoom(currRoom);
        }
    }
    return visited;
}

int ALevelGateBase::GenerateDungeon() {
    std::vector<myRoom> Rooms = GenerateRooms();
    int NumRoomsSpawned = 0;

    for (std::vector<myRoom>::iterator it = Rooms.begin(); it != Rooms.end(); ++it) {
        myRoom room = *it;
        if (!(room.x == 0 && room.y == 0)) {
            FVector RoomLocation(room.x * 1500, room.y * 1500, 0);
            FTransform RoomTransform(RoomLocation);
            GetWorld()->SpawnActor<AActor>(RoomToSpawn, RoomTransform);
            NumRoomsSpawned++;
            
            /*if (GEngine) {
                GEngine->AddOnScreenDebugMessage(-1, 1000.0f, FColor::Yellow, FString::Printf(TEXT("Spawned %d %d DOORS: %d"), room.x, room.y, room.doors));
            }*/

            SpawnHallways(room);
        }
    }

    return NumRoomsSpawned;
}

void ALevelGateBase::SpawnHallways(myRoom room) {
    if (room.doors & 0b1000) {
        FVector HallwayLocation(room.x * 1500, (room.y * 1500) + 750, 0);
        FRotator HallwayRotation(0, 90, 0);     // constructor is Pitch, Yaw, Roll
        FVector HallwayScale(1.0);

        FTransform HallwayTransform(HallwayRotation, HallwayLocation, HallwayScale);
        GetWorld()->SpawnActor<AActor>(HallwayToSpawn, HallwayTransform);
        
    }
    if (room.doors & 0b0100) {
        FVector HallwayLocation((room.x * 1500) + 750, room.y * 1500, 0);
        FTransform HallwayTransform(HallwayLocation);
        GetWorld()->SpawnActor<AActor>(HallwayToSpawn, HallwayTransform);
    }
    if (room.doors & 0b0010) {
        FVector HallwayLocation(room.x * 1500, (room.y * 1500) - 750, 0);
        FRotator HallwayRotation(0, 90, 0);     // constructor is Pitch, Yaw, Roll
        FVector HallwayScale(1.0);

        FTransform HallwayTransform(HallwayRotation, HallwayLocation, HallwayScale);
        GetWorld()->SpawnActor<AActor>(HallwayToSpawn, HallwayTransform);
    }
    if (room.doors & 0b0001) {
        FVector HallwayLocation((room.x * 1500) - 750, room.y * 1500, 0);
        FTransform HallwayTransform(HallwayLocation);
        GetWorld()->SpawnActor<AActor>(HallwayToSpawn, HallwayTransform);
    }
}

std::pair<int, int> ALevelGateBase::GenerateDungeon(int depth) {
    Room startingRoom(Point{ 0, 0 }, 3, 3);
    SpawnRoom(startingRoom);
    std::vector<Room> allRooms = { startingRoom };
    int skipCount = 0;
    
    for (int i = 0; i < depth; i++) {
        if (i < allRooms.size()) {              // don't go out of bounds in case room gen gets stuck
            // Generate rooms outward in a breadth-first-like manner
            std::vector<Room> newRooms = GenerateAdjacentRooms(allRooms[i]);

            for (const Room& nr : newRooms) {                // for each generated room
                bool noOverlaps = true;
                for (const Room& otherRoom : allRooms) {     // check it against every other existing room
                    if (CheckRoomOverlap(nr, otherRoom)) {      // for overlap; if there is an overlap,
                        noOverlaps = false;                     // skip spawning this new room.
                        skipCount++;
                        //GEngine->AddOnScreenDebugMessage(-1, 1000.0f, FColor::Yellow, FString::Printf(TEXT("Skipped ")) + nr.asString());
                        break;
                    }
                }
                if (noOverlaps) {
                    SpawnRoom(nr);                              // else, spawn it and append it to allRooms
                    allRooms.push_back(nr);
                }
            }
        }
    }
    return std::pair<int, int>(allRooms.size(), skipCount);
}

// Spawns a tile into the world at a given location
void ALevelGateBase::SpawnTile(int tileX, int tileY) {
    FVector TileLocation(tileX, tileY, 0);
    FTransform TileTransform(TileLocation);
    GetWorld()->SpawnActor<AActor>(FloorTile, TileTransform);
}

// Spawns a room into the world
void ALevelGateBase::SpawnRoom(const Room& room) {
    // int tileX, tileY;

    for (int x = room.lowerRight.x; x <= room.upperLeft.x; x += TILE_WIDTH) {
        for (int y = room.upperLeft.y; y >= room.lowerRight.y; y -= TILE_HEIGHT) {
            SpawnTile(x, y);
        }
    }
    //GEngine->AddOnScreenDebugMessage(-1, 1000.0f, FColor::Yellow, room.asString());
    //SpawnEnemyInRoom(room);
    
}

// Generate new rooms with the given room as the source
std::vector<Room> ALevelGateBase::GenerateAdjacentRooms(const Room& room) {
    std::vector<Room> newRooms;
    int rand;
    int newOriginX;
    int newOriginY;
    for (int i = 0; i < 4; i++) {
        int newWidth = GenerateRandomRoomDimension();
        int newHeight = GenerateRandomRoomDimension();
        switch (i) {
        case NORTH:     // Generate new room from the north edge
            rand = GenerateRandomIntInRange((room.width - 1) * -1, newWidth - 1);
            newOriginX = room.upperLeft.x + (rand * TILE_WIDTH);
            newOriginY = room.upperLeft.y + (newHeight * TILE_HEIGHT);
            break;
        case EAST:      // Generate new room from the east edge
            rand = GenerateRandomIntInRange(0, room.height + newHeight - 2);
            newOriginX = room.lowerRight.x - TILE_WIDTH;
            newOriginY = room.lowerRight.y + (rand * TILE_HEIGHT);
            break;
        case SOUTH:     // Generate new room from the south edge
            rand = GenerateRandomIntInRange(0, newWidth + room.width - 2);
            // GEngine->AddOnScreenDebugMessage(-1, 1000.0f, FColor::Yellow, FString::Printf(TEXT("South rand: %d "), rand));
            newOriginX = room.lowerRight.x + (rand * TILE_WIDTH);
            newOriginY = room.lowerRight.y - TILE_HEIGHT;
            break;
        case WEST:      // Generate new room from the west edge
            rand = GenerateRandomIntInRange(0, room.height + newHeight - 2);
            newOriginX = room.upperLeft.x + (newWidth * TILE_WIDTH);
            newOriginY = room.lowerRight.y + (rand * TILE_HEIGHT);
            break;
        }
        Room r{ Point{ newOriginX, newOriginY }, newWidth, newHeight };
        //GEngine->AddOnScreenDebugMessage(-1, 1000.0f, FColor::Yellow, FString::Printf(TEXT("Generated ")) + r.asString());
        newRooms.push_back(r);
    }
    return newRooms;
}

// Returns true if two rooms overlap, false if not
bool ALevelGateBase::CheckRoomOverlap(const Room& room1, const Room& room2) {

    Point l1 = room1.upperLeft;
    Point r1 = room1.lowerRight;
    Point l2 = room2.upperLeft;
    Point r2 = room2.lowerRight;

    // if rectangle has area 0, no overlap
    if (l1.x == r1.x || l1.y == r1.y || r2.x == l2.x || l2.y == r2.y)
        return false;

    // If one rectangle is on left side of other
    if (l1.x < r2.x || l2.x < r1.x)
        return false;

    // If one rectangle is above other
    if (r1.y > l2.y || r2.y > l1.y)
        return false;

    return true;
}

//bool ALevelGateBase::SpawnEnemyInRoom(const Room& room) {
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