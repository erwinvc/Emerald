#pragma once
#define BIT(n)	(1<<(n))

enum class ClearMaskFlags {
	COLOR = BIT(0),
	DEPTH = BIT(1),
	STENCIL = BIT(2)
};