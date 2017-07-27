//
// 2016-05-21, jjuiddong
//		- scene ���� �߰�.
//
#pragma once


namespace framework
{

	class cScene;
	class cGameMain
	{
	public:
		cGameMain();
		virtual ~cGameMain();
		
		enum STATE
		{
			INIT,
			RUN,
			SHUTDOWN,
		};

		virtual bool Init(HWND hWnd);
		virtual void ShutDown();
		virtual void Run();
		virtual void Update(const float deltaSeconds);
		virtual void Render(const float deltaSeconds);
		virtual void Exit();

		bool InsertScene(cScene *scene);
		bool RemoveScene(const int sceneId, const bool removeMemory=true);
		bool ChangeScene(const int sceneId);

		const wstring& GetWindowName();
		common::sRecti GetWindowRect();		
		void MessageProc( UINT message, WPARAM wParam, LPARAM lParam);


	protected:
		virtual bool OnInit() { return true; }
		virtual void OnUpdate(const float deltaSeconds) {}
		virtual void OnRender(const float deltaSeconds) {}
		virtual void OnShutdown() {}
		virtual void OnLostDevice() {}
		virtual void OnMessageProc(UINT message, WPARAM wParam, LPARAM lParam) {}


	public:
		STATE m_state;
		HWND m_hWnd;
		wstring m_windowName;
		sRecti m_windowRect;
		graphic::cRenderer m_renderer;
		cScene *m_currentScene;
		map<int, cScene*> m_scenes;


	// singleton
	protected:
		static cGameMain* m_pInstance;
	public:
		static cGameMain* Get();
		static void Release();
	};


	// �����ӿ�ũ ���� �Լ�.
	int FrameWorkWinMain(HINSTANCE hInstance, 
		HINSTANCE hPrevInstance, 
		LPSTR lpCmdLine, 
		int nCmdShow,
		const bool dualMonitor=false);

	// �����ӿ�ũ �ν��Ͻ��� �����Ѵ�. �ݵ�� �� �Լ��� �����ؾ� �Ѵ�.
	cGameMain* CreateFrameWork();
}