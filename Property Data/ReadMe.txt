属性的额外信息,包括:
ID		WORD	属性ID
Save Bits	BYTE	参数长度
Save Add	BYTE	参数应减去的值

数据文件格式:
类型		字节数		说明
头信息
WORD		2		Majic,0x11DE
BYTE[4]		4		"PROP",属性文件标志
DWORD		4		文件字节长度
DWORD		4		CRC校验和
WORD		2		属性ID最大值

属性1
STRUCT				属性1,结构如前所示
属性2
...

注意:
文件最后要用CRC进行加密,所以不能在2进制下进行读或写