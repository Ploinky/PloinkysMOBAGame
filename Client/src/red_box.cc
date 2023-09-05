#include "red_box.h"
#include <map>
#include <string>
#include <list>

namespace PMG {
	RedBox::RedBox() {
        mesh = "chess_person";

        health = 50;
        max_health = 100;
        position = { 0, 0, 0 };
        rotation = { 0, 0, 0 };
	}
}