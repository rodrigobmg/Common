//
// 2016-05-22, jjuiddong
// ȭ�鿡 ��µǴ� ��� ��ü�� �� Ŭ�����κ��� ��ӹ޴´�.
//
#pragma once


namespace graphic
{
	class cRenderer;

	class cNode
	{
	public:
		enum NODE_TYPE { NONE, BONE, MESH, MODEL};

		cNode( const int id, const StrId &name="none");
		virtual ~cNode();

		const StrId& GetName() const;
		int GetId() const;
		cNode* GetParent();

		const Matrix44& GetTransform() const;
		const Matrix44& GetLocalTM() const;
		void SetTransform(const Matrix44 &tm);
		void MultiplyTM(const Matrix44 &tm);
		void SetLocalTM(const Matrix44 &tm);
		cShader* GetShader();
		void SetRender(const bool isRender);
		bool IsRender() const;

		bool InsertChild(cNode *node);
		const cNode* FindNode(const int id) const;
		const cNode* FindNode(const StrId &name) const;
		bool RemoveNode(const int id);
		vector<cNode*>& GetChildren();
		virtual void Clear();

		virtual bool Update(const float deltaSeconds) {return true;}
		virtual void Render(cRenderer &renderer, const Matrix44 &parentTm=Matrix44::Identity);
		virtual void RenderShadow(cRenderer &renderer, const Matrix44 &viewProj,
			const Vector3 &lightPos, const Vector3 &lightDir, const Matrix44 &parentTm=Matrix44::Identity);
		virtual void SetShader(cShader *shader);


	public:
		int m_id;
		StrId m_name;
		cNode *m_parent;
		vector<cNode*> m_children;
		NODE_TYPE m_nodeType;
		Matrix44 m_localTM;
		Matrix44 m_aniTM;
		Matrix44 m_TM;
		Vector3 m_scale;
		bool m_isRender;

		cShader *m_shader; // reference
	};


	inline const StrId& cNode::GetName() const { return m_name; }
	inline int cNode::GetId() const { return m_id; }
	inline cNode* cNode::GetParent() { return m_parent; }
	inline const Matrix44& cNode::GetTransform() const { return m_TM; }
	inline void cNode::SetTransform(const Matrix44 &tm) { m_TM = tm; }
	inline void cNode::MultiplyTM(const Matrix44 &tm) { m_TM *= tm; }
	inline const Matrix44& cNode::GetLocalTM() const { return m_localTM; }
	inline void cNode::SetLocalTM(const Matrix44 &tm) { m_localTM = tm; }
	inline vector<cNode*>& cNode::GetChildren() { return m_children; }
	inline void cNode::SetShader(cShader *shader) { m_shader = shader; }
	inline cShader* cNode::GetShader() { return m_shader; }
	inline void cNode::SetRender(const bool isRender) { m_isRender = isRender; }
	inline bool cNode::IsRender() const { return m_isRender; }
}