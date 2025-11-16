#include "animation-system.h"

#include "game/client-game-state.h"
#include "game/components/components.h"
#include "Model.h"

CAnimationSystem::CAnimationSystem(CClientAssetManager* pAssetManager) {
    m_pAssetManager = pAssetManager;
}

void DoThingsWithBones(Armature* skin, int i, std::map<int, BonePosition>& bonePositions, std::vector<mat_t>& boneTransforms) {
	const Bone& bone = skin->bones[i];
	BonePosition position = bonePositions[bone.Index];

	mat_t localTransform = PMathMatAffineTransform(
	    Vector3(1, 1, 1),
		Vector3(0, 0, 0),
		position.rotation,
		position.translation
	);

	if (bone.parent_index == -1) {
		boneTransforms[i] = localTransform;
	} else {
		for(int pi = 0; pi < skin->bones.size(); pi++) {
			const Bone& b = skin->bones[pi];
			if(b.Index == bone.parent_index) {
				boneTransforms[i] = boneTransforms[pi] * localTransform;
				break;
			}
		}
	}

	for(int otherBone = 0; otherBone < skin->bones.size(); otherBone++) {
		if(skin->bones[otherBone].parent_index == bone.Index) {
			DoThingsWithBones(skin, otherBone, bonePositions, boneTransforms);
		}
	}
}

void CAnimationSystem::Update(CClientGameState* pGameState, float fDelta) {
    for(const UnitId& id : pGameState->vecUnits) {
        AnimationComponent_t* pAnimComp = pGameState->GetAnimation(id);
        RenderableComponent_t* pRendercomp = pGameState->GetRenderable(id);

        if(pAnimComp == nullptr || pRendercomp == nullptr) {
            return;
        }

        HModel hModel = m_pAssetManager->LoadModel(pRendercomp->strRenderable);
        ModelAsset_t& modelAsset = m_pAssetManager->GetModel(hModel);

        if(modelAsset.pModel == nullptr) {
            Logger::Err("Failed to update animation component: model is not loaded");
            return;
        }

        for(const auto& modelNode : modelAsset.pModel->Nodes) {
            Mesh* mesh = modelAsset.pModel->Meshes.at(modelNode.second->Mesh);
            if(mesh == nullptr) {
                continue;
            }

            // TODO
            if(pAnimComp->m_strAnimationName.length() > 0) {
                auto animIt = modelAsset.pModel->Animations.find(pAnimComp->m_strAnimationName);

                if(animIt != modelAsset.pModel->Animations.end() && modelAsset.pModel->Skins.find(modelNode.second->Skin) != modelAsset.pModel->Skins.end()) {
                    Animation* animation = animIt->second;
                    Armature* skin = modelAsset.pModel->Skins.at(modelNode.second->Skin);
                    std::map<int, BonePosition> bonePositions = animation->GetBonePositions(pAnimComp->m_fAnimationTime, pAnimComp->m_bLoop);
                    std::vector<mat_t> boneTransforms(skin->bones.size());

                    for (size_t i = 0; i < skin->bones.size(); ++i) {
                        if(skin->bones[i].parent_index == -1) {
                            DoThingsWithBones(skin, i, bonePositions, boneTransforms);
                        }
                    }

                    // Apply inverse bind pose to get the final bone transformation
                    for (size_t i = 0; i < skin->bones.size(); ++i) {
                        boneTransforms[i] = boneTransforms[i] * PMathMatTranspose(skin->global_inverse_bind_poses[i]);
                    }

                    for(int i = 0; i < boneTransforms.size(); i++) {
                        pAnimComp->vecBones[i] = boneTransforms[i];
                    }
                } else {
                    for(int i = 0; i < 256; i++) {
                        pAnimComp->vecBones[i] = PMathMatIdentity();
                    }
                }
            }
        }
    }
}