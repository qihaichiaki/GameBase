#include "ProgressBar.h"

#include <GameAf.h>
#include <common/Macros.h>
#include <scene/Camera.h>

namespace gameaf {

static inline bool CheckProgress(float delta, float progressValue, float targetProgressValue)
{
    return (delta < 0 && progressValue + delta >= targetProgressValue ||
            delta > 0 && progressValue + delta <= targetProgressValue);
}

void ProgressBar::OnUpdate()
{
    float delta = GameAf::GetInstance().GetFixDeltaTime() * m_progressSpeed *
                  (m_targetProgressValue > m_progressValue ? 1 : -1);

    if (CheckProgress(delta, m_progressValue, m_targetProgressValue)) {
        m_progressValue += delta;
    } else {
        m_progressValue = m_targetProgressValue;
    }

    if (m_isGhostBar) {
        if (CheckProgress(delta, m_ghostProgressValue, m_targetProgressValue)) {
            m_ghostProgressValue += delta * m_ghostSpeedScale;
        } else {
            m_ghostProgressValue = m_targetProgressValue;
        }
    }
}

void ProgressBar::OnDraw(const Camera& camera)
{
    Vector2 pos = GetPosition();
    int leftBack = pos.X - m_borderSize.X / 2 - camera.GetPosition().X;
    int leftFront = pos.X - m_progressBarSize.X / 2 - camera.GetPosition().X;
    int topBack = pos.Y - m_borderSize.Y / 2 - camera.GetPosition().Y;
    int topFront = pos.Y - m_progressBarSize.Y / 2 - camera.GetPosition().Y;

    int rightBack = leftBack + m_borderSize.X;
    int rightFront = leftFront + (m_progressBarSize.X * m_progressValue);
    int rightFrontGhost = leftFront + (m_progressBarSize.X * m_ghostProgressValue);
    int rightFrontBack = leftFront + m_progressBarSize.X;
    int bottomBack = topBack + m_borderSize.Y;
    int bottomFront = topFront + m_progressBarSize.Y;
#ifdef GAMEAF_USE_EASYX
    // 画矩形

    // 边框
    setfillcolor(m_borderColor);
    solidroundrect(leftBack, topBack, rightBack, bottomBack, m_ellipsewidth, m_ellipseheight);
    // 背景
    setfillcolor(m_backgroundColor);
    solidroundrect(leftFront, topFront, rightFrontBack, bottomFront, m_ellipsewidth,
                   m_ellipseheight);
    // 滞留进度条
    if (m_isGhostBar && std::abs(m_ghostProgressValue) > 1e-5) {
        setfillcolor(m_ghostBarColor);
        solidroundrect(leftFront, topFront, rightFrontGhost, bottomFront, m_ellipsewidth,
                       m_ellipseheight);
    }
    // 实际进度条
    if (std::abs(m_progressValue) > 1e-5) {
        setfillcolor(m_progressBarColor);
        solidroundrect(leftFront, topFront, rightFront, bottomFront, m_ellipsewidth,
                       m_ellipseheight);
    }
#else
#endif
}

}  // namespace gameaf