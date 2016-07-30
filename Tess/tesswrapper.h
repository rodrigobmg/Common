//
// 2016-07-27, jjuiddong
//
// tesseract 래핑 클래스
// tesseract를 그대로 사용하면, 컴파일 오류가 생겨, 따로 프로젝트로 만들었다.
//

#pragma once

#include <opencv2/highgui/highgui.hpp>


namespace tesseract {
	class TessBaseAPI;
}


namespace tess
{

	class cTessWrapper
	{
	public:
		cTessWrapper();
		virtual ~cTessWrapper();
		bool Init(const string &dataPath, const string &language);
		string Recognize(cv::Mat &img);


	public:
		tesseract::TessBaseAPI *m_tessApi;
	};

}