#pragma once

class Node {
public:
	Tile* m_tile;
	Node* m_parent;
	float m_fCost, m_gCost, m_hCost;

	Node() {}
	Node(Tile* tile, Node* parent, float gCost, float hCost) : m_tile(tile), m_parent(parent), m_fCost(0), m_gCost(gCost), m_hCost(hCost) {
		m_fCost = m_gCost + m_hCost;
	}

	inline bool operator==(const Node& node) {
		return m_tile == node.m_tile;
	}
};

struct AStarResult {
	bool m_finished;
	vector<Node> m_nodes;
};

class AStar {
	static int Compare(Node*& o1, Node*& o2) {
		if (o2->m_fCost < o1->m_fCost) return 1;
		if (o2->m_fCost > o1->m_fCost) return -1;
		return 0;
	}


	static bool TileInVec(vector<Node*>& list, Tile* tile) {
		for (Node* n : list) {
			if (n->m_tile == tile) return true;
		}
		return false;
	}

public:
	vector<Vector2I> m_open;
	vector<Vector2I> m_closed;
	AStarResult FindPath(Tile* start, Tile* goal) {
		vector<Node*> allNodes;
		vector<Node*> openList;
		vector<Node*> closedList;
		Node* current = new Node(start, nullptr, 0, start->m_position.Distance(goal->m_position));
		allNodes.push_back(current);
		openList.push_back(allNodes[0]);
		while (openList.size() > 0) {
			sort(openList.begin(), openList.end(), Compare);
			current = openList[0];
			if (current->m_tile == goal) {
				vector<Node> path;
				while (current->m_parent != nullptr) {
					path.push_back(*current);
					current = current->m_parent;
				}
				openList.clear();
				closedList.clear();
				for (Node* node : allNodes) delete node;
				return { true, path };
			}
			Utils::RemoveFromVector(openList, current);
			closedList.push_back(current);

			//Sleep(1);
			for (int i = 0; i < 9; i++) {
				if (i == 4 || i == 0 || i == 2 || i == 6 || i == 8) continue;
				int x = current->m_tile->m_position.x;
				int y = current->m_tile->m_position.y;
				int xi = (i % 3) - 1;
				int yi = (i / 3) - 1;

				m_open.clear();
				m_open.reserve(openList.size());
				m_closed.clear();
				m_closed.reserve(closedList.size());

				for (int i = 0; i < closedList.size(); i++) if (closedList[i]) m_closed.push_back(closedList[i]->m_tile->m_position);
				for (int i = 0; i < openList.size(); i++) if (openList[i]) m_open.push_back(openList[i]->m_tile->m_position);

				Tile* at = GetWorld()->GetTile(x + xi, y + yi);
				if (at == nullptr) continue;
				if (at->IsSolid()) continue;

				double gCost = current->m_gCost + current->m_tile->m_position.Distance(at->m_position);
				double hCost = at->m_position.Distance(goal->m_position);
				Node* node = new Node(at, current, gCost, hCost);
				allNodes.push_back(node);
				if (TileInVec(closedList, at) && gCost >= node->m_gCost) continue;
				if (!TileInVec(openList, at) || gCost < node->m_gCost) openList.push_back(node);
			}
		}
		vector<Node> resultingNodes;
		for (Node* node : closedList) {
			resultingNodes.push_back(*node);
		}
		closedList.clear();
		for (Node* node : allNodes) delete node;
		return { false, resultingNodes };
	}

	AStarResult FindPathBypassLocks(Tile* start, Tile* goal, bool canBypass) {
		bool bypassedLock = !canBypass;
		vector<Node*> allNodes;
		vector<Node*> openList;
		vector<Node*> closedList;
		Node* current = new Node(start, nullptr, 0, start->m_position.Distance(goal->m_position));
		allNodes.push_back(current);
		openList.push_back(allNodes[0]);
		while (openList.size() > 0) {
			sort(openList.begin(), openList.end(), Compare);
			current = openList[0];
			if (current->m_tile == goal) {
				vector<Node> path;
				while (current->m_parent != nullptr) {
					path.push_back(*current);
					current = current->m_parent;
				}
				openList.clear();
				closedList.clear();
				for (Node* node : allNodes) delete node;
				return { true, path };
			}
			Utils::RemoveFromVector(openList, current);
			closedList.push_back(current);

			//Sleep(1);
			for (int i = 0; i < 9; i++) {
				if (i == 4 || i == 0 || i == 2 || i == 6 || i == 8) continue;
				int x = current->m_tile->m_position.x;
				int y = current->m_tile->m_position.y;
				int xi = (i % 3) - 1;
				int yi = (i / 3) - 1;

				m_open.clear();
				m_open.reserve(openList.size());
				m_closed.clear();
				m_closed.reserve(closedList.size());

				for (int i = 0; i < closedList.size(); i++) if (closedList[i]) m_closed.push_back(closedList[i]->m_tile->m_position);
				for (int i = 0; i < openList.size(); i++) if (openList[i]) m_open.push_back(openList[i]->m_tile->m_position);

				Tile* at = GetWorld()->GetTile(x + xi, y + yi);
				if (at == nullptr) continue;


				bool isRockAndCanPass = (at->m_definition == &TileDefinition::LOCK && !bypassedLock);
				if (at->IsSolid() && isRockAndCanPass) {
					at->SetEmpty();
					bypassedLock = true;
				}
				if (at->IsSolid()) continue;

				double gCost = current->m_gCost + current->m_tile->m_position.Distance(at->m_position);
				double hCost = at->m_position.Distance(goal->m_position);
				Node* node = new Node(at, current, gCost, hCost);
				allNodes.push_back(node);
				if (TileInVec(closedList, at) && gCost >= node->m_gCost) continue;
				if (!TileInVec(openList, at) || gCost < node->m_gCost) openList.push_back(node);
			}
		}
		vector<Node> resultingNodes;
		for (Node* node : closedList) {
			resultingNodes.push_back(*node);
		}
		closedList.clear();
		for (Node* node : allNodes) delete node;
		return { false, resultingNodes };
	}

	void Draw() {
		for (Vector2I& vec : m_closed) {
			GetLineRenderer()->DrawRect(Rect((float)vec.x, (float)vec.y + 0.5f, 0.5f, 0.5f), Color(0.35f, 0, 0));
		}

		for (Vector2I& vec : m_open) {
			GetLineRenderer()->DrawRect(Rect((float)vec.x, (float)vec.y + 0.5f, 0.5f, 0.5f), Color::Green());
		}
	}

};