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

class Recipe {
public:
	int m_maxExecutions;
	vector<Grammar> m_grammar;
	vector<function<void(Tile*)>> m_calls;
	function<void()> m_callback = [] {};
	Recipe(int max) : m_maxExecutions(max) {}
};

class Ludo {
private:
	map<char, TileDefinition*> m_table;

	bool DoCode(Recipe& recipe) {
		if (recipe.m_calls.size() == 0) return false;
		for (int y = 0; y < MAX_Y; y++) {
			for (int x = 0; x < MAX_X; x++) {
				for (int i = 0; i < recipe.m_calls.size(); i++) {
					recipe.m_calls[i](GetWorld()->GetTile(x, y));
				}
			}
		}
		return true;
	}

	struct AGrammar {
		Grammar* m_grammar;
		Vector2I m_position;
		int m_index;
		Tile* m_tempTile;
		AGrammar(Grammar* g, Vector2I& pos, int index, Tile* tempTile) : m_grammar(g), m_position(pos), m_index(index), m_tempTile(tempTile) {}
	};
	bool DoIteration(Recipe& recipe) {
		vector<AGrammar> coll;
		for (int y = 0; y < MAX_Y; y++) {
			for (int x = 0; x < MAX_X; x++) {
				for (auto& grammar : recipe.m_grammar) {
					for (int i = 0; i < 6; i++) {
						Tile* tempTile;
						if (CheckGrammar(x, y, grammar, i, tempTile)) {
							coll.push_back(AGrammar(&grammar, Vector2I(x, y), i, tempTile));
						}
					}
				}
			}
		}

		if (coll.size() == 0) return false;
		std::mt19937 randgen(time(0));
		int x = std::uniform_int_distribution<int>(0, coll.size() - 1)(randgen);
		AGrammar& a = coll[x];
		ApplyGrammar(a.m_position.x, a.m_position.y, *a.m_grammar, a.m_index, a.m_tempTile);
		return true;
	}

public:
	const int MAX_X = 32;
	const int MAX_Y = 32;
	Rect lastBounds;
	Rect lastBounds2;
	Color color;
	vector<Recipe> m_recipes;
	void SetTable(map<char, TileDefinition*>& table) {
		m_table = table;
	}

	void AddCustomCode(int step, function<void(Tile*)> code) {
		while (m_recipes.size() <= step) {
			m_recipes.push_back(Recipe(-1));
		}
		m_recipes[step].m_calls.push_back(code);
	}

	void SetRecipeMaxExecutions(int recipe, int steps) {
		if (recipe > m_recipes.size()) LOG_ERROR("Recipe doesnt exist");
		m_recipes[recipe].m_maxExecutions = steps;
	}

	void AddCallback(int step, function<void()> callback) {
		while (m_recipes.size() <= step) {
			m_recipes.push_back(Recipe(-1));
		}
		m_recipes[step].m_callback = callback;
	}

	void AddGrammar(int step, const Grammar& grammar) {
		while (m_recipes.size() <= step) {
			m_recipes.push_back(Recipe(-1));
		}
		m_recipes[step].m_grammar.push_back(grammar);
	}

	bool CheckGrammar(int x, int y, Grammar& grammar, int grammarRotation, Tile*& tempTile);
	void ApplyGrammar(int x, int y, Grammar& grammar, int grammarRotation, Tile* tempTile);

	void Iterate() {
		GetThreadPool()->DoJob([&] {
			DoIteration(m_recipes[0]);
		});
	}

	void Execute(int max) {
		GetThreadPool()->DoJob([this, max] {
			for (int i = 0; i < m_recipes.size(); i++) {
				int maxExecutions = m_recipes[i].m_maxExecutions;
				maxExecutions = maxExecutions == -1 ? 1000 : maxExecutions;
				m_recipes[i].m_callback();
				for (int j = 0; j < maxExecutions; j++) {
					if (!DoCode(m_recipes[i])) break;
				}
				for (int j = 0; j < maxExecutions; j++) {
					if (!DoIteration(m_recipes[i])) break;
				}
			}
		});
	}

	void DebugDraw() {
		GetLineRenderer()->DrawRect(lastBounds, Color::Blue());
		GetLineRenderer()->DrawRect(lastBounds2, color);
	}
};
