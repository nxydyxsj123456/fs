#include <stdio.h>

#include "block.h"
#include "superBlock.h"

extern SUPER_BLOCK * superBlockPointer;
extern FILE * dataFp;

void writeBlock(BLOCK * blockP, void * mem)
{
	if(NULL == mem){
		return ;
	}
	fseek(dataFp, blockP->blockNumber * superBlockPointer->blockSize, SEEK_SET);
	fwrite(mem, superBlockPointer->blockSize, 1, dataFp);
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