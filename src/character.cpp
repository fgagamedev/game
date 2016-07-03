#include "character.h"
#include "ije02_game.h"
#include "main_level.h"

#include <ijengine/engine.h>
#include <ijengine/canvas.h>
#include <ijengine/rectangle.h>

#include <iostream>
#include <algorithm>
#include <cstdio>

using std::cout;
using std::endl;
using std::min;
using std::max;

const double SPEED = 80.00;

Character::Character(const vector<string> sprite_paths, unsigned id, double x, double y)
    : m_moving_state(MOVING_RIGHT), m_frame(0), m_start(-1), m_x_speed(0.00), m_y_speed(0.00)
{
    for(int i = 0; i < NUMBER_OF_STATES; i++) {
        m_textures.push_back(resources::get_texture(sprite_paths[i]));
    }

    change_character_state(IDLE_STATE);

    m_id = id;
    m_x = x;
    m_y = y;

    m_w = 32;
    m_h = 32;

    m_bounding_box = Rectangle(m_x, m_y, 24, 24);

    m_speed_vector["down"] = make_pair(0.00, SPEED);
    m_speed_vector["left"] = make_pair(-SPEED, 0.00);
    m_speed_vector["right"] = make_pair(SPEED, 0.00);
    m_speed_vector["up"] = make_pair(0.00, -SPEED);
    
    event::register_listener(this);
    physics::register_object(this);
}

Character::~Character()
{
    physics::unregister_object(this);
    event::unregister_listener(this);
}

void
Character::update_self(unsigned now, unsigned last)
{
    if (m_start == -1)
        m_start = now;

    if (now - m_start > m_state->m_refresh_rate)
    {
        m_start += m_state->m_refresh_rate;
        m_frame = (m_frame + 1) % (m_textures[m_state->m_current_sprite]->w() / 32);
    }

    if(m_y_speed == 0.0 && m_x_speed == 0.0) {
        return;
    }

    update_position(now, last);

    m_bounding_box.set_position(x(), y());


}

inline void
Character::update_position(const unsigned &now, const unsigned &last, bool backwards) {
    int multiplier = (backwards) ? -1 : 1;

    double new_y = y() + multiplier * m_y_speed * (now - last) / 1000.0;
    new_y = min(new_y, SCREEN_HEIGHT - 32.00);
    new_y = max(new_y, 0.0);

    double new_x = x() + multiplier * m_x_speed * (now - last) / 1000.0;
    new_x = min(new_x, SCREEN_WIDTH - 32.00);
    new_x = max(new_x, 0.0);

    set_y(new_y);
    set_x(new_x);
}

void
Character::draw_self(Canvas *canvas, unsigned, unsigned)
{
    Rectangle rect {(double) m_w * m_frame, (double) m_h * m_moving_state, (double) m_w, (double) m_h};
    canvas->draw(m_textures[m_state->m_current_sprite].get(), rect, x(), y());
}

bool
Character::on_event(const GameEvent& event)
{
    if((event.id() == game_event::MOVEMENT_P1 && m_id == 0) ||
       (event.id() == game_event::MOVEMENT_P2 && m_id == 1)) {
        string axis = event.get_property<string>("axis");
        int value = event.get_property<int>("value");

        if(axis == "X") {
            m_x_speed = SPEED * ((double) value / 32768);
            if(value > 0) {
                m_state = MOVING_RIGHT;
            }
            else if(value < 0) {
                m_state = MOVING_LEFT;
            }
        } 
        else if(axis == "Y") {
            m_y_speed = SPEED * ((double) value / 32768);
        }

        if(m_x_speed == 0.0 && m_y_speed == 0.0) {
            change_character_state(IDLE_STATE);
            m_frame = 0;
        }

        return true;
    }

    return false;
}

pair<double, double>
Character::direction() const
{
    return pair<double, double>(m_x_speed, m_y_speed);
}

bool
Character::active() const 
{
    return true;
}

const Rectangle&
Character::bounding_box() const 
{
    return m_bounding_box;
}

const list<Rectangle>&
Character::hit_boxes() const 
{
    static list<Rectangle> boxes {m_bounding_box};
    return boxes;
}

void
Character::on_collision(const Collidable *who, const Rectangle& where, unsigned now, unsigned last) 
{
   update_position(now, last, true);

}

void
Character::change_character_state(State next_state) 
{
    m_state = m_character_state_factory.change_character_state(next_state);
}
