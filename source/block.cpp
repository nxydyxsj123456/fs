#include <stdio.h>
#include <stdlib.h>

#include "block.h"
#include "superBlock.h"

extern SUPER_BLOCK * superBlockPointer;
extern FILE * dataFp;

void initBlock(BLOCK * blockP) //������д0
{
	void * mem = Malloc(superBlockPointer->blockSize);
	writeBlock(blockP, mem);
	free(mem);
}

void writeBlock(BLOCK * blockP, void * mem)
{
	if(NULL == mem){
		return ;
	}
	fseek(dataFp, getBlockOffset(blockP->blockNumber), SEEK_SET);
	fwrite(mem, superBlockPointer->blockSize, 1, dataFp);
}

void readBlock(BLOCK * blockP, void * mem)
{
	fseek(dataFp, getBlockOffset(blockP->blockNumber), SEEK_SET);
	fread(mem, superBlockPointer->blockSize, 1, dataFp);
}

//�ҵ�һ��û�õ���������ȫ��д0�����ظ������ı��
BLOCK * createBlock()
{
	int blockPos = findZero(superBlockPointer->bBitMap, 
		superBlockPointer->blockBitMapCount * superBlockPointer->blockSize / 8);
	fseek(dataFp, superBlockPointer->blockSize * blockPos, SEEK_SET);
	writeNull(512, dataFp);
	BLOCK * blockP = (BLOCK *)Malloc(sizeof(BLOCK));
	blockP->blockNumber = blockPos;
	return blockP;
}

//����������Ż������ָ��
BLOCK * getBlock(int blockNumber)
{
	BLOCK * blockP = (BLOCK *)Malloc(sizeof(BLOCK));
	return blockP;
}

int getBlockOffset(int blockNumber)
{
	return superBlockPointer->blockSize * blockNumber;
}

void freeBlock(BLOCK * blockP)
{
	free(blockP);
}