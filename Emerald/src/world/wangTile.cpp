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
		if (tile && tile->IsSolid()) UpdateTile(pos, tile);
	}
}

void WangTile::UpdateTile(Vector2I& pos, Tile* tile) {
	int index = 0;
	for (int i = 0; i < 8; i++) {
		Tile* surroundingTile = GetWorld()->GetTile(pos + m_positionOffsets[i]);
		if (surroundingTile != nullptr && surroundingTile->IsSolid()) index += 1 << i;
	}

	switch (index) {
	case 255: tile->Set(GROUND, UP); break;
	case 0:
	case 2:
	case 10:
	case 42: tile->Set(SINGLE, UP); break;
	case 8:
	case 40:
	case 168: tile->Set(SINGLE, RIGHT); break;
	case 32:
	case 34:
	case 160:
	case 162: tile->Set(SINGLE, DOWN); break;
	case 128:
	case 130:
	case 136:
	case 138:
	case 170: tile->Set(SINGLE, LEFT); break;
	case 16:
	case 18:
	case 24:
	case 26:
	case 48:
	case 50:
	case 56:
	case 58:
	case 144:
	case 146:
	case 152:
	case 154:
	case 176:
	case 178:
	case 184:
	case 186: tile->Set(SINGLECONNECTION, UP); break;
	case 64:
	case 66:
	case 72:
	case 74:
	case 96:
	case 98:
	case 104:
	case 106:
	case 192:
	case 194:
	case 200:
	case 202:
	case 224:
	case 226:
	case 232:
	case 234: tile->Set(SINGLECONNECTION, RIGHT); break;
	case 1:
	case 3:
	case 9:
	case 11:
	case 33:
	case 35:
	case 41:
	case 43:
	case 129:
	case 131:
	case 137:
	case 139:
	case 161:
	case 163:
	case 169:
	case 171: tile->Set(SINGLECONNECTION, DOWN); break;
	case 4:
	case 6:
	case 12:
	case 14:
	case 36:
	case 38:
	case 44:
	case 46:
	case 132:
	case 134:
	case 140:
	case 142:
	case 164:
	case 166:
	case 172:
	case 174: tile->Set(SINGLECONNECTION, LEFT); break;
	case 65:
	case 67:
	case 73:
	case 75:
	case 97:
	case 99:
	case 105:
	case 107: tile->Set(SINGLECORNER, UP); break;
	case 5:
	case 13:
	case 37:
	case 45:
	case 133:
	case 141:
	case 165:
	case 173: tile->Set(SINGLECORNER, RIGHT); break;
	case 20:
	case 22:
	case 52:
	case 54:
	case 148:
	case 150:
	case 180:
	case 182: tile->Set(SINGLECORNER, DOWN); break;
	case 80:
	case 82:
	case 88:
	case 90:
	case 208:
	case 210:
	case 216:
	case 218: tile->Set(SINGLECORNER, LEFT); break;
	case 28:
	case 30:
	case 60:
	case 62:
	case 156:
	case 158:
	case 188:
	case 190: tile->Set(OUTER, UP); break;
	case 112:
	case 114:
	case 120:
	case 122:
	case 240:
	case 242:
	case 248:
	case 250: tile->Set(OUTER, RIGHT); break;
	case 193:
	case 195:
	case 201:
	case 203:
	case 225:
	case 227:
	case 233:
	case 235: tile->Set(OUTER, DOWN); break;
	case 7:
	case 15:
	case 39:
	case 47:
	case 135:
	case 143:
	case 167:
	case 175: tile->Set(OUTER, LEFT); break;
	case 19:
	case 25:
	case 27:
	case 57:
	case 59:
	case 153:
	case 155: tile->Set(SINGLESTRAIGHT, UP); break;
	case 76:
	case 100:
	case 108:
	case 110:
	case 228:
	case 236: tile->Set(SINGLESTRAIGHT, RIGHT); break;
	case 17:
	case 49:
	case 51:
	case 145:
	case 147:
	case 177:
	case 179:
	case 185:
	case 187: tile->Set(SINGLESTRAIGHT, DOWN); break;
	case 68:
	case 70:
	case 78:
	case 102:
	case 196:
	case 198:
	case 204:
	case 206:
	case 230:
	case 238: tile->Set(SINGLESTRAIGHT, LEFT); break;
	case 69:
	case 77:
	case 101:
	case 109: tile->Set(T, UP); break;
	case 21:
	case 53:
	case 149:
	case 181: tile->Set(T, RIGHT); break;
	case 84:
	case 86:
	case 212:
	case 214: tile->Set(T, DOWN); break;
	case 81:
	case 83:
	case 89:
	case 91: tile->Set(T, LEFT); break;
	case 29:
	case 61:
	case 157:
	case 189: tile->Set(SLOPETOSINGLE1, UP); break;
	case 116:
	case 118:
	case 244:
	case 246: tile->Set(SLOPETOSINGLE1, RIGHT); break;
	case 209:
	case 211:
	case 217:
	case 219: tile->Set(SLOPETOSINGLE1, DOWN); break;
	case 71:
	case 79:
	case 103:
	case 111: tile->Set(SLOPETOSINGLE1, LEFT); break;
	case 23:
	case 55:
	case 151:
	case 183: tile->Set(SLOPETOSINGLE2, UP); break;
	case 92:
	case 94:
	case 220:
	case 222: tile->Set(SLOPETOSINGLE2, RIGHT); break;
	case 113:
	case 115:
	case 121:
	case 123: tile->Set(SLOPETOSINGLE2, DOWN); break;
	case 197:
	case 205:
	case 229:
	case 237: tile->Set(SLOPETOSINGLE2, LEFT); break;
	case 124:
	case 126:
	case 252:
	case 254: tile->Set(SLOPE, UP); break;
	case 241:
	case 243:
	case 249:
	case 251: tile->Set(SLOPE, RIGHT); break;
	case 199:
	case 207:
	case 231:
	case 239: tile->Set(SLOPE, DOWN); break;
	case 31:
	case 63:
	case 159:
	case 191: tile->Set(SLOPE, LEFT); break;
	case 85: tile->Set(CROSS, LEFT); break;
	case 93: tile->Set(OUTERTODOUBLE, UP); break;
	case 117: tile->Set(OUTERTODOUBLE, RIGHT); break;
	case 213: tile->Set(OUTERTODOUBLE, DOWN); break;
	case 87: tile->Set(OUTERTODOUBLE, LEFT); break;
	case 125: tile->Set(TSOLID, UP); break;
	case 245: tile->Set(TSOLID, RIGHT); break;
	case 215: tile->Set(TSOLID, DOWN); break;
	case 95: tile->Set(TSOLID, LEFT); break;
	case 119: tile->Set(VALLEY, DOWN); break;
	case 221: tile->Set(VALLEY, LEFT); break;
	case 127: tile->Set(INNER, UP); break;
	case 253: tile->Set(INNER, RIGHT); break;
	case 247: tile->Set(INNER, DOWN); break;
	case 223: tile->Set(INNER, LEFT); break;
	}
}