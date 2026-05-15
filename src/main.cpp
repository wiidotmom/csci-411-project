#include <iostream>
#include <memory>
#include <vector>
#include <algorithm>
#include <climits>
#include <cmath>
using namespace std;

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

const int WIDTH = 640;
const int HEIGHT = 480;
const int FPS = 30;

class Vec2
{
public:
    float x, y;
    Vec2()
    {
        x = 0;
        y = 0;
    }
    Vec2(float a, float b) : x(a), y(b) {}
    Vec2(Vec2 *other)
    {
        x = other->x;
        y = other->y;
    }
    void add(float a, float b)
    {
        x += a;
        y += b;
    }

    void add(Vec2 *other)
    {
        x += other->x;
        y += other->y;
    }

    void sub(float a, float b)
    {
        x -= a;
        y -= b;
    }

    void sub(Vec2 *other)
    {
        x -= other->x;
        y -= other->y;
    }

    void mult(Vec2 *other)
    {
        x *= other->x;
        y *= other->y;
    }

    void mult(float scalar)
    {
        x *= scalar;
        y *= scalar;
    }

    void div(float scalar)
    {
        x /= scalar;
        y /= scalar;
    }

    void invertX()
    {
        x *= -1;
    }
    void invertY()
    {
        y *= -1;
    }

    float dist(Vec2 *other)
    {
        return sqrt(pow((x - other->x), 2) + pow(y - other->y, 2));
    }

    Vec2 *displacement(Vec2 *other)
    {
        return new Vec2(other->x - x, other->y - y);
    }

    float mag()
    {
        return sqrt(pow(x, 2) + pow(y, 2));
    }
};

class Boid
{
    static inline int NEXT_ID;

public:
    Vec2 *position;
    Vec2 *velocity;
    int id;
    Boid(float x, float y)
    {
        position = new Vec2(x, y);
        velocity = new Vec2(1, 1);
        id = NEXT_ID;
        NEXT_ID++;
    }

    void draw(SDL_Renderer *renderer)
    {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
        // SDL_RenderPoint(renderer, position->x, position->y);
        SDL_RenderLine(renderer, position->x, position->y, position->x + velocity->x, position->y + velocity->y);
    }

    void keepInScreen()
    {
        if (position->x < 0)
            velocity->x = 4;
        else if (position->x > WIDTH)
            velocity->x = -4;
        if (position->y < 0)
            velocity->y = 4;
        else if (position->y > HEIGHT)
            velocity->y = -4;
    }

    void flyTowardsCenter(vector<Boid *> boids)
    {
        Vec2 *c = new Vec2(0, 0);
        for (Boid *other : boids)
        {
            if (other->id != id)
            {
                c->add(other->position);
            }
        }
        c->div(boids.size() - 1);
        c->sub(position);
        c->div(100);
        velocity->add(c);
    }

    void keepDistance(vector<Boid *> boids)
    {
        Vec2 *c = new Vec2(0, 0);
        for (Boid *other : boids)
        {
            if (other->id != id)
            {
                float dist = position->dist(other->position);
                if (dist < 5)
                {
                    c->sub(position->displacement(other->position));
                }
            }
        }
        velocity->add(c);
    }

    void matchVelocity(vector<Boid *> boids)
    {
        Vec2 *v = new Vec2(0, 0);
        for (Boid *other : boids)
        {
            if (other->id != id)
            {
                v->add(other->velocity);
            }
        }
        v->div(boids.size() - 1);
        v->sub(velocity);
        v->div(8);
        velocity->add(v);
    }

    void limitVelocity()
    {
        float mag = velocity->mag();
        if (mag > 8)
        {
            velocity->div(mag);
            velocity->mult(8);
        }
    }

    void update(vector<Boid *> boids)
    {
        keepInScreen();
        flyTowardsCenter(boids);
        keepDistance(boids);
        matchVelocity(boids);
        limitVelocity();

        position->add(velocity);
    }
};

int main()
{
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("boids :)", WIDTH, HEIGHT, 0);

    if (!window)
    {
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL);

    if (!renderer)
    {
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    if (SDL_SetRenderVSync(renderer, 1) == false)
    {
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SDL_Event event;
    bool running = true;

    vector<Boid *> boids;
    boids.push_back(new Boid(WIDTH / 2, HEIGHT / 2));
    boids.push_back(new Boid((WIDTH / 2) + 10, (HEIGHT / 2) + 10));

    while (running)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
            {
                running = false;
            }
            else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
            {
                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    std::cout << "new boid "
                              << event.button.x << " " << event.button.y << std::endl;
                    boids.push_back(new Boid(event.button.x, event.button.y));
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 64, 64, 255, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);

        for (Boid *boid : boids)
        {
            boid->draw(renderer);
            boid->update(boids);
        }

        SDL_RenderPresent(renderer);

        SDL_Delay(1000 / FPS);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}