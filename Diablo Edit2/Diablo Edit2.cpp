
// Diablo Edit2.cpp: 定义应用程序的类行为。
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "Diablo Edit2.h"
#include "MainFrm.h"

#include "Diablo Edit2Doc.h"
#include "Diablo Edit2View.h"

#include "../Generate Data/compress_quicklz.h"

#include <string>
#include <fstream>
#include <sstream>
#include <streambuf>
#include <cctype>
#include <numeric>

using namespace std;

CString DecodeUTF8Name(const std::string& name) 
{
	int len = MultiByteToWideChar(CP_UTF8, 0, (LPCCH)name.c_str(), -1, NULL, 0);
	if (len <= 0) return L"";
	CString ret;
	wchar_t* buf = ret.GetBuffer(len);
	if (buf) MultiByteToWideChar(CP_UTF8, 0, (LPCCH)name.c_str(), -1, buf, len);
	ret.ReleaseBuffer();
	return ret;
}

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CDiabloEdit2App

BEGIN_MESSAGE_MAP(CDiabloEdit2App, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &CDiabloEdit2App::OnAppAbout)
	// 基于文件的标准文档命令
//	ON_COMMAND(ID_FILE_NEW, &CWinApp::OnFileNew)
//	ON_COMMAND(ID_FILE_OPEN, &CWinApp::OnFileOpen)
END_MESSAGE_MAP()

const int CDiabloEdit2App::CLASS_SKILL_INDEX[CLASS_NAME_SIZE][CLASS_SKILL_NAME_SIZE] = {
	{0,1,10,11,20,2,3,12,21,22,4,13,14,23,24,5,6,15,25,26,7,8,16,17,27,9,18,19,28,29},	//Amazon
	{0,1,10,20,21,2,11,12,22,23,3,4,13,14,24,5,6,15,16,25,7,17,18,26,27,8,9,19,28,29},	//Sorceress
	{0,10,11,20,21,1,2,12,13,22,3,4,14,23,24,5,6,15,16,25,7,8,17,26,27,9,18,19,28,29},	//Necromancer
	{0,1,10,20,21,2,11,12,22,23,3,4,13,24,25,5,6,14,15,26,7,8,16,17,27,9,18,19,28,29},	//Paladin
	{0,10,11,12,20,21,1,2,13,14,15,22,23,3,4,16,24,5,6,17,25,7,18,26,27,8,9,19,28,29},	//Barbarian
	{0,1,10,11,20,2,3,12,21,22,4,13,14,23,24,5,6,15,16,25,7,17,18,26,27,8,9,19,28,29},	//Druid
	{0,10,11,20,21,1,2,12,22,23,3,4,13,14,24,5,15,16,25,26,6,7,17,27,28,8,9,18,19,29}	//Assassin
};

// CDiabloEdit2App 构造

CDiabloEdit2App::CDiabloEdit2App() noexcept
{
	// 支持重新启动管理器
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;
#ifdef _MANAGED
	// 如果应用程序是利用公共语言运行时支持(/clr)构建的，则: 
	//     1) 必须有此附加设置，“重新启动管理器”支持才能正常工作。
	//     2) 在您的项目中，您必须按照生成顺序向 System.Windows.Forms 添加引用。
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// TODO: 将以下应用程序 ID 字符串替换为唯一的 ID 字符串；建议的字符串格式
	//为 CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("DiabloEdit2.AppID.NoVersion"));

	//根据系统语言选择默认语言
	//https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-lcid/70feba9f-294e-491e-b6eb-56532684c37f
	LANGID langId = GetSystemDefaultLangID();
	switch (langId) {
		case 0x0004:
		case 0x7804:
		case 0x0804:
		case 0x1004: m_nLangIndex = 1; break;	// Simplified Chinese
		case 0x7c04:
		case 0x0c04:
		case 0x1404:
		case 0x0404: m_nLangIndex = 2; break;	// Traditional Chinese
		default: m_nLangIndex = 0;	// English
	}

	// 将所有重要的初始化放置在 InitInstance 中
}

// 唯一的 CDiabloEdit2App 对象

CDiabloEdit2App theApp;

static PTCHAR ExtractFilePath(PTCHAR pszFilePathName){
	PTCHAR cur = pszFilePathName,last = 0;
	for(;*cur != 0;++cur)
		if(*cur == _T('\\') || *cur == _T('/') || *cur == _T(':'))
			last = cur;
	if(last)
		*++last = 0;
	return pszFilePathName;
}

// CDiabloEdit2App 初始化

BOOL CDiabloEdit2App::InitInstance()
{
	if (!ReadLangRes()) {
		::MessageBox(0, _T("Load language resource file failed."), _T("Error"), MB_OK | MB_ICONERROR);
		return FALSE;
	}
	if (!ReadItemRes()) {
		::MessageBox(0, _T("Load item resource file failed."), _T("Error"), MB_OK | MB_ICONERROR);
		return FALSE;
	}
	if (!ReadPropRes()) {
		::MessageBox(0, _T("Load property resource file failed."), _T("Error"), MB_OK | MB_ICONERROR);
		return FALSE;
	}
	if (!ReadNewChar()) {
		::MessageBox(0, _T("Load character resource failed."), _T("Error"), MB_OK | MB_ICONERROR);
		return FALSE;
	}
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。  否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	// 初始化 OLE 库
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	EnableTaskbarInteraction(FALSE);

	// 使用 RichEdit 控件需要 AfxInitRichEdit2()
	// AfxInitRichEdit2();

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));
	LoadStdProfileSettings(4);  // 加载标准 INI 文件选项(包括 MRU)


	// 注册应用程序的文档模板。  文档模板
	// 将用作文档、框架窗口和视图之间的连接
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CDiabloEdit2Doc),
		RUNTIME_CLASS(CMainFrame),       // 主 SDI 框架窗口
		RUNTIME_CLASS(CDiabloEdit2View));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);


	// 分析标准 shell 命令、DDE、打开文件操作的命令行
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);



	// 调度在命令行中指定的命令。  如果
	// 用 /RegServer、/Register、/Unregserver 或 /Unregister 启动应用程序，则返回 FALSE。
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// 唯一的一个窗口已初始化，因此显示它并对其进行更新
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	// 仅当具有后缀时才调用 DragAcceptFiles
	//  在 SDI 应用程序中，这应在 ProcessShellCommand  之后发生

	//得到程序的运行目录路径
	TCHAR path[1024];
	::GetModuleFileName(0,path,1024);
	m_sAppPath = ExtractFilePath(path);

	return TRUE;
}

int CDiabloEdit2App::ExitInstance() {
	//TODO: 处理可能已添加的附加资源
	AfxOleTerm(FALSE);

	return CWinApp::ExitInstance();
}

template<typename T,class Array>
static inline T & Interpret(Array & p,int i){
	return *reinterpret_cast<T *>(&p[i]);
}

static string readDataFile(const char * input, const char magic[4]) {
	ASSERT(input && magic);
	// Read input file
	ifstream inf(input, ios_base::binary);
	if (!inf.is_open())
		return string();
	string in_buf;
	in_buf.assign(istreambuf_iterator<char>(inf), istreambuf_iterator<char>());
	inf.close();
	// Decompress data
	string out_buf;
	if (!CCompressorQuickLZ().decompress(in_buf, out_buf))
		return string();
	// Check data
	if (out_buf.size() < 5)
		return string();
	if (out_buf.substr(1, 4) != magic)
		return string();
	return out_buf;
}

template<typename T>
static bool parse(istream & is, T & value) {
	if (!is.good())
		return false;
	string s;
	getline(is, s, '\t');
	value = (s.empty() ? 0 : stoi(s));
	return !is.bad();
}

static bool parse(istream & is, string & str) {
	if (is.good())
		getline(is, str, '\t');
	return !is.bad();
}

static bool parse(istream & is, vector<string> & strs) {
	if (is.good())
		for (string s; getline(is, s, '\t'); strs.push_back(s));
	return !is.bad();
}

//Escape "\\" -> '\' and "\n" -> NewLine.
static string escape(string str) {
	string ret;
	size_t pp = 0;
	for (size_t p = 0;;) {
		auto i = str.find_first_of('\\', p);
		if (i == string::npos)
			break;
		p = i + 1;
		if (p < str.size()) {
			const char c = str[p], n = (c == 'n' ? '\n' : (c == '\\' ? '\\' : 0));
			if (n) {
				ret.append(str.substr(pp, i - pp));
				ret.push_back(n);
				pp = ++p;
			}
		}
	}
	ret.append(str.substr(pp));
	return ret;
}

static void compare(const vector<CString> & expected, const vector<CString> & lang) {
	for (size_t i = 0, j = 0; i < lang.size() || j < expected.size();) {
		if (expected[j].IsEmpty())
			++j;
		else if (lang[i] != expected[j] && i != 371)
			throw 0;
		else
			++i, ++j;
	}
}

static string trim(const string & str) {
	size_t i = 0;
	for (; i < str.length() && isspace(str[i]); ++i);
	return (i ? str.substr(i) : str);
}

static tuple<LPVOID, size_t, HGLOBAL> loadBinaryResource(int id) {
	HRSRC hRsrc = FindResource(NULL, MAKEINTRESOURCEW(id), _T("Binary"));
	if (!hRsrc)
		return make_tuple(nullptr, 0, nullptr);
	DWORD dwSize = SizeofResource(NULL, hRsrc);
	if (!dwSize)
		return make_tuple(nullptr, 0, nullptr);
	HGLOBAL hGlobal = LoadResource(NULL, hRsrc);
	if (!hGlobal)
		return make_tuple(nullptr, 0, nullptr);
	LPVOID pBuffer = LockResource(hGlobal);
	if (!pBuffer)
		return make_tuple(nullptr, 0, nullptr);
	return make_tuple(pBuffer, dwSize, hGlobal);
}

static string loadCompressedBinaryResource(int id, const char magic[4]) {
	ASSERT(magic);
	//Load resource
	auto t = loadBinaryResource(id);
	auto buf = reinterpret_cast<const char *>(get<0>(t));
	if (!buf)
		return string();
	string in_buf(buf, get<1>(t));
	// Decompress data
	string out_buf;
	if (!CCompressorQuickLZ().decompress(in_buf, out_buf))
		return string();
	// Check data
	if (out_buf.size() < 5)
		return string();
	if (out_buf.substr(1, 4) != magic)
		return string();
	return out_buf;
}

BOOL CDiabloEdit2App::ReadNewChar() {
	auto t = loadBinaryResource(IDR_BINARY1);
	if (!get<0>(t))
		return FALSE;
	CInBitsStream bs(reinterpret_cast<const BYTE *>(get<0>(t)), get<1>(t));
	m_stNewCharacter.ReadData(bs);
	FreeResource(get<2>(t));
	return (bs.Good() ? TRUE : FALSE);
}

BOOL CDiabloEdit2App::ReadLangRes(void) {
	// Load resource
	string out_buf = loadCompressedBinaryResource(IDR_BINARY4, "LANG");
	if (out_buf.empty())
		return FALSE;
	// Deserialize data
	istringstream iss(out_buf);
	decltype(m_saLanguage) langs;
	decltype(m_aLangBases) bases;
	UINT base = 0, idx = 0;
	for (string line; getline(iss, line);) {
		if (trim(line).empty())	//空行
			continue;
		else if (line[0] == '*') {	// New section
			base = (langs.empty() ? 0 : langs[0].size());
			idx = 0;
			if(base)
				bases.push_back(base);
			continue;
		}
 		istringstream ls(line);
		UINT gidx;
		vector<string> msgs;
		parse(ls, gidx) && parse(ls, msgs);
		if (!gidx)
			gidx = idx++;
		else
			idx = gidx + 1;
		gidx += base;
		if (msgs.size() > langs.size())
			langs.resize(msgs.size());
		for (size_t i = 0; i < msgs.size(); ++i) {
			auto & lang = langs[i];
			auto & msg = msgs[i];
			// Microsoft Excel adds "" to msg sometimes.
			if (msg.size() > 2 && msg[0] == '"' && *msg.rbegin() == '"')
				msg = msg.substr(1, msg.size() - 2);
			msg = escape(msg);
			if (gidx < lang.size()) {
				if (!lang[gidx].IsEmpty())
					throw 0;
				lang[gidx] = msg.c_str();
			} else {
				if (gidx > lang.size())
					lang.resize(gidx);
				lang.emplace_back(DecodeUTF8Name(msg));
			}
		}
	}
	if(!langs.empty())
		bases.push_back(langs[0].size());
	//Decorate
	for (size_t i = 1; i < langs.size(); ++i) {
		auto & lang = langs[i];
		const auto & prev = langs[i - 1];
		if (lang.size() < prev.size())
			lang.resize(prev.size());
		ASSERT(lang.size() == prev.size());
		for (size_t j = 0; j < lang.size(); ++j)
			if (lang[j].IsEmpty())
				lang[j] = prev[j];
	}
	reverse(bases.begin(), bases.end());
	bases.push_back(0);		//OTHER_MSG
	langs.swap(m_saLanguage);
	bases.swap(m_aLangBases);
	return TRUE;
}

BOOL CDiabloEdit2App::ReadItemRes(void) {
	// Load resource
	string out_buf = loadCompressedBinaryResource(IDR_BINARY3, "ITEM");
	if (out_buf.empty())
		return FALSE;
	// Deserialize data
	istringstream iss(out_buf);
	int picOff = 0;
	decltype(m_vItemMetaData) sections;
	sections.reserve(30);
	decltype(m_mIdToMetaData) idMap;
	for (string line; getline(iss, line);) {
		line = trim(line);
		if (line.empty() || line.substr(0, 5) == "*ITEM")	//空行或Magic
			continue;
		else if (line[0] == '*') {	//new section
			sections.emplace_back();
			continue;
		}
		istringstream ls(line);
		string id;
		int pic = 0, range = 0;
		CItemMetaData item;
		parse(ls, id)
			&& parse(ls, pic)
			&& parse(ls, item.NameIndex)
			&& parse(ls, range)
			&& parse(ls, item.Equip)
			&& parse(ls, item.Simple)
			&& parse(ls, item.Normal)
			&& parse(ls, item.White)
			&& parse(ls, item.HasDef)
			&& parse(ls, item.HasDur)
			&& parse(ls, item.IsStacked)
			&& parse(ls, item.HasMonsterID)
			&& parse(ls, item.IsCharm)
			&& parse(ls, item.SpellId)
			&& parse(ls, item.IsUnique)
			&& parse(ls, item.IsCraft)
			&& parse(ls, item.IsGem)
			&& parse(ls, item.Damage1Min)
			&& parse(ls, item.Damage1Max)
			&& parse(ls, item.Damage2Min)
			&& parse(ls, item.Damage2Max)
			&& parse(ls, item.Pad);
		// ID
		if (id.size() != 3)
			return FALSE;
		id.resize(4, ' ');
		memcpy(item.sTypeName, id.c_str(), size(item.sTypeName));
		// Pic
		if (pic < 0)
			picOff += pic;
		item.PicIndex = (pic > 0 ? pic : WORD(idMap.size() + picOff));
		// Name
		if (!item.NameIndex)
			item.NameIndex = WORD(idMap.size());
		// Range
		item.Range = (range ? ((range / 10) << 4) + range % 10 : 0x11);
		// store
		sections.back().push_back(item);
		idMap[item.dwTypeID] = make_pair(sections.size() - 1, sections.back().size() - 1);
	}
	sections.swap(m_vItemMetaData);
	idMap.swap(m_mIdToMetaData);
	return TRUE;
}

BOOL CDiabloEdit2App::ReadPropRes() {
	// Load resource
	string out_buf = loadCompressedBinaryResource(IDR_BINARY2, "PROP");
	if (out_buf.empty())
		return FALSE;
	// Deserialize data
	istringstream iss(out_buf);
	decltype(m_vPropertyMetaData) props;
	for (string line; getline(iss, line);) {
		line = trim(line);
		if (line.empty() || line[0] == '*')	//空行或注释
			continue;
		istringstream ls(line);
		int verMin = 0, id = -1, def = 0;
		vector<CPropertyField> fields;
		parse(ls, id) && parse(ls, verMin) && parse(ls, def);
		if (id < 0)
			return FALSE;
		for (CPropertyField f;; fields.push_back(f.Normalize())) {
			f.bits = f.base = f.min = f.max = 0;
			if (!(parse(ls, f.bits) && parse(ls, f.base) && parse(ls, f.min) && parse(ls, f.max)))
				break;
		}
		if (id >= int(props.size()))
			props.resize(id + 1);
		props[id].addData(CPropertyMetaDataItem(verMin, fields, def));
	}
	props.swap(m_vPropertyMetaData);
	return TRUE;
}

CString CDiabloEdit2App::PropertyDescription(DWORD version, WORD id, DWORD value) const {
	const auto & meta = PropertyMetaData(version, id);
	const auto desc = PropertyName(id);
	auto a = meta.Parse(value);
	ASSERT(3 < a.size());
	switch (id) {
		case 54:a[2] /= 25; break;
		case 56:case 59:a[0] /= 25; break;
		case 57:a[0] = (a[0] * a[2] + 128) >> 8; a[1] = (a[1] * a[2] + 128) >> 8; a[2] /= 25; break;
		case 83:return CSFormat(desc, a[1], ClassName(a[0]));
		case 97:case 151:return CSFormat(desc, a[1], ClassSkillName(a[0]).first);
		case 98:return CSFormat(desc, (1 <= a[0] && a[0] <= 5 ? Msg(4 + a[0]) : Msg(10)));
		case 107:
		{
			const auto p = ClassSkillName(a[0]);
			return CSFormat(desc, a[1], p.first, ClassName(p.second));
		}
		case 112:a[0] = a[0] * 100 / 127; break;
		case 126:swap(a[0], a[1]); break;
		case 155:case 179:case 180:return CSFormat(desc, a[1], MonsterName(a[0]));
		case 188:return CSFormat(desc, a[2], ClassSkillTabName(a[0], a[1]), ClassName(a[1]));
		case 195:case 196:case 197:case 198:case 199:case 201:
			return CSFormat(desc, a[2], a[0], ClassSkillName(a[1]).first);
		case 204:return CSFormat(desc, a[0], ClassSkillName(a[1]).first, a[2], a[3]);
		case 214:case 215:case 216:case 217:case 218:case 219:case 220:case 221:case 222:case 223:
		case 226:case 227:case 228:case 229:case 230:case 231:case 232:case 233:case 234:case 235:
		case 236:case 237:case 238:case 239:case 240:case 241:case 242:case 243:case 244:case 245:
		case 246:case 247:case 248:case 249:case 250:
			return CSFormat(desc, double(a[0]) / 8);
		case 224:case 225:return CSFormat(desc, double(a[0]) / 2);
		case 252:case 253:a[0] = 100 / a[0]; break;
		case 268:case 269:case 270:case 271:case 272:case 273:case 274:case 275:case 276:case 277:
		case 278:case 279:case 280:case 281:case 282:case 283:case 284:case 285:case 286:case 287:
		case 288:case 289:case 290:case 291:case 292:case 293:case 294:case 295:case 296:case 297:
		case 298:case 299:case 300:case 301:case 302:case 303:
			return CSFormat(desc, a[1], a[2], TimeName(a[0]));
		case 305:case 306:case 307:case 308:case 333:case 334:case 335:case 336:
			a[0] = -a[0]; break;
	}
	return CSFormat(desc, a[0], a[1], a[2], a[3]);
}

CString CDiabloEdit2App::PropertyDescription(DWORD version, WORD id) const {
	return PropertyDescription(version, id, PropertyMetaData(version, id).DefaultValue());
}

vector<CPropParam> CDiabloEdit2App::PropertyParameters(DWORD version, WORD id, DWORD value) const {
	const auto & meta = PropertyMetaData(version, id);
	vector<CPropParam> ret;
	for (auto & t : meta.GetParams(value))
		ret.push_back({ get<0>(t), get<1>(t), get<2>(t) });
	return ret;
}

//角色技能id范围：[6, 156)为亚马逊，法师，死灵，圣骑士，野蛮人技能，[221, 281)为德鲁伊，刺客技能。
//普通技能id范围：[0, 6), [217, 221), 350(Delirium)
//其他id都是：一股邪恶力量
pair<CString, int> CDiabloEdit2App::ClassSkillName(UINT skill_id) const {
	if (skill_id < 6)
		return make_pair(NormalSkillName(skill_id), -1);
	else if (skill_id < 156) {
		skill_id -= 6;
		const int cid = skill_id / 30, index = CLASS_SKILL_INDEX[cid][skill_id % 30];
		return make_pair(ClassSkillName(index, cid), cid);
	} else if (217 <= skill_id && skill_id < 221)
		return make_pair(NormalSkillName(skill_id - 211), -1);
	else if (221 <= skill_id && skill_id < 281) {
		skill_id -= 221;
		const int cid = skill_id / 30 + 5, index = CLASS_SKILL_INDEX[cid][skill_id % 30];
		return make_pair(ClassSkillName(index, cid), cid);
	}else if(350 == skill_id)
		return make_pair(NormalSkillName(10), -1);
	return make_pair(NormalSkillName(11), -1);
}

const CItemMetaData * CDiabloEdit2App::ItemMetaData(DWORD typeID) const {
	const auto wh = m_mIdToMetaData.find(typeID);
	if (wh == m_mIdToMetaData.end())
		return 0;
	auto & p = wh->second;
	ASSERT(p.first < m_vItemMetaData.size());
	auto & s = m_vItemMetaData[p.first];
	ASSERT(p.second < s.size());
	return &s[p.second];
}

// CDiabloEdit2App 消息处理程序


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg() noexcept;

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() noexcept : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// 用于运行对话框的应用程序命令
void CDiabloEdit2App::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CDiabloEdit2App 消息处理程序



