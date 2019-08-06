#include "stdafx.h"

Tween::~Tween() {
	GetTweenManager()->Delete(this);
}