//
// 2017-04-24, jjuiddong
// GameMain Class with DockingWindow
//
#pragma once

#include "../ui/renderwindow.h"


namespace framework
{

	class cGameMain2 : public cRenderWindow
	{
	public:
		cGameMain2();
		virtual ~cGameMain2();

		virtual bool Init();
		virtual void Update(const float deltaSeconds) override;
		virtual void LostDevice() override;
		virtual void ResetDevice(graphic::cRenderer *shared = NULL) override;
		virtual void Exit();


	protected:
		virtual bool OnInit() { return true; }


	public:
		HWND m_hWnd;
		wstring m_windowName;
		sRecti m_windowRect;
	};

	// �����ӿ�ũ ���� �Լ�.
	int FrameWorkWinMain2(HINSTANCE hInstance,
		HINSTANCE hPrevInstance,
		LPSTR lpCmdLine,
		int nCmdShow,
		const bool dualMonitor = false);

	// �����ӿ�ũ �ν��Ͻ��� �����Ѵ�. �ݵ�� �� �Լ��� �����ؾ� �Ѵ�.
	cGameMain2* CreateFrameWork2();

}