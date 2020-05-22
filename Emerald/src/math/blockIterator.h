#pragma once

//----------------------------------------------------------------------------
// Copyright (c) 2014 Jason Gedge
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//----------------------------------------------------------------------------
//https://github.com/thegedge/voxel_iterator

class BlockIterator {
public:
	BlockIterator();
	BlockIterator(const glm::vec3& pos, const glm::vec3& dir);

	BlockIterator(const BlockIterator&) = default;
	BlockIterator(BlockIterator&&) = default;
	BlockIterator& operator=(const BlockIterator&) = default;

	// Post-increment
	BlockIterator operator++();

	// Dereferencing
	glm::ivec3 operator*();
	const glm::ivec3 operator*() const;

	/// @return the current t value (distance to initial point)
	double t() const { return currentT; }

private:
	const glm::vec3 size; //BlockSize
	const glm::vec3 dir;
	const glm::vec3 step; // The integral steps in each direction when we iterate (either 1 or -1)
	const glm::vec3 tDelta; // The t value which moves us from one voxel to the next
	glm::vec3 tNext; // The t value which will get us to the next voxel
	glm::vec3 voxel; // The integer indices for the current voxel
	double currentT; // The t value representing the current point
};
