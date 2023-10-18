#include "Armature.h"
#include <fstream>
#include "common/logger.h"

namespace PMG {
	DirectX::XMMATRIX BonePosition::ToMatrix() const {
        return DirectX::XMMatrixRotationQuaternion(rotation) * DirectX::XMMatrixTranslation(translation.x, translation.y, translation.z);
	};

    BonePosition BonePosition::Interpolate(BonePosition from, BonePosition to, double pct) {
        assert(pct <= 1);
        assert(pct >= 0);

        BonePosition ret;

        ret.rotation = DirectX::XMQuaternionSlerp(from.rotation, to.rotation, pct);

        DirectX::XMVECTOR vec{
            std::lerp(DirectX::XMVectorGetX(from.rotation), DirectX::XMVectorGetX(to.rotation), pct),
            std::lerp(DirectX::XMVectorGetY(from.rotation), DirectX::XMVectorGetY(to.rotation), pct),
            std::lerp(DirectX::XMVectorGetZ(from.rotation), DirectX::XMVectorGetZ(to.rotation), pct),
            std::lerp(DirectX::XMVectorGetW(from.rotation), DirectX::XMVectorGetW(to.rotation), pct)
        };

        ret.rotation = vec;

        ret.translation = DirectX::XMFLOAT3(
                std::lerp(from.translation.x, to.translation.x, pct),
                std::lerp(from.translation.y, to.translation.y, pct),
                std::lerp(from.translation.z, to.translation.z, pct)
            );

        return ret;
    }

	void Armature::ComputeGlobalInverseBindPoses() {
		global_inverse_bind_poses.resize(bones.size());

        global_inverse_bind_poses[0] = bones[0].bind_pose.ToMatrix();

		for (int i = 1; i < global_inverse_bind_poses.size(); i++) {
            DirectX::XMMATRIX local_mat = bones[i].bind_pose.ToMatrix();
			global_inverse_bind_poses[i] = local_mat * global_inverse_bind_poses[bones[i].parent_index];
		}

		for (int i = 0; i < global_inverse_bind_poses.size(); i++) {
			global_inverse_bind_poses[i] = DirectX::XMMatrixInverse(nullptr, global_inverse_bind_poses[i]);
		}
	}

	Armature* Armature::LoadArmature(std::string file_name) {
        std::ifstream file(file_name, std::ios_base::binary | std::ios_base::in);

        if (!file.is_open()) {
            Logger::Err("Could not open map file.");
            return nullptr;
        }

        Armature* armature = new Armature();
        int currIndex = 0;
        int currVertex = 0;

        std::string correct_magic = "p3d";
        char magic[4]{ 0, 0, 0, 0 };
        file.read(magic, sizeof(char) * 3);
        std::string magic_string(magic);
        if (magic_string.compare(correct_magic) != 0) {
            Logger::Err("Bad magic string!");
            return nullptr;
        }

        int version;
        file.read((char*)&version, sizeof(int));

        if (version != 1) {
            Logger::Err("Bad version!");
            return nullptr;
        }

        int bone_count;
        file.read((char*)&bone_count, sizeof(int));

        armature->bones.resize(bone_count);

        for (int i = 0; i < bone_count; i++) {
            int name_length;
            file.read((char*)&name_length, sizeof(int));

            char* name = new char[name_length + 1] {0};

            file.read(name, name_length);
            Bone b = Bone();
            b.name = std::string(name);

            delete[] name;

            int parent;
            file.read((char*)&parent, sizeof(int));
            b.parent_index = parent;

            float quaternion[4];
            file.read((char*)&quaternion[0], sizeof(float));
            file.read((char*)&quaternion[1], sizeof(float));
            file.read((char*)&quaternion[2], sizeof(float));
            file.read((char*)&quaternion[3], sizeof(float));

            b.bind_pose.rotation = DirectX::XMVectorSetX(b.bind_pose.rotation, quaternion[0]);
            b.bind_pose.rotation = DirectX::XMVectorSetY(b.bind_pose.rotation, quaternion[1]);
            b.bind_pose.rotation = DirectX::XMVectorSetZ(b.bind_pose.rotation, quaternion[2]);
            b.bind_pose.rotation = DirectX::XMVectorSetW(b.bind_pose.rotation, quaternion[3]);

            float translation[3];
            file.read((char*)&translation[0], sizeof(float));
            file.read((char*)&translation[1], sizeof(float));
            file.read((char*)&translation[2], sizeof(float));

            b.bind_pose.translation.x = translation[0];
            b.bind_pose.translation.y = translation[1];
            b.bind_pose.translation.z = translation[2];

            armature->bones[i] = b;
        }

        return armature;
	}
}