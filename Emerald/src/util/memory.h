#pragma once

struct SystemMemoryInfo {
	int64 m_availablePhysicalMemory;
	int64 m_totalPhysicalMemory;
	int64 m_availableVirtualMemory;
	int64 m_totalVirtualMemory;
};

class Memory : public Singleton<Memory> {
private:

	struct AllocationCounter {
		int64 m_totalAllocations;
		int64 m_allocations;
		int64 m_freed;
	};

	unordered_map<String, AllocationCounter> m_allocations;
	int64 m_totalAllocated = 0;
	int64 m_totalFreed = 0;
	int64 m_currentUsed = 0;
	int64 m_totalAllocations = 0;

	Memory() {}
	~Memory() {}
	friend Singleton;

public:
	static void DontDoThat();
	static void* Allocate(size_t size);
	static void* AllocateDebug(size_t size, String name);
	static void Free(void* block);
	static void FreeDebug(void* block, String name);

	SystemMemoryInfo GetSystemInfo();
	void CheckAllocations();

	//bool compareAllocations(AllocationCounter& first, AllocationCounter& second) {
	//	return first.m_totalAllocations < second.m_totalAllocations;
	//}

	void OnImGui() {
		ImGui::BeginTabBar("tab");
		ImGui::DragInt64("Total allocated", &m_totalAllocated, 0);
		ImGui::DragInt64("Total freed", &m_totalFreed, 0);
		ImGui::DragInt64("Currently used", &m_currentUsed, 0);
		ImGui::DragInt64("Total allocations", &m_totalAllocations, 0);

		ImGui::EndTabBar();

		ImGui::Columns(4, "Allocations");
		ImGui::Separator();
		ImGui::Text("Name"); ImGui::NextColumn();
		ImGui::Text("Total allocated"); ImGui::NextColumn();
		ImGui::Text("Allocated"); ImGui::NextColumn();
		ImGui::Text("Freed"); ImGui::NextColumn();
		ImGui::Separator();

		//sort(m_allocations.begin(), m_allocations.end(), compareAllocations); 

		foreach(a, m_allocations) {
			ImGui::Text(a.first.c_str()); ImGui::NextColumn();
			ImGui::Text("%d", a.second.m_totalAllocations); ImGui::NextColumn();
			ImGui::Text("%d", a.second.m_allocations); ImGui::NextColumn();
			ImGui::Text("%d", a.second.m_freed); ImGui::NextColumn();
		}
		ImGui::Columns(1);
		ImGui::Separator();
	}
};

static Memory* GetMemory() { return Memory::GetInstance(); }

//inline void* operator new(size_t size) {
//	return nullptr;
//}

inline void* operator new(size_t size, String_t name) {
	return Memory::AllocateDebug(size, name);
}

inline void* operator new[](size_t size, String_t name) {
	return Memory::AllocateDebug(size, name);
}

//template<typename T, class... Args>
//inline void* Allocate(size_t size, Args&&... _Val) {
//	void* ptr = Memory::AllocateDebug(size, "A");
//	*ptr = T{ forward<T>(_Val) }
//
//	return ptr;
//}
template<typename T>
inline void Destroy(T* ptr) {
	ptr->~T();
	Memory::FreeDebug(ptr, typeid(T).name());
}

template<typename T, size_t N>
inline void DestroyArray(T(&a)[N]) {
	return _DestroyArray(a, N);
}

template <typename T, int N>
void _DestroyArray(T(&a)[N]) {
	for (int i = 0; i < N; ++i) a[i].~T();
	Memory::FreeDebug(ptr, name);
}

//inline void* operator new[](size_t size) {
//	return Memory::Allocate(size);
//}
//
//inline void* operator new[](size_t size, String_t name) {
//	return Memory::AllocateDebug(size, name);
//}
//
//inline void operator delete(void* block) {
//	Memory::Free(block);
//}
//
//inline void operator delete(void* block, String_t name) {
//	Memory::FreeDebug(block, name);
//}
//
//inline void operator delete[](void* block) {
//	Memory::Free(block);
//}
//
//inline void operator delete[](void* block, String_t name) {
//	Memory::FreeDebug(block, name);
//}

