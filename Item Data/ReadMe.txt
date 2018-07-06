物品的额外信息，包括：
ID	DWORD或BYTE[4],物品唯一ID,如"elx "
Pic	WORD,物品在Bitmap资源中相对第一个图片的索引偏移,
Name	WORD,物品在String列表中的名字索引
EqRange	BYTE,分配如下
	Equip	4 bits,物品可穿戴的位置,
		0	不可穿戴
		1	头
		2	项链
		3	身体
		4	武器或盾
		5	戒指
		6	腰
		7	鞋子
		8	手套
		9	放在腰带里
	Range,4bits物品本身占用网格大小,((col-1) << 2) + (row-1)

Add	BYTE,其他,分配如下:
	bDef		1 bit,有防御值
	bDur		1 bit,有耐久度
	bStack		1 bit,有数量
	bTome		1 bit,是回城书或辨识书,有iTome(5 bits)域
	bMonster	1 bit,有wMonsterID(10 bits)域
	bCharm		1 bit,是护身符(放在身上加属性的),有wCharm域
	bSpell		1 bit,有bSpellID域
	bUnique		1 bit,是否是Unique物品(在没有iQuality域或iQuality值为其他值情况下)
Dmg1Min	BYTE,武器的单手最低伤害
Dmg1Max	BYTE,武器的单手最高伤害
Dmg2Min	BYTE,武器的单手最低伤害
Dmg2Max	BYTE,武器的单手最高伤害
	
	

文件格式:
类型		字节数		说明
头信息
WORD		2		Majic,0x11DE
BYTE[4]		4		"ITEM",物品文件标志
DWORD		4		文件字节长度
DWORD		4		CRC校验和
WORD		2		物品个数
物品1
STRUCT				物品1,结构如前所示
物品2
...

注意:
文件最后要用CRC进行加密,所以不能在2进制下进行读或写