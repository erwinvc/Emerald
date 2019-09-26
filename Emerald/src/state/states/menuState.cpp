#include "stdafx.h"

int ReadIntLeftToRight(byte* address) {
	byte bytes[4];
	bytes[0] = *(address + 3);
	bytes[1] = *(address + 2);
	bytes[2] = *(address + 1);
	bytes[3] = *(address);
	return *(int32*)&bytes[0];
}

struct Labels {
private:
	int32 m_magicNumber;
	int32 m_itemCount;
	byte m_items[60000];

public:
	int GetMagicNumber() { return ReadIntLeftToRight((byte*)&m_magicNumber); }
	int GetItemCount() { return ReadIntLeftToRight((byte*)&m_itemCount); }
	byte GetByte(int index) { return m_items[index]; }
};

struct Images {
private:
	int32 m_magicNumber;
	int32 m_itemCount;
	int32 m_rowCount;
	int32 m_columnCount;
	byte m_items[60000 * 28 * 28];

public:
	int GetMagicNumber() { return ReadIntLeftToRight((byte*)&m_magicNumber); }
	int GetItemCount() { return ReadIntLeftToRight((byte*)&m_itemCount); }
	int GetRowCount() { return ReadIntLeftToRight((byte*)&m_rowCount); }
	int GetColumnCount() { return ReadIntLeftToRight((byte*)&m_columnCount); }

	vector<double> GetImage(int index) {
		int imageIndex = index * (28 * 28);
		byte* first = &m_items[imageIndex];
		vector<double> imageValues;
		imageValues.reserve(28 * 28);
		for (int y = 27; y >= 0; y--) {
			for (int x = 27; x >= 0; x--) {
				byte* b = first + (y * 28 + x);
				float value = (float)*b / (float)0xff;
				imageValues.push_back(value < 0.5f ? 1.0 : -1.0);
			}
		}
		return imageValues;
	}
};


void MenuState::LoadNumbers() {
	vector<byte> imagesData = FileSystem::ReadFileBinary("images.dat");
	vector<byte> labelsData = FileSystem::ReadFileBinary("labels.dat");

	byte* imagesFirst = &imagesData[0];
	byte* labelsFirst = &labelsData[0];

	Images* images = (Images*)imagesFirst;
	Labels* labels = (Labels*)labelsFirst;
	if (labels->GetMagicNumber() != 2049) LOG_ERROR("Magic number mismatch");
	if (labels->GetItemCount() != 60000) LOG_ERROR("Item count mismatch");

	m_numbers.reserve(60000);

	for (int i = 0; i < 60000; i++) {
		m_numbers[i].number = labels->GetByte(i);
		m_numbers[i].data = images->GetImage(i);
	}
}

