//
// 2016-05-31, jjuiddong
// 탭으로 구분된, 텍스트 문서를 분석해, 그래프 구조를 만든다.
//
//	item1
//		item2
//			item3
//			item4
//		item5
//
#pragma once



namespace cvproc {
	namespace imagematch {

		class cGraphScript
		{
		public:
			cGraphScript();
			virtual ~cGraphScript();
			
			struct sNode
			{
 				bool check; // use traverse node
 				int sceneId;// use traverse node
				map<string, string> attrs;
				vector<sNode*> out;
				vector<sNode*> in;
			};

			bool Read(const string &fileName);
			sNode* Find(const string &id);
			sNode* Find(sNode*current, const string &id);
			sNode* FindHead(const string &id);
			bool FindRoute(const string &from, const string &to, OUT vector<sNode*> &out);
			bool FindRoute(sNode*current, const string &to, OUT vector<sNode*> &out);
			void PrintGraph(const string &rootName);
			void CheckClearAllNode();
			void Clear();


		protected:
			sNode* build(sParseTree *parent, sParseTree *current, sNode *parentNode);
			sNode* FindParent(sNode *current, const string &id);
			sNode* FindParentRec(sNode *current, const string &id);
			bool FindRouteRec(sNode*current, const string &id, OUT vector<sNode*> &out);


		public:
			cParser2 m_parser;
			sNode *m_root;
			vector<sNode*> m_nodes;
			vector<sNode*> m_heads; // head node 저장, reference
			int m_sceneIdGen; // from 1 to N
		};

	}
}
