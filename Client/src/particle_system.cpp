#include "particle_system.h"
#include "direct3d.h"
#include "particle_shader.h"
#include <math.h>
#include "camera.h"
#include "DDSTextureLoader11.h"

namespace PMG {
	ParticleSystem::ParticleSystem(std::string texture_name) : texture_name_(texture_name) {
		life = 0;

		has_healthbar = false;
		has_title = false;
	}

	ParticleSystem::~ParticleSystem() {
		if (vertex_buffer_) {
			vertex_buffer_->Release();
			instance_buffer_ = nullptr;
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
	void ParticleSystem::Update(double dt) {
		if (attached_to_ != nullptr) {
			position = attached_to_->position;
			position.y += 1;
			// rotation = attached_to_->rotation;
		}

		life += dt;

		for (auto particle = particles.begin(); particle != particles.end(); particle++) {
			particle->position.x += (dt / 1000.0) * particle->velocity.x;
			particle->position.y += (dt / 1000.0) * particle->velocity.y;
			particle->position.z += (dt / 1000.0) * particle->velocity.z;

			particle->lifetime += dt;

			// particle->velocity.y -= (9.81 * dt);
		}

		std::erase_if(particles, [](auto kv) { return kv.lifetime > 300; } );

		test += dt;
		if (life < system_lifetime && test > 0.2 && particles.size() < particle_count) {
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

		if (life > system_lifetime && particles.size() == 0) {
			destroy = true;
		}
	}

	void ParticleSystem::Attach(GameObject* other) {
		attached_to_ = other;
	}

	void ParticleSystem::Render(Renderer* renderer) {
		renderer->BindShader(ShaderType::PARTICLE);

		float rotY = atan2(position.x - renderer->camera->position.x, position.z - renderer->camera->position.z);
		float rotX = -atan2(position.y - renderer->camera->position.y, position.z - renderer->camera->position.z);
		float rotZ = 0; // no camera roll
		
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
		DirectX::XMStoreFloat4x4(&model_data.modelMatrix, DirectX::XMMatrixTranspose(rotMat * transMat));

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

		unsigned int strides[2] { sizeof(particle_shader_vertex_t), sizeof(particle_instance_data_t) };
		unsigned int offsets[2] { 0, 0 }	;
		ID3D11Buffer* buffers[2] { vertex_buffer_, instance_buffer_ };

		// Render this specific model
		renderer->SetVertexBuffers(buffers, strides, offsets);
		renderer->SetShaderResource(0, 1, texture_);
		renderer->EnableAlphaBlending();
		renderer->DrawInstanced(6, instances.size());
		renderer->DisableAlphaBlending();
	}

	bool ParticleSystem::Initialize(Direct3D* direct3D) {
		particle_shader_vertex_t vertices[6]{
			{ {-0.1, 0.1, 0}, {0, 0} },
			{ {0.1, 0.1, 0}, {1, 0} },
			{ {0.1, -0.1, 0}, {1, 1} },
			{ {-0.1, 0.1, 0}, {0, 0} },
			{ {0.1, -0.1, 0}, {1, 1} },
			{ {-0.1, -0.1, 0}, {0, 1} },
		};
		int vertex_count = 6;
		vertex_buffer_ = direct3D->CreateVertexBuffer(vertices, vertex_count, sizeof(particle_shader_vertex_t) * vertex_count);

		if (vertex_buffer_ == nullptr) {
			initialized = false;
			return false;
		}

		particle_instance_data_t* instances = new particle_instance_data_t[particle_count] {};
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