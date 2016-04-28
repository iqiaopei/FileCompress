#include "HuffmanTree.hpp"
#include<string>
#include <algorithm>  //STL

typedef long Type;

#define _DEBUG_

struct FileInfo
{
	unsigned _ch;   //�ַ�
	Type   _count;  //�ַ����ִ���
	string _code;   //�ַ���Huffman����

	FileInfo(unsigned char ch = 0)
		:_ch(ch)
		, _count(0)
	{}

	bool operator<(const FileInfo& info)
	{
		return this->_count < info._count;
	}

	FileInfo operator+(const FileInfo& info)
	{
		FileInfo tmp;
		tmp._count = this->_count + info._count;
		return tmp;
	}

	bool operator != (const FileInfo& info) const
	{
		return this->_count != info._count;
	}
};

// 1.���ļ�
// 2.ͳ���ļ��ַ����ֵ�����
// 3.���ɶ�Ӧ��Huffman����
// 4.ѹ���ļ�
// 5.д�����ļ�
class FileCompress
{
protected:
	FileInfo _infos[256];
public:
	FileCompress()
	{
		for (int i = 0; i < 256; ++i)
		{
			_infos[i]._ch = i;
		}
	}
/////////////////////////////////ѹ��//////////////////////////////////
	bool Compress(const char* filename)
	{
		assert(filename);
/////////////////////////////���ļ�/////////////////////////////////
		FILE* fOut = fopen(filename, "rb");
		assert(fOut);
////////////////////////////��ȡ�ļ��е��ַ�/////////////////////////
		char ch = fgetc(fOut);
		while (ch != EOF)
		{
			_infos[(unsigned char)ch]._count++;
			ch = fgetc(fOut);
		}
///////////////////////////���ɹ���������//////////////////////////
		HuffmanTree<FileInfo> ht;
		FileInfo invaild;
		ht.CreateTree(_infos, 256, invaild);
		_GenerateHuffmanCode(ht.GetRootNode());
////////////////////////////ѹ���ļ�///////////////////////////////
		string compressFile = filename;
		compressFile += ".compress";
		FILE* fIn = fopen(compressFile.c_str(), "w");
		assert(fIn);

		fseek(fOut, 0, SEEK_SET);
		ch = fgetc(fOut);
///////////////////////////ѹ������-��λ������ֽڴ�///////////////
		//1.ĳλΪ1���Ͱ�code���1��֮��ÿ�ΰ�inch����һλ
		//2.�������ַ��Ĺ�����������λΪ��λ������inch
		//3.ÿ�ΰ�index��1��ÿ��д��һ���ֽھͽ�index��inch��0��д���µ��ֽ�
		//4.�����ַ�����ú������һ���ֽ�δ����������0�����һ���ֽڲ��������Ѳ�0��Ϣд�������ļ�
		int index = 0;
		char inCh = 0;

		while (ch != EOF)
		{
			string& code = _infos[(unsigned char)ch]._code;

			for (size_t i = 0; i < code.size(); ++i)
			{                                       
				inCh = inCh << 1;
				if (code[i] == '1')
				{
					inCh |= 1;
				}
				++index;
				if (index == 8)
				{
					fputc(inCh, fIn);
					index = 0;
					inCh = 0;
				}
			}
			ch = fgetc(fOut);
		}
		if (index != 8)
		{
			inCh <<= (8 - index);
			fputc(inCh, fIn);
		}
/////////////////////////////�����ļ�///////////////////////////////
		//1.��¼�ַ����ַ����ִ���
		//2.��¼�����0�ĸ���
		//3.ǰ���д�������һ�д��32λ���ڶ��д��32λ
		char str[128];
		string conFile = filename;
		conFile += ".config";
		FILE* fWConfig = fopen(conFile.c_str(), "w");
		assert(fWConfig);

		for (size_t i = 0; i < 256; ++i)
		{
			string chInfo;
			if (_infos[i]._count > 0)
			{
				chInfo += _infos[i]._ch;
				chInfo += ',';
				chInfo += _itoa(_infos[i]._count,str,128);
				chInfo += '\n';
			}
		}
/////////////////////////////////////////////////////////////
		fclose(fOut);
		fclose(fIn);
		fclose(fWConfig);
		return true;
	}
////////////////////////////////��ѹ��/////////////////////////////////
	bool DisCompress(const char* filename)
	{
///////////////////////////////��ȡ�����ļ�/////////////////////////////
		string conFile = filename;
		conFile += ".comfig";
		FILE* fRConfig = fopen(conFile.c_str(), "w");
		assert(fRConfig);



	}
public:
/////////////////////////////���ɹ���������////////////////////////////
	void _GenerateHuffmanCode(HuffmanTreeNode<FileInfo>* root)
	{
		if (root == NULL)
			return;
		_GenerateHuffmanCode(root->_left);
		_GenerateHuffmanCode(root->_right);

		if (root->_left == NULL && root->_right == NULL)
		{
			HuffmanTreeNode<FileInfo>* cur = root;
			HuffmanTreeNode<FileInfo>* parent = cur->_parent;
			string& code = _infos[cur->_weight._ch]._code;

			while (parent)
			{
				if (parent->_left == cur)
					code += '0';
				else
					code += '1';
				cur = parent;
				parent = cur->_parent;
			}
			reverse(code.begin(), code.end());//�ѷ��ı���������
		}
	}
	////////////////////���ж������ļ�/////////////////////////////
	bool _ReadLine(FILE* configFile, string& line)
	{
		char ch = fgetc(configFile);
		if (ch == EOF)
			return false;
		while (ch != EOF && ch != '\n')
		{
			line += ch;
			ch = fgetc(configFile);
		}
		return true;
	}
};



void TestFileCompress()
{
	FileCompress fc;
	fc.Compress("In.txt");
}