//
// 2017-03-10, jjuiddong
// Shadow Renderer
//
#pragma once

namespace graphic
{

	class cShader;
	class cRenderer;

	interface iShadowRenderer
	{
	public:
		iShadowRenderer() : m_shadowShader(NULL) {}
		virtual ~iShadowRenderer() {}

		virtual void SetShadowShader(cShader *shader) { m_shadowShader = shader; }
		virtual void RenderShadow(cRenderer &renderer, const Matrix44 &tm = Matrix44::Identity) = 0;


	public:
		cShader *m_shadowShader;
	};

}
