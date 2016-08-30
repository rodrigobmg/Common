
#include "stdafx.h"
#include "streamingsender.h"

using namespace cv;
using namespace cvproc;
using namespace std;

cStreamingSender::cStreamingSender() :
	m_jpgCompressQuality(40)
	, m_isCompressed(false)
	, m_sndBuffer(NULL)
	, m_state(NOTCONNECT)
	, m_fps(20)
	, m_deltaTime(50)
	, m_chunkId(0)
	, m_rows(0)
	, m_cols(0)
	, m_flag(0)
{
}

cStreamingSender::~cStreamingSender()
{
	SAFE_DELETEA(m_sndBuffer);
}


bool cStreamingSender::Init(const int port,
	const bool isConvertGray, const bool isCompressed, const int jpgQuality)
{
	m_isConvertGray = isConvertGray;
	m_isCompressed = isCompressed;
	m_jpgCompressQuality = jpgQuality;

	for each (auto &udp in m_udpClient)
		udp.Close();
	m_tcpServer.Close();
	memset(m_udpUsed, 0, sizeof(m_udpUsed));

	if (!m_tcpServer.Init(port, g_maxStreamSize, 512, 10))
		return false;

	m_tcpServer.SetListener(this);

	m_state = READY;

	if (m_gray.empty())
		m_gray = Mat(480, 640, CV_8UC1);

	if (m_compressBuffer.capacity() == 0)
		m_compressBuffer.reserve(g_maxStreamSize);

	if (!m_sndBuffer)
		m_sndBuffer = new BYTE[g_maxStreamSize];

	m_lastSendTime = timeGetTime();

	return true;
}


void cStreamingSender::Close()
{
	for each (auto &udp in m_udpClient)
		udp.Close();
	m_tcpServer.Close();
}


// 프로토콜을 확인해서 UDP/TCP 전송을 판단한다.
void cStreamingSender::CheckPacket()
{
	RET(!m_tcpServer.IsConnect());

	network::sSockBuffer packet;
	if (m_tcpServer.m_recvQueue.Front(packet))
	{
		// 프로토콜 검사.
		if ((unsigned int)packet.actualLen >= sizeof(sStreamingProtocol))
		{

			sStreamingProtocol *data = (sStreamingProtocol*)packet.buffer;
			cout << "<<< recv streaming protocol = " << (int)data->protocol << endl;
			switch (data->protocol)
			{
			case 100: // udp 프로토콜 설정 패킷
				cout << "** recv protocol setting **" << endl;
				if (data->type == 1)  // udp라면, 소켓 생성
				{

					// 빈 udp 소켓을 찾는다.
					int selectIdx = -1;
					const int size = sizeof(m_udpUsed) / sizeof(bool);
					for (int i = 0; i < size; ++i)
					{
						if (!m_udpUsed[i])
						{
							selectIdx = i;
							break;
						}
					}

					// 빈 udp 소켓을 찾으면..
					if (selectIdx >= 0)
					{
						// 상대 PC의 IP, Port를 확인한 후, UDP 전송을 시작한다.
						char *ip = inet_ntoa(*(in_addr*)&data->uip);
						cout << "select udp index " << selectIdx << endl;
						m_udpClient[selectIdx].SetMaxBufferLength(g_maxStreamSize);
						if (m_udpClient[selectIdx].Init(ip, data->port, 10))
						{
							m_udpUsed[selectIdx] = true;
						}
						else
						{
							cout << "udp init error " << ip << ", " << data->port << endl;
							selectIdx = -1;
						}
					}

					m_users[packet.sock] = selectIdx;
				}
				else
				{
					// tcp 전송
					// 현재 선택된 소켓이 udp로 전송 중이라면, 초기화 한다.
					auto it = m_users.find(packet.sock);
					if (m_users.end() != it)
					{
						const int udpIdx = it->second;
						if (udpIdx >= 0)
						{
							m_udpClient[udpIdx].Close();
							m_udpUsed[udpIdx] = false;
						}
						m_users[packet.sock] = -1;
					}
				}
				break;


			case 101: // gray, compressed, compressed quality 설정
				m_isConvertGray = (data->gray == 1) ? true : false;
				m_isCompressed = (data->compressed == 1) ? true : false;
				m_jpgCompressQuality = data->compQuality;
				m_fps = data->fps;
		
				if (data->fps > 0)
					m_deltaTime = 1000 / data->fps;
				else
					m_deltaTime = 100;

				cout << "**change option >>" << endl;
				cout << "gray=" << m_isConvertGray << ", compressed=" << m_isCompressed << ", jpgCompressQuality=" << m_jpgCompressQuality << ", fps=" << m_fps << endl;
				cout << "**<< " << endl;
				break;
			}

		}

		m_tcpServer.m_recvQueue.Pop(); // 처리한 패킷은 제거
	}

}


// 크기는 640/480 이어야 한다.
// 전송할 영상이 남았다면 0을 리턴한다.
// 모두 전송했다면 1을 리턴한다.
int cStreamingSender::Send(const cv::Mat &image)
{
	// no connection user, return
	if (m_tcpServer.m_sessions.empty())
		return 1;

	if (READY == m_state)
	{
		// fps control
		const int curT = timeGetTime();
		if ((curT - m_lastSendTime) < m_deltaTime)
			return 0;
		m_lastSendTime = curT;
		//

		// 이미지를 변조한다.
		uchar *data = image.data;
		int buffSize = image.total() * image.elemSize();
		m_rows = image.rows;
		m_cols = image.cols;
		m_flag = image.flags;

		// Gray Scale
		if (m_isConvertGray)
		{
			cvtColor(image, m_gray, CV_RGB2GRAY);
			data = m_gray.data;
			buffSize = m_gray.step[0] * m_gray.rows;
		}

		// 압축
		if (m_isCompressed)
		{
			vector<int> p(3);
			p[0] = CV_IMWRITE_JPEG_QUALITY;
			p[1] = m_jpgCompressQuality;
			p[2] = 0;
			cv::imencode(".jpg", m_isConvertGray? m_gray : image, m_compressBuffer, p);
			data = (uchar*)&m_compressBuffer[0];
			buffSize = m_compressBuffer.size();
		}

		const int chunkSize = SendImage(data, buffSize, 
			image.cols, image.rows, image.flags, m_isConvertGray, m_isCompressed);
		if (chunkSize == 1)
			return 1;
	}
	else if (SPLIT == m_state)
	{
		if (SendSplit())
		{
			m_state = READY;
			++m_chunkId;
			if (m_chunkId > 1000)
				m_chunkId = 0;
			return 1;
		}
	}

	return 0;
}


// 용량이 큰 데이타면, 분리해서 전송한다.
// 청크 갯수를 리턴한다.
int cStreamingSender::SendImage(const BYTE *data, const int len, 
	const unsigned short width, const unsigned short height, const unsigned int flag,
	const bool isGray, const bool isCompressed)
{
	const int totalSize = len + sizeof(sStreamingData);

	if (totalSize < g_maxStreamSize)
	{
		sStreamingData *packet = (sStreamingData*)m_sndBuffer;
		packet->id = 0;
		packet->chunkSize = 1;
		packet->chunkIndex = 0;
		packet->width = width;
		packet->height = height;
		packet->flag = flag;
		packet->imageBytes = len;
		packet->isGray = isGray? 1 : 0;
		packet->isCompressed = isCompressed? 1 : 0;
		packet->data = m_sndBuffer + sizeof(sStreamingData);
		memcpy(packet->data, data, len);

		for each (auto &session in m_tcpServer.m_sessions)
		{
			auto it = m_users.find(session.socket);
			int udpIdx = -1;
			if (it != m_users.end())
				udpIdx = it->second;

			if (udpIdx >= 0)
			{
				m_udpClient[udpIdx].SendData((BYTE*)packet, sizeof(sStreamingData) + len);
			}
			else
			{
				m_tcpServer.m_sendQueue.Push(session.socket, (BYTE*)packet, sizeof(sStreamingData) + len);
			}
		}

		return 1;
	}
	else
	{
		const int sizePerChunk = g_maxStreamSize - sizeof(sStreamingData) - sizeof(network::cPacketQueue::sHeader); // size per chunk
		m_chunkCount = (len / sizePerChunk);
		if ((len % sizePerChunk) != 0)
			++m_chunkCount;

		m_sendChunkIndex = 0;
		m_sendBytes = 0;
		m_imagePtr = (BYTE*)data;
		m_imageBytes = len;
		SendSplit();

		m_state = SPLIT;
		return m_chunkCount;
	}
}


// 모든 청크를 전송했다면 true를 리턴한다.
bool cStreamingSender::SendSplit()
{
	const int sizePerChunk = g_maxStreamSize - sizeof(sStreamingData) - sizeof(network::cPacketQueue::sHeader); // size per chunk

	sStreamingData *packet = (sStreamingData*)m_sndBuffer;
	packet->id = (unsigned char)m_chunkId;
	packet->chunkSize = m_chunkCount;
	packet->chunkIndex = m_sendChunkIndex;
	packet->width = m_cols;
	packet->height = m_rows;
	packet->flag = m_flag;
	packet->imageBytes = m_imageBytes;
	packet->isGray = m_isConvertGray ? 1 : 0;
	packet->isCompressed = m_isCompressed ? 1 : 0;
	packet->data = m_sndBuffer + sizeof(sStreamingData);

	int copyLen = 0;
	if (m_sendBytes + sizePerChunk > m_imageBytes)
	{
		// last send
		copyLen = m_imageBytes % sizePerChunk;
		memcpy(packet->data, m_imagePtr + m_sendChunkIndex * sizePerChunk, copyLen);
	}
	else
	{
		copyLen = sizePerChunk;
		memcpy(packet->data, m_imagePtr + m_sendChunkIndex * sizePerChunk, copyLen);
	}

	for each (auto &session in m_tcpServer.m_sessions)
	{
		auto it = m_users.find(session.socket);
		int udpIdx = -1;
		if (it != m_users.end())
			udpIdx = it->second;

		if (udpIdx >= 0)
		{
			m_udpClient[udpIdx].SendData((BYTE*)packet, sizeof(sStreamingData) + copyLen);
		}
		else
		{
			m_tcpServer.m_sendQueue.Push(session.socket, (BYTE*)packet, sizeof(sStreamingData) + copyLen);
			//dbg::Log("SendImage chunkidx = %d\n", m_sendChunkIndex);
		}
	}

	m_sendBytes += sizePerChunk;
	++m_sendChunkIndex;

	if (m_sendBytes >= m_imageBytes)
		return true; // complete send full image

	return false;
}


bool cStreamingSender::IsConnect()
{
	return m_tcpServer.IsConnect();
}


bool cStreamingSender::IsExistClient()
{
	return m_tcpServer.IsExistSession();
}


void cStreamingSender::RemoveSession(const SOCKET remoteSock)
{
	auto it = m_users.find(remoteSock);
	if (m_users.end() != it)
	{
		if (it->second >= 0)
		{
			m_udpUsed[it->second] = false; // UDP 초기화
			m_udpClient[it->second].Close();
		} 
	}
}
