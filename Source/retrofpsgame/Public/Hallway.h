// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <string>
#include "CoreMinimal.h"
#include "AreaBase.h"

/**
 * 
 */
class RETROFPSGAME_API Hallway : public AreaBase
{
public:
	Hallway(MyPoint upperLeft, int w, int h, Direction d, MyPoint parentUL, std::string pType) : AreaBase(upperLeft, w, h, parentUL, pType) {
		direction = d;
	};

	FString asString() const override {
		return FString::Printf(TEXT("%s UL: %s | LR: %s | W, H: %d, %d *** Parent %s UL: %s"),
			*FString(type.c_str()), *upperLeft.asString(), *lowerRight.asString(), width, height, *FString(parentType.c_str()), *parentUpperLeft.asString());
	}

	std::string getType() const override {
		return type;
	}

protected:
	Direction direction;
	std::string type = "Hallway";
};
