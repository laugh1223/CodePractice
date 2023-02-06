#pragma once

#include <map>
#include <string>

class MediaSession;

class MediaSessionManager
{
public:
	static MediaSessionManager* createNew();
	MediaSessionManager();
	~MediaSessionManager();
	bool addSession(MediaSession* session);
	bool removeSession(MediaSession* session);
	MediaSession* getSession(const std::string& name);

private:
	std::map<std::string, MediaSession*> mSessMap;//会话对象集合
};