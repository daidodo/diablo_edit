
// Diablo Edit2.h: Diablo Edit2 应用程序的主头文件
//
#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"       // 主符号
#include "MetaData.h"
#include "MayExist.h"
#include "D2S_Struct.h"

#include <vector>
#include <unordered_map>

struct CPropParam {
	int iValue;
	int iMin;
	int iMax;
};

// CDiabloEdit2App:
// 有关此类的实现，请参阅 Diablo Edit2.cpp
//

class CDiabloEdit2App : public CWinApp
{
public:
	CDiabloEdit2App() noexcept;

// 重写
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// 实现
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
// 自定义成员
public:
	static const UINT CLASS_NAME_SIZE = 7;					//角色数量
	static const UINT CLASS_SKILL_TAB_SIZE = 3;				//每种角色技能树数量
	static const UINT CLASS_SKILL_NAME_SIZE = 30;			//每种角色技能数量
	static const int CLASS_SKILL_INDEX[CLASS_NAME_SIZE][CLASS_SKILL_NAME_SIZE];	//每种角色每个技能索引偏移
	INT m_nLangIndex = 0;			//当前语言索引
	// Functions
	const CString & AppPath() const { return m_sAppPath; }
	INT LangCount() const { return m_saLanguage.size(); }
	CString PropertyDescription(DWORD version, WORD id, DWORD value) const;	//根据属性id和参数value,得到属性的描述字符串
	CString PropertyDescription(DWORD version, WORD id) const;				//根据属性id和参数value的默认值,得到属性的描述字符串
	std::vector<CPropParam> PropertyParameters(DWORD version, WORD id, DWORD value) const;
	const CString & LangTitle(UINT index) const {
		ASSERT(index < m_saLanguage.size());
		return m_saLanguage[index][0];
	}
	const CString & MsgWarning() const { return Msg(1); }
	const CString & MsgError() const { return Msg(2); }
	const CString & MsgNoTitle() const { return Msg(3); }
	const CString & MsgUnknown() const { return Msg(4); }
	const CString & Msg(UINT index) const { return String(SectionToIndex(OTHER_MSG, index)); }

	const CString & MenuPrompt(UINT index) const { return String(SectionToIndex(MENU_PROMPT, index)); }
	const CString & CharItemPopupMenu(UINT index) const { return String(SectionToIndex(CHAR_ITEM_POPUP_MENU, index)); }
	const CString & SystemMenu(UINT index) const { return String(SectionToIndex(SYSTEM_MENU, index)); }
	const CString & OtherUI(UINT index) const { return String(SectionToIndex(OTHER_UI, index)); }
	const CString & NewItemUI(UINT index) const { return String(SectionToIndex(NEW_ITEM_UI, index)); }
	const CString & FoundryUI(UINT index) const { return String(SectionToIndex(FOUNDRY_UI, index)); }
	const CString & ItemSuspendUI(UINT index) const { return String(SectionToIndex(ITEM_SUSPEND_UI, index)); }
	const CString & CharItemsUI(UINT index) const { return String(SectionToIndex(CHAR_ITEMS_UI, index)); }
	const CString & CharBasicInfoUI(UINT index) const { return String(SectionToIndex(CHAR_BASIC_INFO_UI, index)); }
	const CString & MsgBoxInfo(UINT index) const { return String(SectionToIndex(MSG_BOX_INFO, index)); }
	const CString & MercenaryBarbarianName(UINT index) const { return String(SectionToIndex(MERCENARY_NAME_BAR, index)); }
	const CString & MercenarySorcerorName(UINT index) const { return String(SectionToIndex(MERCENARY_NAME_SOR, index)); }
	const CString & MercenaryMercName(UINT index) const { return String(SectionToIndex(MERCENARY_NAME_MERC, index)); }
	const CString & MercenaryScoutName(UINT index) const { return String(SectionToIndex(MERCENARY_NAME_SCOUT, index)); }
	const CString & MercenaryTypeName(UINT index) const { return String(SectionToIndex(MERCENARY_TYPE_NAME, index)); }
	const CString & ItemCatagoryName(UINT index) const { return String(SectionToIndex(ITEM_CATAGORY_NAME, index)); }
	const CString & ItemQualityName(UINT index) const { return String(SectionToIndex(ITEM_QUALITY_NAME, index)); }
	const CString & DifficultyName(UINT index) const { return String(SectionToIndex(DIFFICULTY_NAME, index)); }
	const CString & WayPointName(UINT index) const { return String(SectionToIndex(WAY_POINT_NAME, index)); }
	const CString & QuestName(UINT index) const { return String(SectionToIndex(QUEST_NAME, index)); }
	const CString & NormalSkillName(UINT index) const { return String(SectionToIndex(NORMAL_SKILL_NAME, index)); }
	std::pair<CString, int> ClassSkillName(UINT skill_id) const;			//根据绝对技能索引得到技能名字，以及角色id
	const CString & ClassSkillName(UINT index, UINT class_id) const {		//根据角色id和技能相对索引(0-29)，得到技能名字
		ASSERT(index < CLASS_SKILL_NAME_SIZE && class_id < CLASS_NAME_SIZE);
		return String(SectionToIndex(CLASS_SKILL_NAME, class_id * CLASS_SKILL_NAME_SIZE + index));
	}
	const CString & ClassSkillTabName(UINT tab, UINT class_id) const {		//根据角色id和技能树索引,得到技能树名字
		ASSERT(tab < CLASS_SKILL_TAB_SIZE && class_id < CLASS_NAME_SIZE);
		return String(SectionToIndex(CLASS_SKILL_TAB_NAME, class_id * CLASS_SKILL_TAB_SIZE + tab));
	}
	const CString & TimeName(UINT index) const { return String(SectionToIndex(TIME_NAME, index)); }
	const CString & ClassName(UINT index) const { return String(SectionToIndex(CLASS_NAME, index)); }
	const CString & ItemName(UINT index) const { return String(SectionToIndex(ITEM_NAME, index)); }
	const CString & PropertyName(UINT index) const { return String(SectionToIndex(PROPERTY_NAME, index)); }
	const CString & MonsterName(UINT index) const { return String(SectionToIndex(MONSTER_NAME, index)); }
	const CString & MagicPrefix(UINT index) const { return String(SectionToIndex(MAGIC_PREFIX, index)); }
	const CString & MagicSuffix(UINT index) const { return String(SectionToIndex(MAGIC_SUFFIX, index)); }
	const CString & RareCraftedName(UINT index) const { return String(SectionToIndex(RARE_CRAFTED_NAME, index)); }
	const CString & UniqueName(UINT index) const { return String(SectionToIndex(UNIQUE_ITEM_NAME, index)); }
	const CString & SetItemName(UINT index) const { return String(SectionToIndex(SET_ITEM_NAME, index)); }
	const CString & RuneWordName(UINT index) const { return String(SectionToIndex(RUNE_WORD_NAME, index)); }
	UINT ItemQualityNameSize() const { return SectionSize(ITEM_QUALITY_NAME); }
	UINT PropertyNameSize() const { return SectionSize(PROPERTY_NAME); }
	UINT TimeNameSize() const { return SectionSize(TIME_NAME); }
	UINT DifficultyNameSize() const { return SectionSize(DIFFICULTY_NAME); }
	UINT MagicPrefixSize() const { return SectionSize(MAGIC_PREFIX); }
	UINT MagicSuffixSize() const { return SectionSize(MAGIC_SUFFIX); }
	UINT RareCraftedNameSize() const { return SectionSize(RARE_CRAFTED_NAME); }
	UINT UniqueNameSize() const { return SectionSize(UNIQUE_ITEM_NAME); }
	UINT SetItemNameSize() const { return SectionSize(SET_ITEM_NAME); }
	UINT MonsterNameSize() const { return SectionSize(MONSTER_NAME); }
	UINT MercenaryTypeNameSize() const { return SectionSize(MERCENARY_TYPE_NAME); }
	UINT MercenaryNameScoutSize() const { return SectionSize(MERCENARY_NAME_SCOUT); }
	UINT MercenaryNameMercSize() const { return SectionSize(MERCENARY_NAME_MERC); }
	UINT MercenaryNameSorcerorSize() const { return SectionSize(MERCENARY_NAME_SOR); }
	UINT MercenaryNameBarbarianSize() const { return SectionSize(MERCENARY_NAME_BAR); }

	const std::vector<std::vector<CItemMetaData>> ItemCatagories() const { return m_vItemMetaData; }
	const CItemMetaData * ItemMetaData(DWORD typeID) const;	//如果没有,返回0
	const CPropertyMetaDataItem& PropertyMetaData(DWORD version, UINT index) const {
		ASSERT(index < m_vPropertyMetaData.size());
		return m_vPropertyMetaData[index].findData(version);
	}
	const CD2S_Struct & NewCharacter() const { return m_stNewCharacter; }	//返回新建人物模板
private:
	CString m_sAppPath;			//程序运行的目录
	std::vector<std::vector<CString>> m_saLanguage;		//语言数据
	std::vector<UINT> m_aLangBases;
	enum LangSection {
		DUMMY_START,
		RUNE_WORD_NAME,			//符文之语
		SET_ITEM_NAME,			//套装名字
		UNIQUE_ITEM_NAME,		//暗金装备名字
		RARE_CRAFTED_NAME,		//Rare和Crafted装备名字
		MAGIC_SUFFIX,			//魔法后缀
		MAGIC_PREFIX,			//魔法前缀
		MONSTER_NAME,			//怪物名称
		PROPERTY_NAME,			//属性名称
		ITEM_NAME,				//物品名称
		CLASS_NAME,				//角色名字
		TIME_NAME,				//时刻名字
		CLASS_SKILL_TAB_NAME,	//角色技能树名字（不是技能名）
		CLASS_SKILL_NAME,		//角色技能名字
		NORMAL_SKILL_NAME,		//通用技能名字
		QUEST_NAME,				//任务名字
		WAY_POINT_NAME,			//小站名字
		DIFFICULTY_NAME,		//游戏难度
		ITEM_QUALITY_NAME,		//物品质量名字
		ITEM_CATAGORY_NAME,		//物品种类名字
		MERCENARY_TYPE_NAME,	//雇佣兵类型名字
		MERCENARY_NAME_SCOUT,	//Rogue Scouts类型雇佣兵名字
		MERCENARY_NAME_MERC,	//Desert Mercenary类型雇佣兵名字
		MERCENARY_NAME_SOR,		//Eastern Sorceror类型雇佣兵名字
		MERCENARY_NAME_BAR,		//Barbarian类型雇佣兵名字
		MSG_BOX_INFO,			//对话框消息
		CHAR_BASIC_INFO_UI,		//角色基本信息UI
		CHAR_ITEMS_UI,			//角色装备信息UI
		ITEM_SUSPEND_UI,		//物品属性悬浮窗口UI
		FOUNDRY_UI,				//铸造台窗口UI
		NEW_ITEM_UI,			//创建物品窗口UI
		OTHER_UI,				//其他窗口UI
		SYSTEM_MENU,			//系统菜单
		CHAR_ITEM_POPUP_MENU,	//角色装备UI弹出菜单
		MENU_PROMPT,			//菜单项的提示信息
		OTHER_MSG,				//其他
	};
	std::vector<std::vector<CItemMetaData>> m_vItemMetaData;			//物品元信息，根据物品种类分不同Section
	std::unordered_map<DWORD, std::pair<UINT, UINT>> m_mIdToMetaData;	//物品TypeID到m_vItemMetaData的索引映射关系
	std::vector<CPropertyMetaData> m_vPropertyMetaData;					//属性数据
	CD2S_Struct m_stNewCharacter;		//新建人物模板
	//Functions
	BOOL ReadLangRes();		//从资源文件里读取语言数据
	BOOL ReadItemRes();		//从资源文件里读取物品数据
	BOOL ReadPropRes();		//从资源文件里读取属性数据
	BOOL ReadNewChar();		//从资源里读取新建人物模板
	const CString & String(UINT index) const {
		ASSERT(index < m_saLanguage[m_nLangIndex].size());
		return m_saLanguage[m_nLangIndex][index];
	}
	UINT SectionSize(LangSection section) const {
		ASSERT(m_aLangBases[section - 1] >= m_aLangBases[section]);
		return m_aLangBases[section - 1] - m_aLangBases[section];
	}
	UINT SectionToIndex(LangSection section, UINT index) const {
		ASSERT(index < SectionSize(section));
		return index + m_aLangBases[section];
	}
};

extern CDiabloEdit2App theApp;
