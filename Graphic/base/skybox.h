#pragma once


namespace graphic
{


	class cSkyBox
	{
	public:
		cSkyBox();
		virtual ~cSkyBox();

		bool Create(cRenderer &renderer, const StrPath &textureFilePath);
		bool Create(cRenderer &renderer, const StrPath &textureFilePath,
			char *skyboxTextureNames[6]);
		bool Create2(cRenderer &renderer, const StrPath &textureFilePath);
		void Render(cRenderer &renderer, const Matrix44 &tm = Matrix44::Identity);
		void RenderShader(cRenderer &renderer, const Matrix44 &tm = Matrix44::Identity);


	protected:
		bool CreateVertexBuffer(cRenderer &renderer);


	private:
		enum {FRONT, BACK, LEFT, RIGHT, TOP, BOTTOM, MAX_FACE};
		cVertexBuffer m_vtxBuff;
		cTexture *m_textures[ MAX_FACE];
		cShader *m_shader; // reference
	};


}
