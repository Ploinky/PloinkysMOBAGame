#include <GameObject.h>

namespace PMG {
	CAnimationComponent::CAnimationComponent() {
		m_strAnimationName = "";
		m_bLoop = false;
		m_fAnimationTime = 0;
	}

	CAnimationComponent::CAnimationComponent(std::string strAnimationName, bool bLoop) {
		m_strAnimationName = strAnimationName;
		m_bLoop = bLoop;
		m_fAnimationTime = 0;
	}

	void CAnimationComponent::Update(float fTime) {
		m_fAnimationTime += fTime;
	}

	bool CAnimationComponent::DoLoop() {
		return m_bLoop;
	}

	std::string CAnimationComponent::GetAnimationName() {
		return m_strAnimationName;
	}

	float CAnimationComponent::GetAnimationTime() {
		return m_fAnimationTime;
	}
}