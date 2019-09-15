#pragma once

class Grammar {
public:
	int m_x;
	int m_y;

	String m_toFind[6];
	vector<array<String, 6>> m_toReplace;

	template<typename... Args>
	Grammar(int x, int y, const String& toFind, initializer_list<String> toReplace) : m_x(x), m_y(y) {
		int size = x * y;
		if (toFind.size() != size) LOG_ERROR("Grammar string length doesn't match!");
		m_toFind[0] = toFind;
		m_toFind[1] = Utils::RotateString90(m_toFind[0], x, y);
		m_toFind[2] = Utils::RotateString90(m_toFind[1], x, y);
		m_toFind[3] = Utils::RotateString90(m_toFind[2], x, y);
		m_toFind[4] = Utils::FlipStringHorizontal(toFind, x, y);
		m_toFind[5] = Utils::FlipStringVertical(toFind, x, y);
		for (String str : toReplace) {
			if (str.size() != size) LOG_ERROR("Grammar string length doesn't match!");
			array<String, 6> newArray;
			newArray[0] = str;
			newArray[1] = Utils::RotateString90(newArray[0], x, y);
			newArray[2] = Utils::RotateString90(newArray[1], x, y);
			newArray[3] = Utils::RotateString90(newArray[2], x, y);
			newArray[4] = Utils::FlipStringHorizontal(str, x, y);
			newArray[5] = Utils::FlipStringVertical(str, x, y);
			m_toReplace.push_back(newArray);
		}
	}
};

class Ludo {
private:
	map<char, TileDefinition*> m_table;

	const Grammar& GetRandomGrammar(const vector<Grammar>& grammarCollection) {
		return grammarCollection[Math::RandomInt(0, grammarCollection.size() - 1)];
	}

	void DoCode(const vector<function<void(Tile*)>>& codeCollection) {
		for (int y = 0; y < MAX_Y; y++) {
			for (int x = 0; x < MAX_X; x++) {
				for (int i = 0; i < codeCollection.size(); i++) {
					codeCollection[i](GetWorld()->GetTile(x, y));
				}
			}
		}
	}
	bool DoIteration(const vector<Grammar>& grammarCollection) {
		for (int y = 0; y < MAX_Y; y++) {
			for (int x = 0; x < MAX_X; x++) {
				if (CheckGrammar(x, y, GetRandomGrammar(grammarCollection), Math::RandomInt(0, 5))) return true;
			}
		}
		return false;
	}

public:
	const int MAX_X = 64;
	const int MAX_Y = 64;
	Rect lastBounds;
	Rect lastBounds2;
	Color color;
	vector<vector<Grammar>> m_grammar;
	vector<vector<function<void(Tile*)>>> m_code;
	void SetTable(map<char, TileDefinition*>& table) {
		m_table = table;
	}

	void AddCustomCode(int step, function<void(Tile*)> code) {
		while (m_code.size() <= step) {
			m_code.push_back(vector<function<void(Tile*)>>());
		}
		m_code[step].push_back(code);
	}

	void AddGrammar(int step, const Grammar& grammar) {
		while (m_grammar.size() <= step) {
			m_grammar.push_back(vector<Grammar>());
		}
		m_grammar[step].push_back(grammar);
	}

	bool CheckGrammar(int x, int y, const Grammar& grammar, int grammarRotation);


	void Iterate() {
		GetThreadPool()->DoJob([&] {
			DoIteration(m_grammar[0]);
		});
	}

	void Execute(int max) {
		GetThreadPool()->DoJob([this, max] {

			for(int i = 0; i < Math::Max((int)m_code.size(), (int)m_grammar.size()); i++)
			{
				if (m_code.size() >= i) loop(x, max) DoCode(m_code[i]);
				if (m_grammar.size() >= i) loop(x, max) DoIteration(m_grammar[i]);
			}
			for (auto codeCollection : m_code) {
				loop(x, max) DoCode(codeCollection);
			}
			for (auto grammCollection : m_grammar) {
				loop(x, max) DoIteration(grammCollection);
			}
		});
	}

	void DebugDraw() {
		GetLineRenderer()->DrawRect(lastBounds, Color::Blue());
		GetLineRenderer()->DrawRect(lastBounds2, color);
	}
};
