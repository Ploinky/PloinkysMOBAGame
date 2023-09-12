#include "animation.h"
#include <fstream>
#include "logger.h"

namespace PMG {
	Animation* Animation::LoadAnimation(std::string file_name) {
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

        Animation* animation = new Animation();

        file.read((char*)&animation->frame_count, sizeof(int));

        float duration;
        file.read((char*)&duration, sizeof(float));

        animation->duration = duration;

        file.read((char*)&animation->bone_count, sizeof(int));

        animation->animation_tracks.resize(animation->bone_count);

        for (int i = 0; i < animation->bone_count; i++) {
            animation->animation_tracks[i].resize(animation->frame_count);
        }

        for (int frame = 0; frame < animation->frame_count; frame++) {
            for (int bone = 0; bone < animation->bone_count; bone++) {
                float quaternion[4];
                float translation[3];

                file.read((char*)&quaternion[0], sizeof(float));
                file.read((char*)&quaternion[1], sizeof(float));
                file.read((char*)&quaternion[2], sizeof(float));
                file.read((char*)&quaternion[3], sizeof(float));
                file.read((char*)&translation[0], sizeof(float));
                file.read((char*)&translation[1], sizeof(float));
                file.read((char*)&translation[2], sizeof(float));

                BonePosition pose = BonePosition();
                pose.rotation = DirectX::XMVectorSetX(pose.rotation, quaternion[0]);
                pose.rotation = DirectX::XMVectorSetY(pose.rotation, quaternion[1]);
                pose.rotation = DirectX::XMVectorSetZ(pose.rotation, quaternion[2]);
                pose.rotation = DirectX::XMVectorSetW(pose.rotation, quaternion[3]);
                pose.translation.x = translation[0];
                pose.translation.y = translation[1];
                pose.translation.z = translation[2];
                animation->animation_tracks[bone][frame] = pose;
            }
        }

        return animation;
	}

    void Animation::GetGlobalPoseAtTime(std::vector<DirectX::XMMATRIX>& poses, const Armature* armature, double time) {
        if (poses.size() != bone_count) {
            poses.resize(bone_count);
        }

        const int frame = time;

        if (animation_tracks[0].size() > 0) {
            poses[0] = animation_tracks[0][frame].ToMatrix();
        }
        else {
            poses[0] = DirectX::XMMatrixIdentity();
        }

        for (int bone = 1; bone < bone_count; bone++) {
            DirectX::XMMATRIX mat = DirectX::XMMatrixIdentity();

            if (animation_tracks[bone].size() > 0) {
                mat = animation_tracks[bone][frame].ToMatrix();
            }

            poses[bone] = mat * poses[armature->bones[bone].parent_index];
        }
    }
}