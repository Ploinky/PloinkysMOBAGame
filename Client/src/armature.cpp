#include "armature.h"
#include <fstream>
#include "logger.h"

namespace PMG {
	Physics::mat_t BonePosition::ToMatrix() const {
        return Physics::mat_t::Translation(translation.x, translation.y, translation.z) * Physics::mat_t::Rotation(rotation);
	};

	void Armature::ComputeGlobalInverseBindPoses() {
		global_inverse_bind_poses.resize(bones.size());

        global_inverse_bind_poses[0] = bones[0].bind_pose.ToMatrix();

		for (int i = 1; i < global_inverse_bind_poses.size(); i++) {
            Physics::mat_t local_mat = bones[i].bind_pose.ToMatrix();
			global_inverse_bind_poses[i] = local_mat * global_inverse_bind_poses[bones[i].parent_index];
		}

		for (int i = 0; i < global_inverse_bind_poses.size(); i++) {
			global_inverse_bind_poses[i] = global_inverse_bind_poses[i].inverse();
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

            b.bind_pose.rotation.x = quaternion[0];
            b.bind_pose.rotation.y = quaternion[1];
            b.bind_pose.rotation.z = quaternion[2];
            b.bind_pose.rotation.w = quaternion[3];

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