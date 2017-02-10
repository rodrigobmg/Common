#pragma once


namespace common
{

	string GetFilePathExceptFileName(const std::string &fileName);
	string GetFileExt(const string &fileName);
	string GetFullFileName(const string &fileName);
	string GetFileName(const string &fileName);
	string GetFileNameExceptExt(const string &fileName);
	string RelativePathTo(const string &pathFrom, const string &pathTo);
	wstring RelativePathToW(const wstring &pathFrom, const wstring &pathTo);
	bool IsRelativePath(const string &path);
	string DeleteCurrentPath(const string &fileName);
	__int64 FileSize(const string &fileName);
	string RemoveFileExt(const string &fileName);
	string CheckDirectoryPath(const string &fileName);



	bool CollectFiles( const list<string> &findExt, const string &searchPath, OUT list<string> &out);
	bool CollectFiles2(const list<string> &findExt, const string &searchPath, const string &relativePath, OUT list<string> &out);
	bool CollectFilesOrdered(const list<string> &findExt, const string &searchPath, OUT list<string> &out, const int flags=0);
	bool FindFile( const string &findName, const string &searchPath, string &out  );

	bool IsFileExist(const string &fileName);

	struct sFolderNode
	{
		map<string, sFolderNode*> children;
		list< std::pair<string, sFolderNode*> > childrenFiles; // for Display Order of Folder and Files
		int data; // special data
	};
	sFolderNode* CreateFolderNode(const list<string> &fileList);
	void DeleteFolderNode(sFolderNode *node);

	int FileOperationFunc(unsigned int func, const string &to, const string &from);

}