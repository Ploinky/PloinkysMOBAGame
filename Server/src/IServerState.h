#pragma once

namespace PMG {
	class IServerState {
	public:
		virtual void Update(float dt) = 0;
	};
}