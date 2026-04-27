#include <vector>

#include "CGL/vector2D.h"

#include "mass.h"
#include "rope.h"
#include "spring.h"

namespace CGL {

    Rope::Rope(Vector2D start, Vector2D end, int num_nodes, float node_mass, float k, vector<int> pinned_nodes)
    {
        Vector2D step = (end - start) / (num_nodes - 1);
        for (int i = 0; i < num_nodes; ++i)
        {
            masses.push_back(new Mass(start + step * i, node_mass, false));
            if (i > 0)
            {
                springs.push_back(new Spring(masses[i - 1], masses[i], k));
            }
        }

        for (auto &i : pinned_nodes) {
            masses[i]->pinned = true;
        }
    }

    void Rope::simulateEuler(float delta_t, Vector2D gravity)
    {
        for (auto &s : springs)
        {
            Vector2D direction = s->m2->position - s->m1->position;
            double length = direction.norm();
            Vector2D force = s->k * (length - s->rest_length) * direction.unit();
            s->m1->forces += force;
            s->m2->forces -= force;
        }

        for (auto &m : masses)
        {
            if (!m->pinned)
            {
                m->forces += gravity * m->mass;
                Vector2D acceleration = m->forces / m->mass;
                m->velocity += acceleration * delta_t;
                m->position += m->velocity * delta_t;

                m->velocity *= 0.99;
            }

            m->forces = Vector2D(0, 0);
        }
    }

    void Rope::simulateVerlet(float delta_t, Vector2D gravity)
    {
        for (auto &s : springs)
        {
            Vector2D direction = s->m2->position - s->m1->position;
            double length = direction.norm();
            Vector2D force = s->k * (length - s->rest_length) * direction.unit();

            s->m1->forces += force;
            s->m2->forces -= force;
        }

        for (auto &m : masses)
        {
            if (!m->pinned)
            {
                Vector2D temp_position = m->position;
                Vector2D acceleration = m->forces / m->mass + gravity;

                m->position = m->position + (m->position - m->last_position) + acceleration * delta_t * delta_t;
                m->position = temp_position + (m->position - temp_position) * 0.99;
                m->last_position = temp_position;
            }

            m->forces = Vector2D(0, 0);
        }
    }
}
