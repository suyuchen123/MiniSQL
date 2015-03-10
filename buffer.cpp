
#include "buffer.h"
#include "CatalogManager.h"
#include <iostream>
#include <string>
#include <fstream>
using namespace std;
//class CatalogManager;
#define UNKNOWN_FILE 8    
#define TABLE_FILE 9      
#define INDEX_FILE 10     

extern CatalogManager catalogM;



buffer::buffer()
{
	 nowBlock=0;  
	 nowFile=0;
	 fileHead = NULL;
	for (int i = 0; i < MAX_FILE_ACTIVE; i++)
	{
		file[i].type = 0;
		file[i].fileName = "";
		file[i].recordAmount = -1;
		file[i].usage = -1;
		file[i].recordLength = -1;
		file[i].next = NULL;
		file[i].firstBlock=-1;
	}
	for (int i = 0; i < MAX_BLOCK; i++)
	{
		//分配文件块数据存储空间，并置0
		block[i].address = new char[BLOCK_LEN];
		memset(block[i].address, 0, BLOCK_LEN);
		//为文件块初始化变量
		block[i].offset = -1;
		block[i].dirty = 0;
		block[i].lock = 0;
		block[i].id = i;
		block[i].lrutime = 0;
		block[i].usage = -1;
		block[i].nextBlock = -1;
		block[i].preBlock = -1;
		block[i].file = NULL;

	}

}

buffer::~buffer()
{
	for (int i = 0; i < MAX_BLOCK; i++)
	{
		if (block[i].file!=NULL && block[i].usage!=-1)
			freeBlock(block[i].file->fileName, i, block[i].dirty);
	}
}

//put the file into list of file and return the number of block it have

int buffer::getNumberOfBlocks(string aimfile)
{
	fileInfo *fp;
	int bp;
	for (fp = fileHead; fp != NULL; fp = fp->next)
	{
		if (fp->fileName == aimfile)  // that file is in memeory then return it
		{
			return ((get_file_size (aimfile.c_str())/ BLOCK_LEN ));
		}
	}

	if (nowFile < MAX_FILE_ACTIVE)
	{
		for (int i = nowFile; i < MAX_FILE_ACTIVE; i++)
		{
			if (file[i].usage == -1)
			{
				fp = &file[i];
				break;
			}
		}
		nowFile++;
		fp->usage = 0;
		fp->fileName = aimfile;
		// put it into the top of the link 
		if (fileHead != NULL)
		{
			fp->next = fileHead;
			fileHead = fp;
		}
		else                  // if filehead is not null set fp in to file head
		{
			fp->next = NULL;
			fileHead = fp;
		}
	}
	else  // nowfile==MAX
	{
		fp = fileHead;
		string oldname = fp->fileName;
		fp->fileName = aimfile; // set the top as the file block
		for (bp = fp->firstBlock; bp != NULL; bp = block[bp].nextBlock)
		{
			block[bp].usage = 0;
			if (freeBlock(oldname,bp,block[bp].dirty)==0)
			{
				cout << "Fail to flush block" << endl;
				return NULL;
			}
			
		}
		fp->firstBlock = NULL;  // the file got no block here.


	}
//        Wait the catalog function to put in
//get information data from dict function
int imformation[3];
catalogM.getfileinfo(aimfile, imformation);       // get the information of the file
//if it is a table

if (imformation[0] == TABLE_FILE)
{
fp->type = TABLE_FILE;
fp->recordAmount = imformation[1];
fp->recordLength = imformation[2];
}
//if index file
else if (imformation[0] == INDEX_FILE)
{
fp->type = INDEX_FILE;
fp->recordLength = -1;
fp->recordAmount = -1;
}
//do not have such file
else
{
cout << "File not found" << endl;
return NULL;
}

	return  (get_file_size(aimfile.c_str()) / BLOCK_LEN);
}



char * buffer::getBlockAddress(string fileName, int blockNo)
{
	// the block is not suit for the file
	if (block[blockNo].file == NULL) return NULL;
	if (block[blockNo].usage == -1 || block[blockNo].file->fileName != fileName)
	{
		return NULL;
	}
	return block[blockNo].address;
}


char * buffer::getoffsetBlockAddress(string fileName, int offset)
{
	blockInfo * LRUBlock;
	int bp;
	fileInfo  *	fp;
	fileInfo  *	temfp;
	LRUBlock = NULL;
	temfp = NULL;

	for (int i = 0; i < MAX_BLOCK; i++)                 // if that block is already in memory
	{
		if (block[i].file == NULL || block[i].usage == -1) continue;
		if (block[i].file->fileName == fileName && block[i].offset == offset)
		{
			return block[i].address;
		}
	}

	// start to find empty one and read

	for (int i = 0; i < MAX_BLOCK; i++)
	{
		if (block[i].usage == -1)
		{
			LRUBlock = &block[i];
			break;
		}
	}

	if (LRUBlock == NULL)            //get an empty block;
	{
		int flag = 100000;
		for (fp = fileHead; fp != NULL; fp = fp->next)
			for (bp = fp->firstBlock; bp != NULL; bp = block[bp].nextBlock)
			{
			if (block[bp].lrutime < flag  && block[bp].lock != 1)
			{
				flag = block[bp].lrutime;
				LRUBlock = &block[bp];
				temfp = fp;
			}
			}

		if (LRUBlock->dirty)
		{
			freeBlock(temfp->fileName, LRUBlock->id, 1);
			LRUBlock->dirty = 0;
			nowBlock++;
		}
	}

	//clear the address
	memset(LRUBlock->address, 0, BLOCK_LEN);

		LRUBlock->offset = offset;
	LRUBlock->dirty = 0;
	LRUBlock->lock = 0;
	LRUBlock->lrutime = 0;
	LRUBlock->usage = 0;
	LRUBlock->nextBlock = -1;
	LRUBlock->preBlock = -1;
	LRUBlock->file = getFile(fileName);


	FILE * fileread;
	blockInfo * bpp;
	bpp = LRUBlock;

	if (fileread = fopen(fileName.c_str(), "rb+"))
	{
		fseek(fileread, bpp->offset*BLOCK_LEN, 0);
		//if read over then  return null 
		if (fread(bpp->address, 1, BLOCK_LEN, fileread) == 0) //if read nothing
		{

			fclose(fileread);
			bpp->offset = 0;
			return NULL;
		}
		fclose(fileread);
	}
	// set position

	block[getFile(fileName)->firstBlock].preBlock = bpp->id;
	LRUBlock->nextBlock = getFile(fileName)->firstBlock;
	getFile(fileName)->firstBlock = bpp->id;
	bpp->usage = findUsage(fileName.c_str(), bpp->address);

	return bpp->address;          //return that id

}


//create new file
void buffer::createNewFile(string aimfile)
{
	fileInfo *fp;
	int bp;
	for (fp = fileHead; fp != NULL; fp = fp->next)
	{
		if (fp->fileName == aimfile)  // that file is in memeory then return 
		{
			return;
		}
	}
	system("pause");
	//create file
	FILE *filetowite;
	
	filetowite = fopen(aimfile.c_str(), "rb+");
	if (filetowite == NULL)
	{
	//then create the file;
		//fclose(filetowite);
		filetowite = fopen(aimfile.c_str(), "w");
		if (filetowite != NULL)
		{
			cout << aimfile << " file has been created"<<endl;
		}
		else
		{
			cout << aimfile << "file create failed"<<endl;
		}
	}
	else
	{
		cout << "The file has exsit" << endl;
	}
	
	//give a place in the list for the file
	if (nowFile < MAX_FILE_ACTIVE)
	{
		for (int i = nowFile; i < MAX_FILE_ACTIVE; i++)
		{
			if (file[i].usage == -1)
			{
				fp = &file[i];
				break;
			}
		}
		nowFile++;
		fp->usage = 0;
		fp->fileName = aimfile;
		// put it into the top of the link 
		if (fileHead != NULL)
		{
			fp->next = fileHead;
			fileHead = fp;
		}
		else                  // if filehead is not null set fp in to file head
		{
			fp->next = NULL;
			fileHead = fp;
		}
	}
	else  // nowfile==MAX
	{
		fp = fileHead;
		string oldname = fp->fileName;
		fp->fileName = aimfile; // set the top as the file block
		for (bp = fp->firstBlock; bp != NULL; bp = block[bp].nextBlock)
		{
			block[bp].usage = 0;
			if (!freeBlock(oldname, bp, block[bp].dirty))
			{
				cout << "Fail to flush block" << endl;
			}
			return ;
		}
		fp->firstBlock = NULL;  // the file got no block here.


	}
	 //       Wait the catalog function to put in
	//get information data from dict function
	int imformation[3];
		catalogM.getfileinfo(aimfile, imformation);       // get the information of the file
	//if it is a table

	if (imformation[0] == TABLE_FILE)
	{
	fp->type = TABLE_FILE;
	fp->recordAmount = imformation[1];
	fp->recordLength = imformation[2];
	}
	//if index file
	else if (imformation[0] == INDEX_FILE)
	{
	fp->type = INDEX_FILE;
	fp->recordLength = -1;
	fp->recordAmount = -1;
	}
	//do not have such file
	else
	{
	cout << "File not found" << endl;
	return ;
	}
	

	return;
}

int buffer::freeBlock(string aimfile, int blockNo, bool isDirty)
{
	//make sure the blook are for the right aimfile
	if (blockNo < 0) return 0;
	if (block[blockNo].file == NULL)
	{
		return 0;
	}
	if (block[blockNo].usage == -1 || block[blockNo].file->fileName != aimfile)
	{
		return 0;
	}

	if (isDirty == 1)
	{
		FILE *fp;
		if (fp = fopen(aimfile.c_str(), "rb+"))
		{
			fseek(fp, block[blockNo].offset*BLOCK_LEN, 0);  //get to the positon of the file
			if (fwrite(block[blockNo].address, BLOCK_LEN, 1, fp) == -1)
			{
				cout << "write erro" << endl;
				return 0; //write earro
			}
			fclose(fp);
		}
		else
		{
			cout << "Fail to open the block of file " << aimfile << endl;
			return 0;
		}
		isDirty = 0;
	}
	/*
	if (block[blockNo].preBlock > 0)
	{
	block[block[blockNo].preBlock].nextBlock = block[blockNo].nextBlock;
	}

	if (block[blockNo].nextBlock > 0)
	{
	block[block[blockNo].nextBlock].preBlock = block[blockNo].preBlock;
	}

	block[blockNo].usage = -1;
	nowBlock--;*/

	return 1;
}



int buffer::freeBlock_static(string aimfile, int static_block, bool isDirty)    
// static_block--->the offset of the file ,first find the block in memory
{
	int blockNo;
	for (int i = 0; i < MAX_BLOCK; i++)                 // if that block is already in memory
	{
		if (block[i].file == NULL || block[i].usage == -1) continue;
		if (block[i].file->fileName == aimfile && block[i].offset == static_block)
		{
			blockNo = i;
		}
	}



	//make sure the blook are for the right aimfile
	if (block[blockNo].file == NULL)
	{
		return 0;
	}
	if (block[blockNo].usage == -1 || block[blockNo].file->fileName != aimfile)
	{
		return 0;
	}

	if (isDirty == 1)
	{
		FILE *fp;
		if (fp = fopen(aimfile.c_str(), "rb+"))
		{
			fseek(fp, block[blockNo].offset*BLOCK_LEN, 0);  //get to the positon of the file
			if (fwrite(block[blockNo].address, BLOCK_LEN, 1, fp) == -1)
			{
				cout << "write erro" << endl;
				return 0; //write earro
			}
			fclose(fp);
		}
		else
		{
			cout << "Fail to open the block of file " << aimfile << endl;
			return 0;
		}
		isDirty = 0;
	}
	/*
	if (block[blockNo].preBlock > 0)
	{
	block[block[blockNo].preBlock].nextBlock = block[blockNo].nextBlock;
	}

	if (block[blockNo].nextBlock > 0)
	{
	block[block[blockNo].nextBlock].preBlock = block[blockNo].preBlock;
	}

	block[blockNo].usage = -1;
	nowBlock--;*/

	return 1;
}















int buffer::findnextblock(string fileName)    //get a new block for some file;
{
	blockInfo * LRUBlock;
	int bp;
	fileInfo  *	fp;
	fileInfo  *	temfp;
	LRUBlock = NULL;
	temfp = NULL;
	for (int i = 0; i < MAX_BLOCK; i++)
	{
		if (block[i].usage == -1)
		{
			LRUBlock = &block[i];
			break;
		}
	}

	if (LRUBlock == NULL)            //get an empty block;
	{
	int flag = 100000;
	for (fp = fileHead; fp != NULL; fp = fp->next)
		for (bp = fp->firstBlock; bp != NULL; bp = block[bp].nextBlock)
		{
		if (block[bp].lrutime < flag  && block[bp].lock != 1)
		{
			flag = block[bp].lrutime;
			LRUBlock = &block[bp];
			temfp = fp;
		}
		}

		if (LRUBlock->dirty)
		{
			freeBlock(temfp->fileName, LRUBlock->id, 1);
			LRUBlock->dirty = 0;
			nowBlock++;
		}
	}

	//clear the address
	memset(LRUBlock->address, 0, BLOCK_LEN);

	if ((getFile(fileName)->firstBlock) > 0)
	{
		LRUBlock->offset = block[getFile(fileName)->firstBlock].offset + 1;
	}
	else
	{
		LRUBlock->offset = 0;
	}
	LRUBlock->dirty = 0;
	LRUBlock->lock = 0;
	LRUBlock->lrutime = 0;
	LRUBlock->usage = -1;;
	LRUBlock->nextBlock = -1;
	LRUBlock->preBlock = -1;



	FILE * fileread;
	blockInfo * bpp;
	bpp = LRUBlock;

	if (fileread = fopen(fileName.c_str(), "rb+"))
	{
		fseek(fileread, bpp->offset*BLOCK_LEN, 0);
		//if read over then  return null 
		if (fread(bpp->address, 1, BLOCK_LEN, fileread) == 0) //if read nothing
		{
			
			fclose(fileread);
			bpp->offset = 0;
			return -1;
		}
		fclose(fileread);
	}
	// set position

	LRUBlock->file = getFile(fileName);  //set status after makesure
	LRUBlock->usage = 0;
	block[getFile(fileName)->firstBlock].preBlock = bpp->id;
	LRUBlock->nextBlock = getFile(fileName)->firstBlock;
	getFile(fileName)->firstBlock = bpp->id;
	bpp->usage = findUsage(fileName.c_str(), bpp->address);
	
	return bpp->id;          //return that id
	
}


int buffer::addNewBlock(string fileName)    //get a new block for some file;
{
	blockInfo * LRUBlock;
	int bp;
	fileInfo  *	fp;
	fileInfo  *	temfp;
	LRUBlock = NULL;
	temfp = NULL;
	for (int i = 0; i < MAX_BLOCK; i++)
	{
		if (block[i].usage == -1)
		{
			LRUBlock = &block[i];
			break;
		}
	}

	if (LRUBlock == NULL)            //get an empty block;
	{
		int flag = 100000;
		for (fp = fileHead; fp != NULL; fp = fp->next)
			for (bp = fp->firstBlock; bp != NULL; bp = block[bp].nextBlock)
			{
			if (block[bp].lrutime < flag  && block[bp].lock != 1)
			{
				flag = block[bp].lrutime;
				LRUBlock = &block[bp];
				temfp = fp;
			}
			}

		if (LRUBlock->dirty)
		{
			freeBlock(temfp->fileName, LRUBlock->id, 1);
			LRUBlock->dirty = 0;
			nowBlock++;
		}
	}




	if ((getFile(fileName)->firstBlock) > 0)
	{
		LRUBlock->offset = block[getFile(fileName)->firstBlock].offset + 1;
	}
	else
	{
		LRUBlock->offset = 0;
	}
	//clear the address
	memset(LRUBlock->address, 0, BLOCK_LEN);
	LRUBlock->dirty = 0;
	LRUBlock->lock = 0;
	LRUBlock->lrutime = 0;
	LRUBlock->usage = 0;
	LRUBlock->nextBlock = -1;
	LRUBlock->preBlock = -1;
	LRUBlock->file = getFile(fileName);


	FILE * fileread;
	blockInfo * bpp;
	bpp = LRUBlock;
	int filelength = get_file_size(fileName.c_str());
	bpp->offset = filelength / BLOCK_LEN;

	if (fileread = fopen(fileName.c_str(), "rb+"))
	{
		fseek(fileread, bpp->offset*BLOCK_LEN, 0);
			fwrite(bpp->address, BLOCK_LEN, 1, fileread); //write empty thing in ;
	
		fclose(fileread);
	}
	// set position

	block[getFile(fileName)->firstBlock].preBlock = bpp->id;
	bpp->nextBlock = getFile(fileName)->firstBlock;
	getFile(fileName)->firstBlock = bpp->id;
	bpp->usage = findUsage(fileName.c_str(), bpp->address);

	return bpp->id;          //return that id

}


void buffer::deleteFile(string fileName)
{
	for (int i = 0; i <= MAX_BLOCK; i++)
	{
		if (block[i].file == NULL) continue;
		if (block[i].usage != -1 && block[i].file->fileName == fileName)
		{
			freeBlock(fileName, i, block[i].dirty);
		}
	}
	
	remove(fileName.c_str());
	return;
}













// get file struct of aimfilename
fileInfo * buffer::getFile(string aimfilename)
{
	fileInfo *fp;
	int bp;
	for (fp = fileHead; fp != NULL; fp = fp->next)
	{
		if (fp->fileName == aimfilename) return fp;
	}

	if (fp == NULL)
	{
		if (nowFile < MAX_FILE_ACTIVE)
		{
			for (int i = nowFile; i < MAX_FILE_ACTIVE; i++)
			{
				if (file[i].usage == -1)
				{
					fp = &file[i];
					break;
				}
			}
			nowFile++;
			fp->usage = 0;
			fp->fileName = aimfilename;
			// put it into the top of the link 
			if (fileHead != NULL)
			{
				fp->next = fileHead;
				fileHead = fp;
			}
			else                  // if filehead is not null set fp in to file head
			{
				fp->next = NULL;
				fileHead = fp;
			}
		}
		else  // nowfile==MAX
		{
			fp = fileHead;
			fp->fileName = aimfilename; // set the top as the file block
			for (bp = fp->firstBlock; bp != NULL; bp = block[bp].nextBlock)
			{
				block[bp].usage = 0;
				if (freeBlock(aimfilename,bp,block[bp].dirty)==1)
				{
					cout << "Fail to flush block" << endl;
					return NULL;
				}
				
			}
			fp->firstBlock = NULL;  // the file got no block here.


		}
	}
	//        Wait the catalog function to put in
	//get information data from dict function
	int imformation[3];
	catalogM.getfileinfo(aimfilename, imformation);       // get the information of the file
	//if it is a table
	
	if (imformation[0] == TABLE_FILE)
	{
		fp->type = TABLE_FILE;
		fp->recordAmount = imformation[1];
		fp->recordLength = imformation[2];
	}
	//if index file
	else if (imformation[0] == INDEX_FILE)
	{
		fp->type = INDEX_FILE;
		fp->recordLength = -1;         
		fp->recordAmount = -1;
	}
	//do not have such file 
	else
	{
		cout << "File not found" << endl;
		return NULL;
	}
	

	return fp;
}
int buffer::findUsage(const char * fileName, const char * address)
{
	string s(fileName);
	int recordLen;
	recordLen = catalogM.getrecordLength(s);
	const char * p;
	p = address;
	// route get length
	while (p - address<BLOCK_LEN)
	{
		const char * tmp;
		tmp = p;
		int i;


		for (i = 0; i<recordLen; i++)
		{
			if (*tmp != 0)
				break;
			tmp++;
		}
		if (i == recordLen)
			break;

		p = p + recordLen;
	}
	return p - address;
}
/*
int main()
{
	bm.createNewFile("testfile2");
	cout << bm.getFile("testfile2")->fileName<<endl;
	int i=bm.addNewBlock("testfile2");
	cout << bm.getNumberOfBlocks("testfile1") << endl;;

	//cout << bm.block[i].address<<endl;
	system("pause");
}*/
