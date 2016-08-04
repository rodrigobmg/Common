#include "stdafx.h"
#include "packetqueue.h"
#include "session.h"

using namespace network;


cPacketQueue::cPacketQueue()
	: m_memPoolPtr(NULL)
	, m_chunkBytes(0)
	, m_totalChunkCount(0)
	, m_tempHeaderBuffer(NULL)
	, m_tempBuffer(NULL)
	, m_isStoreTempHeaderBuffer(false)
	, m_isIgnoreHeader(false)
{
	InitializeCriticalSectionAndSpinCount(&m_criticalSection, 0x00000400);
}

cPacketQueue::~cPacketQueue()
{
	Clear();

	DeleteCriticalSection(&m_criticalSection);
}


bool cPacketQueue::Init(const int packetSize, const int maxPacketCount, const bool isIgnoreHeader)
// isIgnoreHeader = false
{
	Clear();

	m_isIgnoreHeader = isIgnoreHeader;

	// Init Memory pool
	m_chunkBytes = packetSize;
	m_totalChunkCount = maxPacketCount;
	m_memPoolPtr = new BYTE[(packetSize+sizeof(sHeader)) * maxPacketCount];
	m_memPool.reserve(maxPacketCount);
	for (int i = 0; i < maxPacketCount; ++i)
		m_memPool.push_back({ false, m_memPoolPtr + (i*(packetSize + sizeof(sHeader))) });
	//

	m_queue.reserve(maxPacketCount);
	m_tempBuffer = new BYTE[packetSize + sizeof(sHeader)];
	m_tempBufferSize = packetSize + sizeof(sHeader);
	m_tempHeaderBuffer = new BYTE[sizeof(sHeader)];
	m_tempHeaderBufferSize = 0;

	return true;
}


// ��Ʈ��ũ�� ���� �� ��Ŷ�̳�, ����ڰ� ������ ��Ŷ�� �߰��� ��, ȣ���Ѵ�.
// �������� ������ ��Ŷ�� ó���� ���� ȣ���� �� �ִ�.
// *data�� �ΰ� �̻��� ��Ŷ�� ������ ����, �̸� ó���Ѵ�.
void cPacketQueue::Push(const SOCKET sock, const BYTE *data, const int len,
	const bool fromNetwork) // fromNetwork = false
{
	cAutoCS cs(m_criticalSection);

	int totalLen = len;

	// ���� ������ ���۰� ���� ��.. 
	// �ӽ÷� �����ص״� ������ ���ڷ� �Ѿ�� ������ ���ļ� ó���Ѵ�.
	if (m_isStoreTempHeaderBuffer)
	{
		if (m_tempBufferSize < (len + m_tempHeaderBufferSize))
		{
			SAFE_DELETEA(m_tempBuffer);
			m_tempBuffer = new BYTE[len + m_tempHeaderBufferSize];
			m_tempBufferSize = len + m_tempHeaderBufferSize;
		}

		memcpy(m_tempBuffer, m_tempHeaderBuffer, m_tempHeaderBufferSize);
		memcpy(m_tempBuffer + m_tempHeaderBufferSize, data, len);	
		m_isStoreTempHeaderBuffer = false;

		data = m_tempBuffer;
		totalLen = m_tempHeaderBufferSize + len;
	}

	int offset = 0;
	while (offset < totalLen)
	{
		const BYTE *ptr = data + offset;
		const int size = totalLen - offset;
		if (sSockBuffer *buffer = FindSockBuffer(sock))
		{
			offset += CopySockBuffer(buffer, ptr, size);
		}
		else
		{
			// ���� ������ ����Ÿ�� sHeader ���� ���� ��, �ӽ÷� 
			// ������ ��, ������ ��Ŷ�� ���� �� ó���Ѵ�.
			if (fromNetwork && (size < sizeof(sHeader)))
			{
				m_tempHeaderBufferSize = size;
				m_isStoreTempHeaderBuffer = true;
				memcpy(m_tempHeaderBuffer, ptr, size);
				offset += size;
			}
			else
			{
				offset += AddSockBuffer(sock, ptr, size, fromNetwork);
			}
		}
	}
}


// ũ��Ƽ�ü����� ���� ������ �Ŀ� ȣ������.
// sock �� �ش��ϴ� ť�� �����Ѵ�. 
sSockBuffer* cPacketQueue::FindSockBuffer(const SOCKET sock)
{
	// find specific packet by socket
	for (u_int i = 0; i < m_queue.size(); ++i)
	{
		if (sock != m_queue[i].sock)
			continue;
		// �ش� ������ ä������ ���� ���۶��, ����
		if (m_queue[i].readLen < m_queue[i].totalLen)
			return &m_queue[i];
	}
	return NULL;
}


// cSockBuffer *dst�� data �� len ��ŭ �����Ѵ�.
// �� ��, len�� SockBuffer�� ����ũ�� ��ŭ �����Ѵ�.
// ���� �� ũ�⸦ ����Ʈ ������ �����Ѵ�.
int cPacketQueue::CopySockBuffer(sSockBuffer *dst, const BYTE *data, const int len)
{
	RETV(!dst, 0);
	RETV(!dst->buffer, 0);

	const int copyLen = min(dst->totalLen - dst->readLen, len);
	memcpy(dst->buffer + dst->readLen, data, copyLen);
	dst->readLen += copyLen;

	if (dst->readLen == dst->totalLen)
		dst->full = true;

	return copyLen;
}


// �� ��Ŷ ���۸� �߰��Ѵ�.
int cPacketQueue::AddSockBuffer(const SOCKET sock, const BYTE *data, const int len, const bool fromNetwork)
{
	// ���� �߰��� ������ ��Ŷ�̶��
	sSockBuffer sockBuffer;
	sockBuffer.sock = sock;
	sockBuffer.totalLen = 0;
	sockBuffer.readLen = 0;
	sockBuffer.full = false;

	bool isMakeHeader = false; // ��Ŷ ����� ���� ���, �����Ѵ�.

	int offset = 0;
	if (fromNetwork)
	{
		// ��Ʈ��ũ�� ���� �� ��Ŷ�̶��, �̹� ��Ŷ����� ���Ե� ���´�.
		// ��ü ��Ŷ ũ�⸦ �����ؼ�, �и��� ��Ŷ������ �Ǵ��Ѵ�.
		sHeader *pheader = (sHeader*)data;
		if ((pheader->head[0] == '$') && (pheader->head[1] == '@'))
		{
			sockBuffer.totalLen = pheader->length;
			sockBuffer.actualLen = pheader->length - sizeof(sHeader);
			sockBuffer.buffer = Alloc();
		}
		else
		{
			if (m_isIgnoreHeader)
			{
				// ����� ���� ��Ŷ�� ���, �����ؼ� �߰��Ѵ�.
				isMakeHeader = true;
			}
			else
			{
				// error occur!!
				// ��Ŷ�� ���ۺΰ� �ƴѵ�, ���ۺη� ������.
				// ����ΰ� �����ų�, ���� ���۰� Pop �Ǿ���.
				// �����ϰ� �����Ѵ�.
				common::dbg::ErrLog("packetqueue push error!! packet header not found\n");
				return len;
			}
		}
	}

	if (!fromNetwork || isMakeHeader)
	{
		// �۽��� ��Ŷ�� �߰��� ���, or ����� ���� ��Ŷ�� ���� ���.
		// ��Ŷ ����� �߰��Ѵ�.
		sockBuffer.totalLen = len + sizeof(sHeader);
		sockBuffer.actualLen = len;

		sHeader header;
		header.head[0] = '$';
		header.head[1] = '@';
		header.length = len + sizeof(sHeader);

		sockBuffer.buffer = Alloc();
		if (sockBuffer.buffer)
		{
			offset += CopySockBuffer(&sockBuffer, (BYTE*)&header, sizeof(sHeader));
		}
	}	

	if (!sockBuffer.buffer)
	{
		common::dbg::ErrLog("packetqueue push error!! canceled 1.\n");
		return len; // Error!! 
	}

	if ((sockBuffer.totalLen < 0) || (sockBuffer.actualLen < 0))
	{
		common::dbg::ErrLog("packetqueue push error!! canceled 2.\n");
		return len;
	}

	CopySockBuffer(&sockBuffer, data, len);
	m_queue.push_back(sockBuffer);

	if (fromNetwork)
	{
		return sockBuffer.readLen;
	}
	else
	{
		// ��Ŷ��� ũ��� ����(�����ϰ� data���� ����� ũ�⸦ �����Ѵ�.)
		return sockBuffer.readLen - sizeof(sHeader); 
	}
}


bool cPacketQueue::Front(OUT sSockBuffer &out)
{
	RETV(m_queue.empty(), false);

	cAutoCS cs(m_criticalSection);
	RETV(!m_queue[0].full, false);

	out.sock = m_queue[0].sock;
	out.buffer = m_queue[0].buffer + sizeof(sHeader); // ����θ� ������ ��Ŷ ����Ÿ�� �����Ѵ�.
	out.readLen = m_queue[0].readLen;
	out.totalLen = m_queue[0].totalLen;
	out.actualLen = m_queue[0].actualLen;

	return true;
}


void cPacketQueue::Pop()
{
	cAutoCS cs(m_criticalSection);
	RET(m_queue.empty());

	Free(m_queue.front().buffer);
	common::rotatepopvector(m_queue, 0);
}


void cPacketQueue::SendAll()
{
	RET(m_queue.empty());

	cAutoCS cs(m_criticalSection);
	for (u_int i = 0; i < m_queue.size(); ++i)
	{
		if (m_isIgnoreHeader)
		{
			send(m_queue[i].sock, (const char*)m_queue[i].buffer+sizeof(sHeader), m_queue[i].actualLen, 0);
		}
		else
		{
			send(m_queue[i].sock, (const char*)m_queue[i].buffer, m_queue[i].totalLen, 0);
		}
		Free(m_queue[i].buffer);
	}
	m_queue.clear();
}


// ť�� ����� ������ �� ũ�� (����Ʈ ����)
int cPacketQueue::GetSize()
{
	RETV(m_queue.empty(), 0);

	int size = 0;
	cAutoCS cs(m_criticalSection);
	for (u_int i = 0; i < m_queue.size(); ++i)
		size += m_queue[i].totalLen;
	return size;
}


// exceptOwner �� true �϶�, ��Ŷ�� ���� Ŭ���̾�Ʈ�� ������ ������ Ŭ���̾�Ʈ�鿡�� ���
// ��Ŷ�� ������.
void cPacketQueue::SendBroadcast(vector<sSession> &sessions, const bool exceptOwner)
{
	cAutoCS cs(m_criticalSection);

	for (u_int i = 0; i < m_queue.size(); ++i)
	{
		if (!m_queue[i].full)
			continue; // �� ä������ ���� ��Ŷ�� ����

		for (u_int k = 0; k < sessions.size(); ++k)
		{
			// exceptOwner�� true�� ��, ��Ŷ�� �� Ŭ���̾�Ʈ���Դ� ������ �ʴ´�.
			const bool isSend = !exceptOwner || (exceptOwner && (m_queue[i].sock != sessions[k].socket));
			if (isSend)
				send(sessions[k].socket, (const char*)m_queue[i].buffer, m_queue[i].totalLen, 0);
		}
	}

	// ��� ������ �� ť�� ����.
	for (u_int i = 0; i < m_queue.size(); ++i)
		Free(m_queue[i].buffer);
	m_queue.clear();

	ClearMemPool();
}


void cPacketQueue::Lock()
{
	EnterCriticalSection(&m_criticalSection);
}


void cPacketQueue::Unlock()
{
	LeaveCriticalSection(&m_criticalSection);
}


BYTE* cPacketQueue::Alloc()
{
	for (u_int i = 0; i < m_memPool.size(); ++i)
	{
		if (!m_memPool[i].used)
		{
			m_memPool[i].used = true;
			return m_memPool[i].p;
		}
	}
	return NULL;
}


void cPacketQueue::Free(BYTE*ptr)
{
	for (u_int i = 0; i < m_memPool.size(); ++i)
	{
		if (ptr == m_memPool[i].p)
		{
			m_memPool[i].used = false;
			break;
		}
	}
}


void cPacketQueue::Clear()
{
	SAFE_DELETEA(m_memPoolPtr);
	SAFE_DELETEA(m_tempBuffer);
	SAFE_DELETEA(m_tempHeaderBuffer);
	m_memPool.clear();
	m_queue.clear();
}


// �޸� Ǯ�� �ʱ�ȭ�ؼ�, ��¼�� ������ �� ���׸� �����Ѵ�.
void cPacketQueue::ClearMemPool()
{
	for (u_int i = 0; i < m_memPool.size(); ++i)
		m_memPool[i].used = false;
}