// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#define TILE_WIDTH 300
#define TILE_HEIGHT 300
#define HALLWAY_THINNESS 2
#define MIN_ROOM_DIMENSION 4
#define MAX_ROOM_DIMENSION 12
#define HALLWAY_CHANCE 5	// probability of a room being a hallway = 1/HALLWAY_CHANCE

#include <random>
#include "CoreMinimal.h"

/**
 * 
 */
class RETROFPSGAME_API MyPoint
{
public:
	MyPoint(int x, int y) : x(x), y(y) {}
	MyPoint() : x(0), y(0) {}

	int x, y;

	FString asString() const {
		return FString::Printf(TEXT("(%d, %d)"), x, y);
	}
};

enum class Direction { NORTH, EAST, SOUTH, WEST, ERROR };