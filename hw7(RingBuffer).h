#pragma once

#define BUFSIZE	10000

class CRingBuffer {
private:
	char* _Buffer;
	int _iBufferSize;
	int _iFront;
	int _iRear;

	void Initial(int iBufferSize);

public:
	CRingBuffer(int iBufferSize = BUFSIZE);
	~CRingBuffer();
	void ReSize(int iSize);

	int GetBufferSize();
	int GetUseSize();
	int GetFreeSize();

	int DirectEnqueueSize();
	int DirectDequeueSize();

	int Enqueue(char* chpData, int iSize);
	int Dequeue(char* chpDest, int iSize);

	int Peek(char* chpDest, int iSize);

	void MoveRear(int iSize);
	void MoveFront(int iSize);

	void ClearBuffer();

	char* GetFrontBufferPtr();
	char* GetRearBufferPtr();

};