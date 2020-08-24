#include "stdafx.h"

BlockPos BlockPos::Offset(BlockSide blockSide, int amount) {
	switch (blockSide) {
		case BlockSide::DOWN:	return { x, y - amount, z };
		case BlockSide::UP:		return { x, y + amount, z };
		case BlockSide::NORTH:	return { x, y, z - amount };
		case BlockSide::SOUTH:	return { x, y, z + amount };
		case BlockSide::WEST:	return { x - amount, y, z };
		case BlockSide::EAST:	return { x + amount, y, z };
	}
}