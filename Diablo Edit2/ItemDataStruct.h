#pragma once

/*
结构说明:
ID			DWORD或BYTE[4],物品唯一ID,如"elx "
Pic			WORD,物品在Bitmap资源中相对第一个图片的索引偏移,
Name		WORD,物品在String列表中的名字索引
EqRange		BYTE,分配如下
				Equip	4 bits,物品可穿戴的位置,
					0	不可穿戴
					1	头
					2	项链
					3	身体
					4	武器
					5	戒指
					6	腰
					7	鞋子
					8	手套
					9	放在腰带里
				Range,4bits物品本身占用网格大小,((col-1) << 2) + (row-1)
Add			BYTE,其他,分配如下:
				bDef		1 bit,有防御值
				bDur		1 bit,有耐久度
				bStack		1 bit,有数量
				bTome		1 bit,是回城书或辨识书,有iTome(5 bits)域
				bMonster	1 bit,有wMonsterID(10 bits)域
				bCharm		1 bit,是护身符(放在身上加属性的),有wCharm域
				bSpell		1 bit,有bSpellID域
				bUnique		1 bit,是否是Unique物品(在没有iQuality域情况下)
Dmg1Min	    BYTE,武器的单手最低伤害
Dmg1Max	    BYTE,武器的单手最高伤害
Dmg2Min	    BYTE,武器的双手最低伤害
Dmg2Max	    BYTE,武器的双手最高伤害
//*/
class CItemDataStruct
{
//fields
	union{
		DWORD	dwTypeID;
		BYTE	sTypeName[4];
	};
	WORD	wPic;
	WORD	wName;
	BYTE	bEqRange;
	BYTE	bAdd;
    BYTE    bDmg1Min,bDmg1Max;
    BYTE    bDmg2Min,bDmg2Max;
public:
	static const int SIZE = 14;
//members
	DWORD	TypeID() const{return dwTypeID;}
	WORD	PicIndex() const{return wPic;}
	WORD	NameIndex() const{return wName;}
	BYTE	Equip() const{return bEqRange >> 4;}
	BYTE	Range() const{return ((bEqRange & 0xC) << 2) + (bEqRange & 3) + 0x11;}
	BOOL	HasDef() const{return (bAdd & 0x80) != 0;}
	BOOL	HasDur() const{return (bAdd & 0x40) != 0;}
	BOOL	IsStacked() const{return (bAdd & 0x20) != 0;}
	BOOL	IsTome() const{return (bAdd & 0x10) != 0;}
	BOOL	HasMonsterID() const{return (bAdd & 8) != 0;}
	BOOL	IsCharm() const{return (bAdd & 4) != 0;}
	BOOL	HasSpellID() const{return (bAdd & 2) != 0;}
	BOOL	IsUnique() const{return (bAdd & 1) != 0;}
    UINT    Damage1Min() const{return bDmg1Min;}
    UINT    Damage1Max() const{return bDmg1Max;}
    UINT    Damage2Min() const{return bDmg2Min;}
    UINT    Damage2Max() const{return bDmg2Max;}
};
