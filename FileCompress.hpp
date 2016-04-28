#include "HuffmanTree.hpp"
#include<string>
#include <algorithm>  //STL

typedef long Type;

#define _DEBUG_

struct FileInfo
{
	unsigned _ch;   //字符
	Type   _count;  //字符出现次数
	string _code;   //字符的Huffman编码

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

// 1.打开文件
// 2.统计文件字符出现的字数
// 3.生成对应的Huffman编码
// 4.压缩文件
// 5.写配置文件
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
/////////////////////////////////压缩//////////////////////////////////
	bool Compress(const char* filename)
	{
		assert(filename);
/////////////////////////////打开文件/////////////////////////////////
		FILE* fOut = fopen(filename, "rb");
		assert(fOut);
////////////////////////////读取文件中的字符/////////////////////////
		char ch = fgetc(fOut);
		while (ch != EOF)
		{
			_infos[(unsigned char)ch]._count++;
			ch = fgetc(fOut);
		}
///////////////////////////生成哈夫曼编码//////////////////////////
		HuffmanTree<FileInfo> ht;
		FileInfo invaild;
		ht.CreateTree(_infos, 256, invaild);
		_GenerateHuffmanCode(ht.GetRootNode());
////////////////////////////压缩文件///////////////////////////////
		string compressFile = filename;
		compressFile += ".compress";
		FILE* fIn = fopen(compressFile.c_str(), "w");
		assert(fIn);

		fseek(fOut, 0, SEEK_SET);
		ch = fgetc(fOut);
///////////////////////////压缩核心-用位存代替字节存///////////////
		//1.某位为1，就把code或非1，之后每次把inch左移一位
		//2.把所有字符的哈夫曼编码以位为单位存入了inch
		//3.每次把index加1，每当写满一个字节就将index和inch置0，写入新的字节
		//4.所有字符都存好后若最后一个字节未存满，则用0把最后一个字节补满，并把补0信息写入配置文件
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
/////////////////////////////配置文件///////////////////////////////
		//1.记录字符和字符出现次数
		//2.记录最后补齐0的个数
		//3.前两行存数，第一行存高32位，第二行存低32位
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
////////////////////////////////解压缩/////////////////////////////////
	bool DisCompress(const char* filename)
	{
///////////////////////////////读取配置文件/////////////////////////////
		string conFile = filename;
		conFile += ".comfig";
		FILE* fRConfig = fopen(conFile.c_str(), "w");
		assert(fRConfig);



	}
public:
/////////////////////////////生成哈夫曼编码////////////////////////////
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
			reverse(code.begin(), code.end());//把反的编码正过来
		}
	}
	////////////////////以行读配置文件/////////////////////////////
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