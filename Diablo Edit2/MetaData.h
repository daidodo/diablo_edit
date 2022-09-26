#pragma once

#include "stdafx.h"
#include <string>
#include <vector>

//物品元数据
struct CItemMetaData
{
	union {
		BYTE sTypeName[4];	//物品名字（唯一ID）, 如"elx "
		DWORD dwTypeID;
	};
	WORD PicIndex = 0;		//物品在Bitmap资源中相对第一个图片的索引偏移
	WORD NameIndex = 0;		//物品在ItemName列表中的名字索引
	BYTE Equip = 0;			//4 bits,物品可穿戴的位置,
							//	0	不可穿戴
							//	1	头
							//	2	项链
							//	3	身体
							//	4	武器
							//	5	戒指
							//	6	腰
							//	7	鞋子
							//	8	手套
							//	9	放在腰带里
	BYTE Range = 0;			//4 bits物品本身占用网格大小
	BOOL Simple = FALSE;	//物品是否有Smiple标志
	BOOL Normal = FALSE;	//质量只能是normal，除去Simple物品
	BOOL White = FALSE;		//质量只能是low, normal, high（白色），除去Simple和Normal物品
	BOOL HasDef = FALSE;	//有防御值
	BOOL HasDur = FALSE;	//有耐久度
	BOOL IsStacked = FALSE;	//有数量
	BOOL HasMonsterID = FALSE;//有wMonsterID(10 bits)域
	BOOL IsCharm = FALSE;	//是护身符(放在身上加属性的),有wCharm域
	UINT SpellId = 0;		//0：没有SpellId域；其他：SpellId + 1
	BOOL IsUnique = FALSE;	//是否是Unique物品(在没有iQuality域情况下)
	BOOL IsCraft = FALSE;	//名字是否以橙色显示，对物品结构无影响
	BOOL IsGem = FALSE;		//是否是宝石（可镶嵌）
	UINT Damage1Min = 0;	//武器的单手最低伤害
	UINT Damage1Max = 0;	//武器的单手最高伤害
	UINT Damage2Min = 0;	//武器的双手最低伤害
	UINT Damage2Max = 0;	//武器的双手最高伤害
	UINT Pad = 0;			//D2R里Simple物品最后的补偿bits
};

//属性的参数
struct CPropertyField
{
	int bits, base, min, max;
	//Functions:
	const CPropertyField & Normalize();
};

//属性元数据
class CPropertyMetaData
{
	std::vector<CPropertyField> fields_;
	DWORD def_ = 0;
	int bitsSum_ = 0;
public:
	CPropertyMetaData() {}
	CPropertyMetaData(const std::vector<CPropertyField> & fields, DWORD def);
	int Bits() const { return bitsSum_; }
	std::vector<int> Parse(DWORD value) const;
	std::vector<std::tuple<int, int, int>> GetParams(DWORD value) const;
	std::pair<BOOL, DWORD> GetValue(const std::vector<int> & params) const;
	DWORD DefaultValue() const { return def_; }
};

CString CSFormat(LPCTSTR lpszFormat, ...);
