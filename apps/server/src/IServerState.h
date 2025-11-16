#pragma once

class IServerStateHandler;

class IServerState {
public:
	IServerState(IServerStateHandler* handler) : handler_(handler) {};
	virtual void Update(float dt) = 0;

	IServerStateHandler* handler_;
};