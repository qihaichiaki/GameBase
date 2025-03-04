#pragma once

#include <game_object/component/Rigidbody2D.h>

#include <vector>

/**
 * 刚体管理器
 */

namespace gameaf {
class RigidbodyManager
{
public:
    static RigidbodyManager& GetInstance()
    {
        static RigidbodyManager instance;
        return instance;
    }

    void OnFixedUpdate(float delta)
    {
        for (auto rigidbody2d : m_rigidbody2ds) {
            rigidbody2d->OnUpdate(delta);
        }
    }

public:
    Rigidbody2D* CreateRigidbody2D(GameObject* game_object)
    {
        auto rigidbody2D = new Rigidbody2D(game_object);
        m_rigidbody2ds.push_back(rigidbody2D);
        return rigidbody2D;
    }

    Rigidbody2D* CopyRigidbody2D(Rigidbody2D* cop_rigidbody2D)
    {
        if (cop_rigidbody2D == nullptr) return nullptr;
        auto rigidbody2D = new Rigidbody2D(*cop_rigidbody2D);
        m_rigidbody2ds.push_back(rigidbody2D);
        return rigidbody2D;
    }

    void DeleteRigidbody2D(Rigidbody2D* del_rigidbody2D)
    {
        if (del_rigidbody2D == nullptr) return;

        m_rigidbody2ds.erase(
            std::remove(m_rigidbody2ds.begin(), m_rigidbody2ds.end(), del_rigidbody2D),
            m_rigidbody2ds.end());
        delete del_rigidbody2D;
    }

private:
    RigidbodyManager() = default;
    RigidbodyManager(const RigidbodyManager&) = delete;
    RigidbodyManager& operator=(const RigidbodyManager&) = delete;

    ~RigidbodyManager()
    {
        for (auto rigidbody2d : m_rigidbody2ds) {
            delete rigidbody2d;
        }
    }

private:
    std::vector<Rigidbody2D*> m_rigidbody2ds;
};

}  // namespace gameaf