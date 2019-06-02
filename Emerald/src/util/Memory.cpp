#include "stdafx.h"

void Memory::DontDoThat() {
	LOG_ERROR("Don't do that!");
}

void* Memory::Allocate(size_t size) {
	GetMemory()->m_totalAllocated += size;
	GetMemory()->m_currentUsed += size;
	GetMemory()->m_totalAllocations++;

	size_t actualSize = size + sizeof(size_t);
	byte* result = (byte*)_aligned_malloc(actualSize, 16);
	memset(result, 0, actualSize);
	memcpy(result, &size, sizeof(size_t));
	result += sizeof(size_t);
	return result;

}

void* Memory::AllocateDebug(size_t size, String name) {
	ASSERT(size < 1024 * 1024 * 1024, "");

	vector<String> split = Utils::Split(name, " ");
	if (split.size() < 2 || split[0] != "class") {
		LOG_ERROR("Wtf is %s?", name);
		return nullptr;
	}

	AllocationCounter& counter = GetMemory()->m_allocations[split[1]];
	counter.m_allocations++;
	counter.m_totalAllocations++;
	return Allocate(size);
}

void Memory::Free(void* block) {
	if (block == nullptr) return;
	byte* memory = ((byte*)block) - sizeof(size_t);
	size_t size = *(size_t*)memory;
	GetMemory()->m_totalFreed += size;
	GetMemory()->m_currentUsed -= size;
	_aligned_free(memory);
}

void Memory::FreeDebug(void* block, String name) {
	vector<String> split = Utils::Split(name, " ");
	if (split.size() < 2 || split[0] != "class") {
		LOG_ERROR("Wtf is %s?", name);
		return;
	}

	AllocationCounter& counter = GetMemory()->m_allocations[split[1]];
	ASSERT(block, "");
	counter.m_freed++;
	counter.m_allocations--;
	Free(block);

	//yep.Just look at vector method "emplace" - how do they call ctor
}

SystemMemoryInfo Memory::GetSystemInfo() {
	MEMORYSTATUSEX status;
	status.dwLength = sizeof(MEMORYSTATUSEX);
	GlobalMemoryStatusEx(&status);

	SystemMemoryInfo result =
	{
		(int64)status.ullAvailPhys,
		(int64)status.ullTotalPhys,
		(int64)status.ullAvailVirtual,
		(int64)status.ullTotalVirtual
	};
	return result;
}

void Memory::CheckAllocations() {
	bool found = false;
	for (auto allocation : m_allocations) {
		if (allocation.second.m_allocations > 0) {
			found = true;
			LOG("%s has %d references that have not been deallocated", allocation.first.c_str(), allocation.second.m_allocations);
		}
	}

	#ifdef MEMORY_CHECK
	if (found) LOG_ERROR("Memory leaks detected");
	#endif
}
