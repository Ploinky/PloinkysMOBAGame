#pragma once

#include <list>
#include <map>
#include <typeindex>
#include <typeinfo>
#include <any>
#include <stdint.h>

namespace PMG {
    typedef uint32_t entity_id;

    typedef struct {
        float x;
        float y;
        float r;
        float tx;
        float ty;
    } transform_t;

    typedef struct {
        unsigned int netId;
    } network_t;

    class IComponentArray {
    public:
        virtual void Remove(entity_id id) {};
    };

    template <typename T>
    class ComponentArray : IComponentArray {
    public:
        typedef std::map<entity_id, T> mapType;
        mapType entries;
        void Remove(entity_id id) {
            entries.erase(id);
        }
    };

    class ComponentRegistry {
    public:
        entity_id Create() {
            entity_id id = m_currentId++;
            m_entities.push_back(id);
            return id;
        }

        void Destroy(entity_id id) {
            for (auto e = m_entities.begin(); e != m_entities.end(); e++) {
                if (*e == id) {
                    m_entities.erase(e);
                    break;
                }
            }

            for (auto arr : components) {
                arr.second->Remove(id);
            }
        }

        template<typename T>
        T* GetComponent(entity_id id) {
            if (components.find(typeid(T).hash_code()) == components.end()) {
                return nullptr;
            }

            ComponentArray<T>* map = (ComponentArray<T>*)components.at(typeid(T).hash_code());

            if (map->entries.find(id) == map->entries.end()) {
                return nullptr;
            }

            return &map->entries.at(id);
        }

        template<typename T>
        void AddComponent(entity_id id) {
            if (components.find(typeid(T).hash_code()) == components.end()) {
                ComponentArray<T>* arr = new ComponentArray<T>();
                components.emplace(typeid(T).hash_code(), (IComponentArray*)arr);
            }

            ComponentArray<T>* map = (ComponentArray<T>*)components.at(typeid(T).hash_code());
            T t;
            map->entries.insert({ id, t });
        }

        template<typename T>
        void AddComponent(entity_id id, T value) {
            if (components.find(typeid(T).hash_code()) == components.end()) {
                ComponentArray<T>* arr = new ComponentArray<T>();
                components.emplace(typeid(T).hash_code(), (IComponentArray*)arr);
            }

            ComponentArray<T>* map = (ComponentArray<T>*)components.at(typeid(T).hash_code());
            map->entries.insert({ id, value });
        }

        template<typename T>
        void RemoveComponent(entity_id id) {
            if (components.find(typeid(T).hash_code()) == components.end()) {
                return;
            }

            ComponentArray<T>* map = (ComponentArray<T>*)components.at(typeid(T).hash_code());

            if (map->entries.find(id) != map->entries.end()) {
                map->entries.erase(map->entries.find(id));
            }
        }

        template<typename T>
        std::list<entity_id> GetEntities() {
            if (components.find(typeid(T).hash_code()) == components.end()) {
                ComponentArray<T>* arr = new ComponentArray<T>();
                components.emplace(typeid(T).hash_code(), (IComponentArray*)arr);
            }

            ComponentArray<T>* map = (ComponentArray<T>*)components.at(typeid(T).hash_code());

            std::list<entity_id> keys;

            for (auto x : map->entries) {
                keys.push_back(x.first);
            }
            return keys;
        }
    private:
        std::list<entity_id> m_entities;

        std::map<std::size_t, IComponentArray*> components;
        entity_id m_currentId = 0;
    };
}