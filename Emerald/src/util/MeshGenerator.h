#pragma once

namespace MeshGenerator {

	/*Create a sphere with x amount of stacks and slices*/
	Mesh* Sphere(int stacks = 20, int slices = 20);

	/*Create a simple quad*/
	Mesh* Quad();
}