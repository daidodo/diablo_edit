#pragma once

#include "stdafx.h"
#include <string>
#include <vector>

//物品元数据
struct CItemMetaData
{
#ifdef _DEBUG
	std::string name;		//物品名字（唯一ID）, 如"elx "，DEBUG用
#endif
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
	BOOL HasDef = FALSE;	//有防御值
	BOOL HasDur = FALSE;	//有耐久度
	BOOL IsStacked = FALSE;	//有数量
	BOOL IsTome = FALSE;	//是回城书或辨识书,有iTome(5 bits)域
	BOOL HasMonsterID = FALSE;	//有wMonsterID(10 bits)域
	BOOL IsCharm = FALSE;	//是护身符(放在身上加属性的),有wCharm域
	BOOL HasSpellID = FALSE;//有bSpellID域
	BOOL IsUnique = FALSE;	//是否是Unique物品(在没有iQuality域情况下)
	UINT Damage1Min = 0;	//武器的单手最低伤害
	UINT Damage1Max = 0;	//武器的单手最高伤害
	UINT Damage2Min = 0;	//武器的双手最低伤害
	UINT Damage2Max = 0;	//武器的双手最高伤害
};

//属性元数据
class CPropertyMetaData
{
	std::vector<std::pair<int, int>> fields_;
	int bitsSum_ = 0;
public:
	CPropertyMetaData() {}
	CPropertyMetaData(std::vector<std::pair<int, int>> fields);
	int Bits() const { return bitsSum_; }
	std::vector<int> Parse(DWORD value) const;
};

CString CSFormat(LPCTSTR lpszFormat, ...);
