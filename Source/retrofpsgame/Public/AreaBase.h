// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <string>
#include "CoreMinimal.h"
#include "MyPoint.h"

/**
 * 
 */
class RETROFPSGAME_API AreaBase
{
public:
	AreaBase(MyPoint upperLeft, int w, int h, MyPoint parentUL, std::string pType) :
		width(w), height(h), upperLeft(upperLeft), parentUpperLeft(parentUL), parentType(pType) {
		lowerRight = MyPoint{ upperLeft.x - ((width - 1) * TILE_WIDTH), upperLeft.y - ((height - 1) * TILE_HEIGHT) };
	}

	virtual ~AreaBase() {}

	virtual FString asString() const {
		return FString::Printf(TEXT("%s UL: %s | LR: %s | W, H: %d, %d *** Parent %s UL: %s"),
			*FString(type.c_str()), *upperLeft.asString(), *lowerRight.asString(), width, height, *FString(parentType.c_str()), *parentUpperLeft.asString());
	}

	// Getters
	MyPoint getUpperLeft() const {
		return upperLeft;
	}

	MyPoint getLowerRight() const {
		return lowerRight;
	}

	int getWidth() const {
		return width;
	}

	int getHeight() const {
		return height;
	}

	virtual std::string getType() const {
		return type;
	}


protected:
	int width;		// how many tiles the room contains along X
	int height;		// how many tiles the room contains along Y

	MyPoint upperLeft;
	MyPoint lowerRight;
	MyPoint parentUpperLeft;
	std::string parentType;
	std::string type = "AreaBase";
};
