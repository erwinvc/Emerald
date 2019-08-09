#include "stdafx.h"

const Vector2I WangTile::m_positionOffsets[8] = {
		{0, 1},  //North
		{-1, 1}, //NorthEase
		{-1, 0}, //East
		{-1, -1},//SouthEast
		{0, -1}, //South
		{1, -1}, //SouthWest
		{1, 0},	 //West
		{1, 1}	 //Northwest
};

void WangTile::UpdateArea(int x, int z) {
	for (int i = 0; i < 8; i++) {
		Vector2I pos = Vector2I(x, z) + m_positionOffsets[i];
		Tile* tile = GetWorld()->GetTile(pos);
		if (tile && tile->IsRock()) UpdateTile(pos, tile);
	}
}

void WangTile::UpdateTile(Vector2I& pos, Tile* tile) {
	int index = 0;
	for (int i = 0; i < 8; i++) {
		Tile* surroundingTile = GetWorld()->GetTile(pos + m_positionOffsets[i]);
		if (surroundingTile != nullptr && surroundingTile->IsRock()) index += 1 << i;
	}

	switch (index) {
	case 26:
	case 31:
	case 58:
	case 59:
	case 63:
	case 154:
	case 158:
	case 159:
	case 186:
	case 191:
	case 234: tile->Set(SLOPE, UP); break;
	case 74:
	case 75:
	case 78:
	case 79:
	case 95:
	case 107:
	case 111:
	case 123:
	case 202:
	case 206:
	case 235:
	case 239: tile->Set(SLOPE, RIGHT); break;
	case 88:
	case 89:
	case 92:
	case 93:
	case 120:
	case 121:
	case 220:
	case 222:
	case 248:
	case 249:
	case 250:
	case 252:
	case 253: tile->Set(SLOPE, DOWN); break;
	case 82:
	case 83:
	case 87:
	case 114:
	case 115:
	case 214:
	case 215:
	case 242:
	case 246:
	case 247: tile->Set(SLOPE, LEFT); break;
	case 10:
	case 11:
	case 14:
	case 15:
	case 27:
	case 42:
	case 43:
	case 47:
	case 55:
	case 139:
	case 155:
	case 182:
	case 187:
	case 203:
	case 207:
	case 216: tile->Set(OUTER, UP); break;
	case 72:
	case 73:
	case 104:
	case 105:
	case 106:
	case 108:
	case 109:
	case 110:
	case 124:
	case 125:
	case 200:
	case 210:
	case 232:
	case 233:
	case 238: tile->Set(OUTER, RIGHT); break;
	case 80:
	case 84:
	case 86:
	case 112:
	case 208:
	case 209:
	case 211:
	case 212:
	case 217:
	case 221:
	case 240:
	case 243:
	case 244: tile->Set(OUTER, DOWN); break;
	case 18:
	case 19:
	case 22:
	case 23:
	case 30:
	case 54:
	case 62:
	case 118:
	case 119:
	case 146:
	case 150:
	case 151:
	case 190: tile->Set(OUTER, LEFT); break;
	case 127: tile->Set(INNER, UP); break;
	case 251: tile->Set(INNER, RIGHT); break;
	case 254: tile->Set(INNER, DOWN); break;
	case 223: tile->Set(INNER, LEFT); break;
	case 126: tile->Set(VALLEY, UP); break;
	case 219: tile->Set(INNER, DOWN); break;
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
	case 6:
	case 7:
	case 8:
	case 9:
	case 12:
	case 16:
	case 20:
	case 21:
	case 24:
	case 25:
	case 28:
	case 29:
	case 32:
	case 38:
	case 39:
	case 40:
	case 41:
	case 45:
	case 46:
	case 48:
	case 52:
	case 56:
	case 57:
	case 60:
	case 61:
	case 64:
	case 66:
	case 67:
	case 70:
	case 71:
	case 96:
	case 98:
	case 99:
	case 100:
	case 102:
	case 103:
	case 116:
	case 128:
	case 131:
	case 144:
	case 147:
	case 148:
	case 152:
	case 153:
	case 156:
	case 157:
	case 169:
	case 176:
	case 180:
	case 184:
	case 185:
	case 188:
	case 189:
	case 192:
	case 194:
	case 195:
	case 198:
	case 199:
	case 201:
	case 224:
	case 225:
	case 226:
	case 227:
	case 228:
	case 230:
	case 231:
	case 241: break; //GetWorld()->BreakTile(pos.x, pos.y); break;
	case 255: tile->SetFull(); break;
	}
}