#ifndef Buffer_H
#define Buffer_H
#include <iostream>
#include <fstream>
#define BLOCK_LEN			4096	// the size of one block
#define MAX_FILE_ACTIVE 	5		// limit the active files in the buffer
#define MAX_BLOCK			40		// the max number of the blocks
using namespace std;

struct fileInfo  {
	int type;				// 0-> data file£¬ 1 -> index file
	string fileName;		// the name of the file

	int recordAmount;		// the number of record in the file
	int recordLength;		// the length of the record in the file
	int usage;               //if the block is in use
	fileInfo *next;			// the pointer points to the next file
	int firstBlock;	// point to the first block within the file
};

struct blockInfo  {
	int offset;            //the offset
	int dirty;             //block dirty bit
	int lock;              //block lock
	int lrutime;               //recent use time lru algorithm
	int id;                 //block id;
	int usage;             //use point ,if =-1 then not be used
	char * address;        //start address
	int nextBlock; //To next block
	int preBlock;  //To pre block
	fileInfo *file;    	// the pointer point to the file, which the block belongs to

};

class buffer
{
public:
	buffer();
	~buffer();
	fileInfo* getFile(string filename); // get the file 
	 
	int findUsage(const char * fileName, const char * address);


    // needs by index
	int getNumberOfBlocks(string fileName);

	char *getBlockAddress(string fileName, int blockNo);

	char *getoffsetBlockAddress(string fileName, int blockNo);
	

	int freeBlock(string fileName, int blockNo, bool isDirty);// success or not

	int freeBlock_static(string fileName, int blockNo, bool isDirty);// success or not

		void createNewFile(string fileName);

		int addNewBlock(string fileName);

		int findnextblock(string fileName);

		void deleteFile(string fileName);
	

	fileInfo * fileHead;          // the head of first file

	fileInfo file[MAX_FILE_ACTIVE]; //the now opened files

	blockInfo  block[MAX_BLOCK]; // now opended blocks
private:


	int nowBlock;          // the number of opened block
	int nowFile;         // the number of  opened files
	

	unsigned long get_file_size(const char *filename){
		unsigned long size;
		FILE* fp = fopen(filename, "rb+");
		if (fp == NULL){
			printf("ERROR: Open file %s failed.\n", filename);
			return 0;
		}
		fseek(fp, 0L, SEEK_END);
		size = ftell(fp);
		fclose(fp);
		return size;
	}




};


#endif