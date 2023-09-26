#include "particle_emitter.h"
#include "particle_shader.h"
#include "direct3d.h"
#include "camera.h"
#include "DDSTextureLoader11.h"
#include "util.h"

namespace PMG {
	ParticleEmitter::ParticleEmitter(std::string texture_name) : texture_name_(texture_name) {
		emitter_life = 0;
	};

	ParticleEmitter* ParticleEmitter::Load(std::string file_name) {
		std::list<std::string> lines = Util::ReadLinesFromFile(file_name);

		ParticleEmitter* particle_emitter = new ParticleEmitter("");

		for (std::string line : lines) {
			std::string token = line.substr(0, line.find("="));

			if(token.compare("texture_file") == 0) {
				particle_emitter->texture_name_ = line.substr(line.find("=") + 1);
			}
			else if (token.compare("particle_velocity_x") == 0) {
				particle_emitter->particle_velocity.x = std::stof(line.substr(line.find("=") + 1));
			}
			else if (token.compare("particle_velocity_y") == 0) {
				particle_emitter->particle_velocity.y = std::stof(line.substr(line.find("=") + 1));
			}
			else if (token.compare("particle_velocity_z") == 0) {
				particle_emitter->particle_velocity.z = std::stof(line.substr(line.find("=") + 1));
			}
			else if (token.compare("particle_velocity_range_x") == 0) {
				particle_emitter->particle_velocity_range.x = std::stof(line.substr(line.find("=") + 1));
			}
			else if (token.compare("particle_velocity_range_y") == 0) {
				particle_emitter->particle_velocity_range.y = std::stof(line.substr(line.find("=") + 1));
			}
			else if (token.compare("particle_velocity_range_z") == 0) {
				particle_emitter->particle_velocity_range.z = std::stof(line.substr(line.find("=") + 1));
			}
			else if (token.compare("particle_scale_x") == 0) {
				particle_emitter->particle_scale.x = std::stof(line.substr(line.find("=") + 1));
			}
			else if (token.compare("particle_scale_y") == 0) {
				particle_emitter->particle_scale.y = std::stof(line.substr(line.find("=") + 1));
			}
			else if (token.compare("particle_scale_z") == 0) {
				particle_emitter->particle_scale.z = std::stof(line.substr(line.find("=") + 1));
			}
			else if (token.compare("particle_count") == 0) {
				particle_emitter->particle_count = std::stoi(line.substr(line.find("=") + 1));
			}
			else if (token.compare("emitter_lifetime") == 0) {
				particle_emitter->emitter_lifetime = std::stoi(line.substr(line.find("=") + 1));
			}
			else if (token.compare("static_angle_x") == 0) {
				particle_emitter->particle_angle.x = std::stof(line.substr(line.find("=") + 1));
				particle_emitter->static_angle = true;
			}
			else if (token.compare("static_angle_y") == 0) {
				particle_emitter->particle_angle.y = std::stof(line.substr(line.find("=") + 1));
				particle_emitter->static_angle = true;
			}
			else if (token.compare("static_angle_z") == 0) {
				particle_emitter->particle_angle.z = std::stof(line.substr(line.find("=") + 1));
				particle_emitter->static_angle = true;
			}
		}

		return particle_emitter;
	}

	ParticleEmitter::~ParticleEmitter() {
		if (vertex_buffer_) {
			vertex_buffer_->Release();
			vertex_buffer_ = nullptr;
		}

		if (instance_buffer_) {
			instance_buffer_->Release();
			instance_buffer_ = nullptr;
		}

		if (texture_) {
			texture_->Release();
			texture_ = nullptr;
		}
	}
	static double test = 0;

	void ParticleEmitter::Update(float dt) {
		for (auto particle = particles.begin(); particle != particles.end(); particle++) {
			particle->position.x += (dt / 1000.0) * particle->velocity.x;
			particle->position.y += (dt / 1000.0) * particle->velocity.y;
			particle->position.z += (dt / 1000.0) * particle->velocity.z;

			particle->lifetime += dt;

			// particle->velocity.y -= (9.81 * dt);
		}

		std::erase_if(particles, [](auto kv) { return kv.lifetime > 300; });

		test += dt;
		emitter_life += dt;

		if (emitter_life < emitter_lifetime && test > 0.2 && particles.size() < particle_count) {
			float velX = particle_velocity.x;

			if (particle_velocity_range.x != 0) {
				velX += ((rand() % (int)(particle_velocity_range.x * 100.0f)) / 100.0f) - particle_velocity_range.x / 2;

			}
			float velY = particle_velocity.y;

			if (particle_velocity_range.y != 0) {
				velY += ((rand() % (int)(particle_velocity_range.y * 100.0f)) / 100.0f) - particle_velocity_range.y / 2;

			}
			float velZ = particle_velocity.z;

			if (particle_velocity_range.z != 0) {
				velZ += ((rand() % (int)(particle_velocity_range.z * 100.0f)) / 100.0f) - particle_velocity_range.z / 2;

			}
			particles.push_back({ {0, 0, 0}, {1, 0, 0, 1}, { velX,velY, velZ } });
			//particles.push_back({ {0, 0, 0}, {1, 0, 0, 1}, { 0, (rand() % 100) / 10.0, 0 }, 0 });
			test = 0;
		}
	}

	bool ParticleEmitter::IsDone() {
		return emitter_life >= emitter_lifetime && particles.size() == 0;
	}

	void ParticleEmitter::Render(Renderer* renderer) {
		renderer->BindShader(ShaderType::PARTICLE);

		float rotY = atan2(position.x - renderer->camera->position.x, position.z - renderer->camera->position.z);
		float rotX = -atan2(position.y - renderer->camera->position.y, position.z - renderer->camera->position.z);
		float rotZ = 0; // no camera roll

		if (static_angle) {
			rotX = DirectX::XMConvertToRadians(particle_angle.x);
			rotY = DirectX::XMConvertToRadians(particle_angle.y);
			rotZ = DirectX::XMConvertToRadians(particle_angle.z);
		}

		particle_shader_frame_const_t data{};
		data.cameraMatrix = renderer->cameraMatrix;
		data.projMatrix = renderer->m_projMatrix;
		DirectX::XMStoreFloat4x4(&data.billboard_matrix, DirectX::XMMatrixTranspose(DirectX::XMMatrixRotationRollPitchYaw(rotX, rotY, rotZ)));

		renderer->UpdateShaderConst<particle_shader_frame_const_t>(data);

		particle_shader_model_const_t model_data{};
		DirectX::XMMATRIX rotMat = DirectX::XMMatrixRotationRollPitchYaw(DirectX::XMConvertToRadians(rotation.x),
			DirectX::XMConvertToRadians(rotation.y),
			DirectX::XMConvertToRadians(rotation.z));
		DirectX::XMMATRIX transMat = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
		DirectX::XMMATRIX scaleMat = DirectX::XMMatrixScaling(particle_scale.x, particle_scale.y, particle_scale.z);
		DirectX::XMStoreFloat4x4(&model_data.modelMatrix, DirectX::XMMatrixTranspose(scaleMat * rotMat * transMat));

		renderer->UpdateShaderConst<particle_shader_model_const_t>(model_data);

		std::vector<particle_instance_data_t> instances;

		for (const Particle& particle : particles) {
			particle_instance_data_t p;
			p.instance_position[0] = particle.position.x;
			p.instance_position[1] = particle.position.y;
			p.instance_position[2] = particle.position.z;
			instances.push_back(p);
		}

		D3D11_MAPPED_SUBRESOURCE mappedResource;
		// Lock the vertex buffer.
		HRESULT result = renderer->direct3D->context->Map(instance_buffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (FAILED(result))
		{
			throw std::exception();
		}

		// Get a pointer to the data in the vertex buffer.
		particle_instance_data_t* verticesPtr = (particle_instance_data_t*)mappedResource.pData;

		// Copy the data into the vertex buffer.
		memcpy(verticesPtr, (void*)instances.data(), (sizeof(particle_instance_data_t) * instances.size()));

		// Unlock the vertex buffer.
		renderer->direct3D->context->Unmap(instance_buffer_, 0);

		unsigned int strides[2]{ sizeof(particle_shader_vertex_t), sizeof(particle_instance_data_t) };
		unsigned int offsets[2]{ 0, 0 };
		ID3D11Buffer* buffers[2]{ vertex_buffer_, instance_buffer_ };

		// Render this specific model
		renderer->SetVertexBuffers(buffers, strides, offsets);
		renderer->SetShaderResource(0, 1, texture_);
		renderer->EnableAlphaBlending();
		renderer->DrawInstanced(6, instances.size());
		renderer->DisableAlphaBlending();
	}

	bool ParticleEmitter::Initialize(Direct3D* direct3D) {
		particle_shader_vertex_t vertices[6]{
			{ {-1, 1, 0}, {0, 0} },
			{ {1, 1, 0}, {1, 0} },
			{ {1, -1, 0}, {1, 1} },
			{ {-1, 1, 0}, {0, 0} },
			{ {1, -1, 0}, {1, 1} },
			{ {-1, -1, 0}, {0, 1} },
		};
		int vertex_count = 6;
		vertex_buffer_ = direct3D->CreateVertexBuffer(vertices, vertex_count, sizeof(particle_shader_vertex_t) * vertex_count);

		if (vertex_buffer_ == nullptr) {
			initialized = false;
			return false;
		}

		particle_instance_data_t* instances = new particle_instance_data_t[particle_count]{};
		instance_buffer_ = direct3D->CreateInstanceBuffer(instances, particle_count, sizeof(particle_instance_data_t));

		if (instance_buffer_ == nullptr) {
			initialized = false;
			return false;
		}

		HRESULT hr = DirectX::CreateDDSTextureFromFile(direct3D->device, std::wstring(texture_name_.begin(), texture_name_.end()).c_str(), NULL, &texture_, 0, NULL);

		if (FAILED(hr)) {
			return false;
		}

		initialized = true;

		return initialized;
	}
}