#include "particle_system.h"
#include "direct3d.h"
#include "particle_shader.h"
#include <math.h>
#include "camera.h"
namespace PMG {
	ParticleSystem::ParticleSystem() {
		life_time = 1000;
		life = 0;
	}

	void ParticleSystem::Update(double dt) {
		dt /= 1000.0;
		life += (dt);

		for (auto particle = particles.begin(); particle != particles.end(); particle++) {
			particle->position.x += dt * particle->velocity.x;
			particle->position.y += dt * particle->velocity.y;
			particle->position.z += dt * particle->velocity.z;

			particle->lifetime += dt;
		}

		std::erase_if(particles, [](auto kv) { return kv.lifetime > 1; } );

		particles.push_back({ {0, 0, 0}, {1, 0, 0, 1}, { (rand() % 200 - 100) / 100.0, (rand() % 100) / 10.0, (rand() % 200 - 100) / 100.0 }, 0 });
	}

	void ParticleSystem::Render(Renderer* renderer) {
		renderer->BindShader(ShaderType::PARTICLE);

		particle_shader_frame_const_t data{};
		data.cameraMatrix = renderer->cameraMatrix;
		data.projMatrix = renderer->m_projMatrix;

		renderer->UpdateShaderConst<particle_shader_frame_const_t>(data);

		// Calculate the rotation that needs to be applied to the billboard model to face the current camera position using the arc tangent function.
		double rotY = atan2(position.x - renderer->camera->position.x, position.z - renderer->camera->position.z) * (180.0 / M_PI);
		double rotX = atan2(position.y - renderer->camera->position.y, position.z - renderer->camera->position.z) * (180.0 / M_PI);
		double rotZ = rotation.z; // atan2(position.x - renderer->camera->position.x, position.y - renderer->camera->position.y) * (180.0 / M_PI);

		particle_shader_model_const_t model_data{};
		DirectX::XMMATRIX rotMat = DirectX::XMMatrixRotationRollPitchYaw(DirectX::XMConvertToRadians(-rotX),
			DirectX::XMConvertToRadians(rotY),
			DirectX::XMConvertToRadians(rotZ));
		DirectX::XMMATRIX transMat = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
		DirectX::XMStoreFloat4x4(&model_data.modelMatrix, DirectX::XMMatrixTranspose(rotMat * transMat));

		renderer->UpdateShaderConst<particle_shader_model_const_t>(model_data);

		std::vector<particle_instance_data_t> instances;
	
		for (Particle particle : particles) {
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
		renderer->DrawInstanced(6, instances.size());
	}

	bool ParticleSystem::Initialize(Direct3D* direct3D) {
		particle_shader_vertex_t vertices[6]{
			{ {-0.1, 0.1, 0}, {1, 0, 0, 1} },
			{ {0.1, 0.1, 0}, {1, 0, 0, 1} },
			{ {0.1, -0.1, 0}, {1, 0, 0, 1} },
			{ {-0.1, 0.1, 0}, {1, 0, 0, 1} },
			{ {0.1, -0.1, 0}, {1, 0, 0, 1} },
			{ {-0.1, -0.1, 0}, {1, 0, 0, 1} },
		};
		int vertex_count = 6;
		vertex_buffer_ = direct3D->CreateVertexBuffer(vertices, vertex_count, sizeof(particle_shader_vertex_t) * vertex_count);

		if (vertex_buffer_ == nullptr) {
			initialized = false;
			return false;
		}

		particle_instance_data_t instances[10000] {
			{ 0, 0, 0},
			{ 0, 1, 0},
			{ 1, 0, 0},
			{ 1, 1, 0},
		};
		instance_buffer_ = direct3D->CreateInstanceBuffer(instances, 1000, sizeof(particle_instance_data_t));

		if (instance_buffer_ == nullptr) {
			initialized = false;
			return false;
		}

		initialized = true;

		return initialized;
	}
}