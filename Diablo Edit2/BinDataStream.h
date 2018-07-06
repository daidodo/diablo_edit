#pragma once

#include <vector>

class CBinDataStream
{
	DWORD byte_pos,bit_pos;
	std::vector<BYTE>	data_;
public:
	CBinDataStream():byte_pos(0),bit_pos(0){}
	~CBinDataStream(){}
	DWORD BytePos() const{return byte_pos;}
	void ReadFile(CFile & cf){
		data_.resize(size_t(cf.GetLength()));
		cf.Read(&data_[0],UINT(cf.GetLength()));
	}
	void WriteFile(CFile & cf){
		ASSERT(data_.size() && _T("CBinDataStream::WriteFile(CFile & cf)"));
		cf.Write(&data_[0],UINT(data_.size()));
		cf.Flush();
	}
	//字节读取
	CBinDataStream & operator >>(DWORD & value){
		ASSERT(!bit_pos && _T("CBinDataStream::operator >>(DWORD & value)"));
		value = *reinterpret_cast<DWORD *>(&data_[byte_pos]);
		byte_pos += 4;
		return *this;
	}
	CBinDataStream & operator >>(WORD & value){
		ASSERT(!bit_pos && _T("CBinDataStream::operator >>(WORD & value)"));
		value = *reinterpret_cast<WORD *>(&data_[byte_pos]);
		byte_pos += 2;
		return *this;
	}
	CBinDataStream & operator >>(BYTE & value){
		ASSERT(!bit_pos && _T("CBinDataStream::operator >>(BYTE & value)"));
		value = data_[byte_pos++];
		return *this;
	}
	template<int N,typename T>
	CBinDataStream & operator >>(T (&value)[N]){
		ASSERT(!bit_pos && _T("template<int N,typename T>CBinDataStream::operator >>(T (&value)[N])"));
		for(int i = 0;i < N;++i)
			operator >>(value[i]);
		return *this;
	}
	template<int N>
	CBinDataStream & operator >>(BYTE (&value)[N]){
		ASSERT(!bit_pos && _T("template<int N>CBinDataStream::operator >>(BYTE (&value)[N])"));
		::CopyMemory(value,&data_[byte_pos],N);
		byte_pos += N;
		return *this;
	}
	//位读取
	void AlignByte(){	//按照8位进行对齐，舍弃多余的位
		while(bit_pos)
			if(++bit_pos == 8)
				bit_pos = 0,++byte_pos;
	}
	void ReadBit(BOOL & value){
		value = (data_[byte_pos] >> bit_pos) & 1;
		if(++bit_pos == 8)
			bit_pos = 0,++byte_pos;
	}
	void ReadBits(DWORD & value,int len){
		ASSERT(len <= 32 && _T("CBinDataStream::ReadBits(DWORD & value,int len)"));
		const DWORD HIGHEST_BIT = 0x80000000;
		value = 0;
		for(int i = len;i > 0;--i){
			value >>= 1;
			if((data_[byte_pos] >> bit_pos) & 1)
				value += HIGHEST_BIT;
			if(++bit_pos == 8)
				bit_pos = 0,++byte_pos;
		}
		value >>= (32 - len);
	}
	void ReadBits(WORD & value,int len){
		ASSERT(len <= 16 && _T("CBinDataStream::ReadBits(WORD & value,int len)"));
		const DWORD HIGHEST_BIT = 0x8000;
		value = 0;
		for(int i = len;i > 0;--i){
			value >>= 1;
			if((data_[byte_pos] >> bit_pos) & 1)
				value += HIGHEST_BIT;
			if(++bit_pos == 8)
				bit_pos = 0,++byte_pos;
		}
		value >>= (16 - len);
	}
	void ReadBits(BYTE & value,int len){
		ASSERT(len <= 8 && _T("CBinDataStream::ReadBits(BYTE & value,int len)"));
		const DWORD HIGHEST_BIT = 0x80;
		value = 0;
		for(int i = len;i > 0;--i){
			value >>= 1;
			if((data_[byte_pos] >> bit_pos) & 1)
				value += HIGHEST_BIT;
			if(++bit_pos == 8)
				bit_pos = 0,++byte_pos;
		}
		value >>= (8 - len);
	}
	//vector<BYTE>
	CBinDataStream & operator >>(std::vector<BYTE> & vec){
		ASSERT(!bit_pos && _T("CBinDataStream::operator >>(std::vector<BYTE> & vec)"));
		vec.assign(data_.begin() + byte_pos,data_.end());
		return *this;
	}
	//把不能识别的数据转储
	void Restore(std::vector<BYTE> & vec,DWORD from,DWORD to){
		ASSERT(to > from && _T("CBinDataStream::Restore(std::vector<BYTE> & vec,DWORD from,DWORD to)"));
		vec.assign(&data_[from],&data_[to]);
	}
	//随机定位
	void SeekBack(DWORD back){byte_pos -= back;}
	//调试用，得到后续的2进制位流
	CString GetBitString(DWORD len = 32) const{
		CString ret;
		DWORD byte = byte_pos;
		DWORD bit = bit_pos;
		while(len-- != 0 && byte < data_.size()){
			ret = ((data_[byte] >> bit) & 1 ? _T("1") : _T("0")) + ret;
			if(++bit == 8){
				bit = 0,++byte;
				ret = _T(" ") + ret;
			}
		}
		return ret;
	}
};
