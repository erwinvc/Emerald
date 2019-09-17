#include "stdafx.h"

void GameState::Cleanup() {

}

void GameState::Initialize() {
	playerLight = Pointlight(Vector3(0, 0, 0), 3, Color::White());
	texIri = GetAssetManager()->Get<Texture>("Irridescence");
	texNoise = GetAssetManager()->Get<Texture>("Noise");
	m_geometryShader = GetShaderManager()->Get("Geometry");
	m_uiShader = GetShaderManager()->Get("UI");
	//m_pointlights.push_back(Pointlight(GetCamera()->m_position, 25, Color::White()));
	m_ludo = new Ludo();
	m_lamp = GetAssetManager()->Get<Model>("Lamp");
	m_sphere = GetAssetManager()->Get<Model>("Sphere");

	AssetRef<BasicMaterial> mat = GetMaterialManager()->Create<BasicMaterial>("Lamp");
	mat->SetAlbedo(GetAssetManager()->Get<Texture>("Concrete"));
	mat->SetNormal(GetAssetManager()->Get<Texture>("BricksNormal"));
	mat->SetSpecular(GetAssetManager()->Get<Texture>("ConcreteSpec"));
	m_lamp->SetMaterial(mat);

	mat2 = GetMaterialManager()->Create<BasicMaterial>("Sphere");
	mat2->SetAlbedo(GetAssetManager()->Get<Texture>("SphereAlbedo"));
	mat2->SetNormal(GetAssetManager()->Get<Texture>("SphereNormal"));
	mat2->SetSpecular(GetAssetManager()->Get<Texture>("SphereSpec"));
	mat2->SetEmission(GetAssetManager()->Get<Texture>("SphereEmission"));
	mat2->m_emissionStrength = 5;
	m_sphere->SetMaterial(mat2);
	//entity->m_scale = Vector3(0.02f);

	map<char, TileDefinition*> m_table = {
		{'*', &TileDefinition::ANY },
		{'s', &TileDefinition::SOLID },
		{'e', &TileDefinition::EMPTY},
		{'r', &TileDefinition::ROOM},
		{'h', &TileDefinition::HOOK},
		{'l', &TileDefinition::LOCK},
		{'b', &TileDefinition::LAMP}
	};
	m_ludo->SetTable(m_table);
	m_ludo->AddCallback(0, [&] {drawDebugLines = true; });
	m_ludo->AddGrammar(0, Grammar(3, 3, "shsssssss", { "sessesshs", "seshehshs", "seshessss", "seshesshs", "seshehsss" }));
	m_ludo->AddGrammar(0, Grammar(6, 6, "**h****sss**sssss*sssss*sssss**sss**", { "**e*****e***srrrs*hrrrs*srrrs**shs**", "**e*****e***srrrs*hrrrh*srrrs**sss**", "**e*****e***srrrs*hrrrh*srrrs**shs**" }));
	m_ludo->AddGrammar(0, Grammar(9, 9, "****h*****sssssss**sssssss*sssssssssssssssssssssssssss*sssssss**sssssss****sss***", { "****e*****sssesss**srrrrrs**srrrrrs*ssrrrrrssherrrrrehssrrrrrss*sssesss****shs***",
	"****e*****sssesss**srrrrrs**srrrrrs*ssrrrrrssherrrrrehssrrrrrss*sssssss**********", "****e*****sssesss**srrrrrs**srrrrrs**srrrrrs**srrrrrs**srrrrrs**sssesss****shs***", "****e*****sssesss**srrrrrs**srrrrrs*ssrrrrrssherrrrrssssrrrrrss*sssesss****shs***" }));
	m_ludo->AddGrammar(0, Grammar(7, 7, "**shs****sss****sss****sss****sss****sss****sss**", { "**ses****ses****ses****ses****ses****ses****shs**", "**ses****ses****ses****seh****ses****ses****shs**", "**ses****ses****ses****heh****ses****ses****shs**" }));
	m_ludo->AddGrammar(1, Grammar(1, 1, "h", { "s" }));

	m_ludo->AddCustomCode(2, [](Tile* tile) {
		for (int i = 0; i < tile->m_tiles.size(); i++) {
			if (tile->m_tiles[i]->IsSolid()) {
				Utils::RemoveFromVector(tile->m_tiles, tile->m_tiles[i]);
			}
		}
	});

	m_ludo->AddGrammar(2, Grammar(5, 5, "ss*sssrrrs*rrr*srrrsss*ss", { "ss*sssbrrs*rrr*srrrsss*ss" })); //Add lamps to small rooms
	m_ludo->AddGrammar(2, Grammar(5, 5, "rrrrrrrrrrrrrrrrrrrrrrrrr", { "brrrrrrrrrrrrrrrrrrrrrrrr", "rrrrrbrrrrrrrrrrrrrrrrrrr", "rbrrrrrrrrrrrrrrrrrrrrrrr" })); //Add lamps to big rooms

	m_ludo->AddCallback(3, [&] {
		Tile* tile = GetWorld()->GetTile(14, 0);
		float distance = 0;

		Tile* deepestTile = nullptr;
		FindDeepestNode(tile, 0, distance, deepestTile);

		deepestTile->SetDefinition(&TileDefinition::SOLID2);
		goal = deepestTile;

		aStar = new AStar();
		AStarResult res = aStar->FindPath(tile, goal);
		path = res.m_nodes;
		if (!res.m_finished)LOG_ERROR("Failed to find path!");
		delete aStar;
		aStar = nullptr;

		for (auto& a : path) {
			a.m_tile->SetDefinition(&TileDefinition::HOOK);
		}
	});

	//m_ludo->AddGrammar(4, Grammar(3, 3, "*h*shs*h*", { "*h*sls*h*" })); //Add locks
	//m_ludo->AddGrammar(4, Grammar(3, 3, "*h*hhs*s*", { "*h*hls*s*" })); //Add locks

	m_ludo->AddGrammar(4, Grammar(3, 3, "shs******", { "sls******" })); //Add locks

	m_ludo->AddGrammar(5, Grammar(1, 1, "h", { "e" })); //Replace path with empty

	m_ludo->AddCallback(6, [&] {
		aStar = new AStar();
		bool canBypass = false;
		while (true) {
			AStarResult result = aStar->FindPathBypassLocks(GetWorld()->GetTile(14, 0), goal, canBypass);
			if (result.m_finished) {
				delete aStar;
				aStar = nullptr;
				return;
			} else {
				int rand = Math::RandomInt(0, result.m_nodes.size() - 1);
				result.m_nodes[rand].m_tile->SetDefinition(&TileDefinition::KEY);
				canBypass = true;
			}
		}
	});

	m_ludo->AddGrammar(7, Grammar(4, 4, "srrrsrrrsrrr****", { "srrrsbrrsrrr****" }));
	m_ludo->AddGrammar(8, Grammar(1, 1, "r", { "e" })); //Replace room tiles with empties
	m_ludo->AddCustomCode(8, [&](Tile* tile) {
		if (tile->m_definition == &TileDefinition::LAMP) {
			tile->SetDefinition(&TileDefinition::EMPTY);
			m_pointlights.push_back(Pointlight(Vector3(tile->m_position.x + 0.5f, 1.5f, tile->m_position.y + 0.5f), 10, (Color::RandomPrimary() + Color(0.1f, 0.1f, 0.1f)) * 5));
			Entity* e = new Entity(m_lamp);
			e->m_position.x = tile->m_position.x + 0.5f;
			e->m_position.z = tile->m_position.y + 0.5f;
			m_lamps.push_back(e);
			//Sleep(15);
		}
	});

	m_ludo->AddCustomCode(8, [&](Tile* tile) {
		if (tile->m_definition == &TileDefinition::KEY) {
			tile->SetDefinition(&TileDefinition::EMPTY);
			WangTile::UpdateArea(tile->m_position.x, tile->m_position.y);
			Entity* e = new Entity(m_sphere);
			e->m_position.x = tile->m_position.x + 0.5f;
			e->m_position.y = 0.5f;
			e->m_position.z = tile->m_position.y + 0.5f;
			m_keys.push_back({ e, Pointlight(Vector3(tile->m_position.x + 0.5f, 1.5f, tile->m_position.y + 0.5f), 3, (Color::Red()) * 2) });
			//Sleep(15);
		}
	});

	m_ludo->AddCustomCode(8, [](Tile* tile) {
		if (tile->m_definition == &TileDefinition::LOCK) {
			tile->SetFull();
			WangTile::UpdateTile(tile->m_position, tile);
			WangTile::UpdateArea(tile->m_position.x, tile->m_position.y);
		}
	});

	m_ludo->AddCallback(9, [&] {drawDebugLines = false; });

	m_ludo->SetRecipeMaxExecutions(4, 4);
	m_ludo->SetRecipeMaxExecutions(7, 40);

	GetWorld()->GetTile(14, 0)->SetDefinition(&TileDefinition::HOOK);
}