#pragma once

namespace PMG::Physics {
	class Line {
	public:
		Line() {};
		Line(Vector2 start, Vector2 end) : Start(start), End(end) {};
		Vector2 Start;
		Vector2 End;
	};
}