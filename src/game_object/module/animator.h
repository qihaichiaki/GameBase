#pragma once

namespace gameaf {

struct Rect;
class Camera;

class Animator
{
public:
    Animator() = default;
    ~Animator() = default;

public:
    void onUpdate(float delta);
    void onRender(const Camera&, const Rect&);

private:
};
}  // namespace gameaf