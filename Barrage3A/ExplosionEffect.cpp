#include"ExplosionEffect.h"

void ExplosionEffect::Start()
{
    // ’†S‰~
    CreateCircle(Vector2D(0, 0), 1.0f, 2.0f);

    // ã‰º¶‰E‰~
    CreateCircle(Vector2D(0, -45), 0.0f, 1.5f);  // ã
    CreateCircle(Vector2D(0, 45), 0.0f, 1.5f);   // ‰º
    CreateCircle(Vector2D(-45, 0), 0.0f, 1.5f);  // ¶
    CreateCircle(Vector2D(45, 0), 0.0f, 1.5f);   // ‰E
}

void ExplosionEffect::CreateCircle(const Vector2D& dir, float initSpeed, float accel, int l )
{
    auto sh = GameObject::Instantiate("ShapesRenderer")->AddAppBase<ShapesRenderer>();
    sh->GetGameObject()->transform->rotation = 0;
    sh->GetGameObject()->transform->position = centerPos - dir;
    sh->SetShapeType(ShapeType::Circle);

    auto filter = std::make_shared<Filter>();
    filter->SetFilter(FilterType::INVDESTCOLOR, 255);
    sh->SetFilter(filter);
    //sh->SetColor(0xFFFFFFAA);
    sh->SetSize(1);  // Å‰‚Í¬‚³‚È‰~
    sh->GetSortingLayer().layer = 300;
    sh->GetSortingLayer().orderInLayer = l;
    circles.push_back({ sh, dir, initSpeed, accel });
}

void ExplosionEffect::Update()
{
    if (Time.deltaTime == 0) return;

    time += Time.deltaTime;

    for (auto& c : circles)
    {
        // ”¼ŒaŠg‘å
        float newSize = c.renderer->GetSize() + c.speed;
        c.renderer->SetSize(newSize);
        // ‰Á‘¬.
        c.speed += c.acceleration;
    }

    // ’†S‚©‚çÄ“x”š”­‚·‚é‰~
    if (time > 0.4f && circles.size() < 6)
    {
        CreateCircle(Vector2D(0, 0), 0.0f, 2.0f);
    }

    // ˆê’èŽžŠÔŒã‚É‘Síœ
    if (time > 2.5f)
    {
        for (auto& c : circles)
        {
            c.renderer->GetGameObject()->Destroy();
        }
        this->GetGameObject()->Destroy();
    }
}