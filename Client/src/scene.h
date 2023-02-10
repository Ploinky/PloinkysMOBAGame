#pragma once

#include <list>
#include <string>
#include <vector>
#include "navigation.h"
#include "physics.h"
#include "networking.h"

namespace PMG {
	class Mesh;
	class NetworkConnection;
	class Map;
	class Renderer;
	class Window;
	class Scene;
	class ClientStateHandler;
	class GuiElement;
	class GuiTextfield;

	typedef struct {
		vec2_t pos;
    float rot;
		unsigned long unitId;
	} unit_t;

	typedef struct {
		unsigned long index;
		long long received;
		std::list<unit_t> units;
	} game_tick_t;
	
	class Scene {
	public:
		Scene(ClientStateHandler* stateHandler) : m_stateHandler(stateHandler) {};
		virtual void Update(float dt) {};
		virtual void Render(Renderer* renderer) {};
		virtual void CharTyped(uint16_t ch) {};
		virtual void KeyPressed(uint16_t key) {};
		virtual void KeyReleased(uint16_t key) {};
		virtual void MouseMoved(int screenX, int screenY) {};
		virtual void MouseButtonPressed(int button) {};
		virtual void MouseButtonReleased(int button) {};

		ClientStateHandler* m_stateHandler = 0;

		int m_sceneWidth = 0;
		int m_sceneHeight = 0;
	};

	class MainMenuScene : public Scene {
	public:
		MainMenuScene(ClientStateHandler* stateHandler);

		void Update(float dt);
		void Render(Renderer* renderer);
		void CharTyped(uint16_t ch);
		void KeyPressed(uint16_t key);
		void KeyReleased(uint16_t key);
		void MouseMoved(int screenX, int screenY);
		void MouseButtonPressed(int button);
		void MouseButtonReleased(int button);
		std::wstring GetIp();

		GuiElement* rootGuiElement;
		GuiTextfield* m_textfield;
		int m_mousePos[2]{ 0 };
	};

	class ConnectScene : public Scene {
	public:
		ConnectScene(ClientStateHandler* stateHandler, std::string ip);

		void Update(float dt);
		void Render(Renderer* renderer);
		void CharTyped(uint16_t ch);
		void KeyPressed(uint16_t key);
		void KeyReleased(uint16_t key);
		void MouseMoved(int screenX, int screenY);
		void MouseButtonPressed(int button);
		void MouseButtonReleased(int button);
    net_client_t GetConnection();
		std::wstring m_text;

    private:
		  net_client_t m_netConnection;
	};

	class NetworkedGame : public Scene {
	public:
		NetworkedGame(ClientStateHandler* stateHandler, net_client_t connection);
		~NetworkedGame();

		std::vector<Mesh*> models;
		NavMesh* m_navMesh;

		net_client_t m_netConnection;

		void Update(float dt);
		void Render(Renderer* renderer);

		void HandleNetworkMessage(packet_t* packet);
		void HandleTicks(long long frameTime);

		void SpawnUnit(unsigned long id);
		void DespawnUnit(unsigned long id);
		void CharTyped(uint16_t ch);
		void KeyPressed(uint16_t key);
		void KeyReleased(uint16_t key);
		void MouseMoved(int screenX, int screenY);
		void MouseButtonPressed(int button);
		void MouseButtonReleased(int button);

		void TestIntersect(Renderer* renderer, int mx, int my, float* x, float* y);

		Mesh* GetModelForUnit(unsigned long untiId);

		Map* m_map;

		std::vector<game_tick_t> ticks;
		std::list<unit_t> units;

		std::string ip;
		int fps;

		int m_mouseClicked[3] = {0, 0, 0};
		int m_camDir[2] = {0, 0};
		float m_camPos[3] = {0, 0, 0};

		// Keyboard input
		bool m_keys[0xFF]{ 0 };

		// Mouse input
		bool m_mouseButtons[3]{ 0 };
		int m_mousePos[2]{ 0 };
	};
}