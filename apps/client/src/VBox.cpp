#include "Gui.h"
#include "Renderer.h"
#include <common/PMG_Common.h>

void VBox::LayoutChildren() {
	if (m_children.empty()) {
		return;
	}

	// Horizontal maximum size of children cannot be more than this boxes size
	// Vertical should try to find space for all children
	// -> prefSized children get priority over prefSizeLess children
	// -> if all children have prefSize and can fit, including gaps, render them centered
	// -> if no gaps -> space evenly
	// -> keep relative prefsizes?!

	float totalPrefHeight = 0;
	int noPrefHeightCount = 0;

	for (auto el : m_children) {
		totalPrefHeight += el->m_prefSize.y;

		// Avoid another loop later...
		if (el->m_prefSize.y == 0) {
			noPrefHeightCount++;
		}

		// Width is always the smaller value of preferred width and my max width
		// If no preferred width is given, grow to max width!
		// Careful, now it's size[0]...
		el->m_size.x = el->m_prefSize.x != 0 ? std::min(el->m_prefSize.x, m_size.x) : m_size.x;
	}

	if (m_gap != 0) {
		totalPrefHeight += m_gap * static_cast<int>(m_children.size()) + 1;
	}

	if (totalPrefHeight > m_size.y) {
		// Oh god oh fuck WTF do i do now?
		Logger::Err("I did not come prepared for this!");
		return;
	}

	for (auto el : m_children) {
		if (el->m_prefSize.y != 0) {
			el->m_size.y = el->m_prefSize.y;
		}
	}

	float othersHeight = noPrefHeightCount > 0 ? (m_size.y - totalPrefHeight) / noPrefHeightCount : 0;

	for (auto el : m_children) {
		if (el->m_prefSize.y == 0) {
			el->m_size.y = othersHeight;
		}
	}

	float totalHeight = totalPrefHeight + othersHeight * noPrefHeightCount + m_gap * noPrefHeightCount;

	float gap = static_cast<float>(m_gap);

	// Is this how we should be autogapping???
	if (gap == 0) {
		gap = (m_size.y - totalHeight) / (m_children.size() + 1);
	}

	// Now set positions... jesus h roosevelt christ
	float currPos = gap;

	// Start first item at offset depending on how much space the other elements use
	// -> what a horrible attempt at centering elements, hey?
	if (totalHeight > 0 && m_gap != 0) {
		currPos += (m_size.y - totalHeight) / 2.0f;
	}

	for (auto el : m_children) {
		el->m_pos.x = (m_size.x - el->m_size.x) / 2.0f + m_pos.x;
		el->m_pos.y = currPos;
		currPos += gap + el->m_size.y;
	}

	for (auto el : m_children) {
		el->LayoutChildren();
	}
}

void VBox::Render(CRenderer* renderer) {
	for (auto el : m_children) {
		el->Render(renderer);
	}
}
