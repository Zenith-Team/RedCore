#define COLLISION_DRAW_DIAGONAL 1
#define COLLISION_DRAW_BG 1

#define TELKIN_REGISTERS
#include <telkin/Telkin.h>

#include <actor/Actor.h>
#include <actor/ActorMgr.h>
#include <collision/ActorBgCollisionCheck.h>
// #include <collision/ActorBgCollisionCheckMgr.h>
#include <collision/ActorBgCollisionMgr.h>
#include <collision/ActorCircleBgCollision.h>
#include <collision/ActorCollisionCheck.h>
#include <collision/ActorCollisionCheckMgr.h>
#include <collision/ActorEllipseBgCollision.h>
#include <collision/BasicRideLineBgCollision.h>
#include <collision/LoopRideLineBgCollision.h>
#include <collision/PoleRopeBgCollision.h>
#include <collision/UnitBgCollisionHolder.h>
#include <game/AreaTask.h>
#include <gfx/seadGraphicsContext.h>
#include <gfx/seadPrimitiveRenderer.h>
#include <graphics/RenderObjLayer.h>
#include <layer/aglRenderInfo.h>
#include <map/Bg.h>
#include <scroll/BgScrollMgr.h>

static inline bool equalMagnitude(const f32 a, const f32 b) { return sead::Mathf::equalsEpsilon(sead::Mathf::abs(a), sead::Mathf::abs(b)); }

static inline f32 calcZ(const f32 z) {
    // Map z value from layer2-to-layer0 range ([-3500, 3600]) to range above layer0 ([4000, 7100])
    return (31 * z + 392500) / 71;
}

static inline void drawLine(const sead::Vector2f& point1, const sead::Vector2f& point2, const f32 z, const sead::Color4f& color, const f32 line_width) {
    // TODO: Line width
    const sead::Vector3f _p1(point1.x, point1.y, calcZ(z));
    const sead::Vector3f _p2(point2.x, point2.y, calcZ(z));
    sead::PrimitiveRenderer::instance()->drawLine(_p1, _p2, color, color);
}

static inline void drawBox(const sead::BoundBox2f& box, const f32 z, const sead::Color4f& color, const f32 line_width) {
    // TODO: Line width
    sead::PrimitiveRenderer::instance()->drawBox(sead::PrimitiveRenderer::QuadArg().setBoundBox(box, calcZ(z)).setColor(color, color));

#if COLLISION_DRAW_DIAGONAL
    drawLine(sead::Vector2f(box.getMin().x, box.getMax().y), sead::Vector2f(box.getMax().x, box.getMin().y), z, color, line_width); // Diagonal line
#endif // COLLISION_DRAW_DIAGONAL
}

static inline void drawCircle(const sead::Vector2f& center, const f32 z, const f32 radius, const sead::Color4f& color, const f32 line_width) {
    // TODO: Line width
    sead::PrimitiveRenderer::instance()->drawCircle32(sead::Vector3f(center.x, center.y, calcZ(z)), radius, color);
}

struct SinCos {
    f32 sin_v;
    f32 cos_v;
};

template <s32 cVertNum> struct CircleSinCosTable {
    static_assert(cVertNum >= 3);

    CircleSinCosTable()
        : step(sead::Mathf::cHalfRoundIdx / f32(cVertNum) * 2) {
        for (s32 i = 0; i < cVertNum - 1; i++)
            sead::Mathf::sinCosIdx(&data[i].sin_v, &data[i].cos_v, step * (i + 1));

        sead::Mathf::sinCosIdx(&data[cVertNum - 1].sin_v, &data[cVertNum - 1].cos_v, 0);
    }

    SinCos data[cVertNum];
    const f32 step;
};

static const s32 cVertNum = 32;
static const CircleSinCosTable<cVertNum> cCircleSinCosTable;

static inline void _drawEllipse(const sead::Vector2f& center, const f32 z, const f32 r_x, const f32 r_y, const sead::Color4f& color, const f32 line_width) {
    sead::Vector2f p0(r_x, 0.0f);
    sead::Vector2f p1;

    for (s32 i = 0; i < cVertNum; i++) {
        const SinCos& dat = cCircleSinCosTable.data[i];

        p1.x = r_x * dat.cos_v;
        p1.y = r_y * dat.sin_v;

        drawLine(center + p0, center + p1, z, color, line_width);

        p0 = p1;
    }
}

static inline void drawEllipse(const sead::Vector2f& center, const f32 z, const f32 r_x, const f32 r_y, const sead::Color4f& color, const f32 line_width) {
    if (equalMagnitude(r_x, r_y))
        drawCircle(center, z, r_x, color, line_width);

    else
        _drawEllipse(center, z, r_x, r_y, color, line_width);
}

static inline void _drawEllipse(const sead::Vector2f& center, const f32 z, const f32 r_x, const f32 r_y, const u32 angle, const sead::Color4f& color, const f32 line_width) {
    switch (angle) {
    case 0:
        return _drawEllipse(center, z, r_x, r_y, color, line_width);
    case sead::Mathf::cQuarterRoundIdx:
        return _drawEllipse(center, z, -r_y, r_x, color, line_width);
    case sead::Mathf::cHalfRoundIdx:
        return _drawEllipse(center, z, -r_x, -r_y, color, line_width);
    case (sead::Mathf::cHalfRoundIdx + sead::Mathf::cQuarterRoundIdx):
        return _drawEllipse(center, z, r_y, -r_x, color, line_width);
    }

    sead::Matrix22f mtx;
    mtx.makeRIdx(angle);

    sead::Vector2f p0(mtx(0, 0) * r_x /* + mtx(0, 1) * 0.0f */, mtx(1, 0) * r_x /* + mtx(1, 1) * 0.0f */);
    sead::Vector2f p1;

    for (s32 i = 0; i < cVertNum; i++) {
        const SinCos& dat = cCircleSinCosTable.data[i];

        const sead::Vector2f p(r_x * dat.cos_v, r_y * dat.sin_v);

        p1.x = mtx(0, 0) * p.x + mtx(0, 1) * p.y;
        p1.y = mtx(1, 0) * p.x + mtx(1, 1) * p.y;

        drawLine(center + p0, center + p1, z, color, line_width);

        p0 = p1;
    }
}

static inline void drawEllipse(const sead::Vector2f& center, const f32 z, const f32 r_x, const f32 r_y, const u32 angle, const sead::Color4f& color, const f32 line_width) {
    if (equalMagnitude(r_x, r_y))
        drawCircle(center, z, r_x, color, line_width);

    else
        _drawEllipse(center, z, r_x, r_y, angle, color, line_width);
}

static inline void drawArc(const sead::Vector2f& center, const f32 z, const f32 radius, const u32 arc_start, const u32 arc_end, const sead::Color4f& color, const f32 line_width) {
    if (arc_start == arc_end)
        return drawCircle(center, z, radius, color, line_width);

    f32 sin_v[2];
    f32 cos_v[2];

    sead::Mathf::sinCosIdx(&sin_v[0], &cos_v[0], arc_start);
    sead::Mathf::sinCosIdx(&sin_v[1], &cos_v[1], arc_end);

    const sead::Vector2f start_p(radius * cos_v[0], radius * sin_v[0]);
    const sead::Vector2f end_p(radius * cos_v[1], radius * sin_v[1]);

    const f32 step = cCircleSinCosTable.step;

    s32 start_index = arc_start / step;
    s32 end_index = arc_end / step;

    if (start_index == end_index && arc_end - arc_start <= step) /* Arc is between 2 steps */ {
        drawLine(center + start_p, center + end_p, z, color, line_width);
    } else {
        end_index -= 1;
        if (end_index < 0)
            end_index += cVertNum;

        sead::Vector2f p0 = start_p;
        sead::Vector2f p1;

        { // Start line
            const SinCos& dat = cCircleSinCosTable.data[start_index];

            p1.x = radius * dat.cos_v;
            p1.y = radius * dat.sin_v;

            drawLine(center + p0, center + p1, z, color, line_width);

            p0 = p1;
        }

        for (s32 i = start_index; i != end_index;) /* Mid lines */ {
            i = u32(i + 1) % cVertNum;

            const SinCos& dat = cCircleSinCosTable.data[i];

            p1.x = radius * dat.cos_v;
            p1.y = radius * dat.sin_v;

            drawLine(center + p0, center + p1, z, color, line_width);

            p0 = p1;
        }

        { // End line
            p1 = end_p;

            drawLine(center + p0, center + p1, z, color, line_width);
        }
    }
}

static inline void drawCapsule(const sead::Vector2f& point1, const sead::Vector2f& point2, const f32 z, const f32 radius, const sead::Color4f& color, const f32 line_width) {
    const u32 angle = sead::Mathf::atan2Idx(point1.y - point2.y, point1.x - point2.x) - sead::Mathf::cQuarterRoundIdx;

    drawArc(point1, z, radius, angle, angle + sead::Mathf::cHalfRoundIdx, color, line_width);
    drawArc(point2, z, radius, angle + sead::Mathf::cHalfRoundIdx, angle, color, line_width);

    f32 sin_v, cos_v;
    sead::Mathf::sinCosIdx(&sin_v, &cos_v, angle);

    const sead::Vector2f delta(radius * cos_v, radius * sin_v);

    drawLine(point1 - delta, point2 - delta, z, color, line_width);
    drawLine(point1 + delta, point2 + delta, z, color, line_width);
}

static inline void drawActorCollisionCheck_Box(const ActorCollisionCheck& cc) {
    const sead::BoundBox2f& box = cc.getBoundBox();
    const f32 z = cc.getOwner()->getPos().z;

    drawBox(box, z, sead::Color4f::cRed, 1.0f);
}

static inline void drawActorCollisionCheck_Circle(const ActorCollisionCheck& cc) {
    const sead::Vector2f center(cc.getCenterPosX(), cc.getCenterPosY());
    const f32 z = cc.getOwner()->getPos().z;
    const f32 r_x = cc.getHalfSizeX();
    const f32 r_y = cc.getHalfSizeY();

    drawEllipse(center, z, r_x, r_y, sead::Color4f::cRed, 1.0f);
}

static inline void drawActorCollisionCheck_DaikeiUD(const ActorCollisionCheck& cc) {
    const f32 left = cc.getLeftPos();
    const f32 right = cc.getRightPos();

    const f32 center_y = cc.getCenterPosY();

    const sead::Vector2f tl(left, center_y + cc.getDaikei(0));
    const sead::Vector2f tr(right, center_y + cc.getDaikei(2));
    const sead::Vector2f br(right, center_y + cc.getDaikei(3));
    const sead::Vector2f bl(left, center_y + cc.getDaikei(1));

    const f32 z = cc.getOwner()->getPos().z;

    drawLine(tl, tr, z, sead::Color4f::cRed, 1.0f);
    drawLine(tr, br, z, sead::Color4f::cRed, 1.0f);
    drawLine(br, bl, z, sead::Color4f::cRed, 1.0f);
    drawLine(bl, tl, z, sead::Color4f::cRed, 1.0f);
#if COLLISION_DRAW_DIAGONAL
    drawLine(tl, br, z, sead::Color4f::cRed, 1.0f); // Diagonal line
#endif // COLLISION_DRAW_DIAGONAL
}

static inline void drawActorCollisionCheck_DaikeiLR(const ActorCollisionCheck& cc) {
    const f32 center_x = cc.getCenterPosX();
    const f32 top = cc.getTopPos();

    const f32 bottom = cc.getUnderPos();

    const sead::Vector2f tl(center_x + cc.getDaikei(0), top);
    const sead::Vector2f bl(center_x + cc.getDaikei(2), bottom);
    const sead::Vector2f br(center_x + cc.getDaikei(3), bottom);
    const sead::Vector2f tr(center_x + cc.getDaikei(1), top);

    const f32 z = cc.getOwner()->getPos().z;

    drawLine(tl, tr, z, sead::Color4f::cRed, 1.0f);
    drawLine(tr, br, z, sead::Color4f::cRed, 1.0f);
    drawLine(br, bl, z, sead::Color4f::cRed, 1.0f);
    drawLine(bl, tl, z, sead::Color4f::cRed, 1.0f);
#if COLLISION_DRAW_DIAGONAL
    drawLine(tl, br, z, sead::Color4f::cRed, 1.0f); // Diagonal line
#endif // COLLISION_DRAW_DIAGONAL
}

static inline void drawActorCollisionCheck(const ActorCollisionCheck& cc) {
    switch (cc.getShapeType()) {
    case ActorCollisionCheck::cShapeType_Box:
        drawActorCollisionCheck_Box(cc);
        break;
    case ActorCollisionCheck::cShapeType_Circle:
        drawActorCollisionCheck_Circle(cc);
        break;
    case ActorCollisionCheck::cShapeType_DaikeiUD:
        drawActorCollisionCheck_DaikeiUD(cc);
        break;
    case ActorCollisionCheck::cShapeType_DaikeiLR:
        drawActorCollisionCheck_DaikeiLR(cc);
        break;
    }
}

static inline const sead::Color4f& getBgCollisionColor(const u64& bc_data) {
    switch (BgUnitCode::getHitType(bc_data)) {
    case BgUnitCode::cHitType_None:
    default:
        return sead::Color4f::cBlack;
    case BgUnitCode::cHitType_Full:
        return sead::Color4f::cBlue;
    case BgUnitCode::cHitType_Half:
        return sead::Color4f::cMagenta;
    case BgUnitCode::cHitType_HalfCeiling:
        return sead::Color4f::cGreen;
    case BgUnitCode::cHitType_HalfBoth:
        return sead::Color4f::cGray;
    }
}

static inline void drawBgCollision_Circle(const ActorCircleBgCollision& bg_collision, const sead::Color4f& color) {
    const sead::Vector2f center(bg_collision.getCenterPosX(), bg_collision.getCenterPosY());
    const f32 z = bg_collision.getFollowArg().position->z;
    const f32 radius = bg_collision.getRadius();
    const u32 arc_start = bg_collision.getArcStart();
    const u32 arc_end = bg_collision.getArcEnd();

    drawArc(center, z, radius, arc_start, arc_end, color, 1.0f);
}

static inline void drawBgCollision_Ellipse(const ActorEllipseBgCollision& bg_collision, const sead::Color4f& color) {
    const sead::Vector2f center(bg_collision.getCenterPosX(), bg_collision.getCenterPosY());
    const f32 z = bg_collision.getFollowArg().position->z;
    const sead::Vector2f& radii = bg_collision.getHalfSize();
    const u32 angle = bg_collision.getAngle();

    drawEllipse(center, z, radii.x, radii.y, angle, color, 1.0f);
}

template <typename T> static inline bool drawBgCollision_RideLine(const T& bg_collision, const sead::Color4f& color) {
    const sead::Buffer<BasicRideLine>& ride_line = bg_collision.getRideLine();
    if (ride_line.size() < 1)
        return false;

    const sead::Vector2f pos(bg_collision.getPosX(), bg_collision.getPosY());
    const f32 z = bg_collision.getFollowArg().position->z;

    for (sead::Buffer<BasicRideLine>::constIterator itr_ride_line = ride_line.begin(), itr_ride_line_end = ride_line.end(); itr_ride_line != itr_ride_line_end; ++itr_ride_line) {
        const sead::Segment2f& segment = itr_ride_line->getSegment().getSegment();
        drawLine(pos + segment.getPos0(), pos + segment.getPos1(), z, color, 1.0f);
    }

    return true;
}

static inline void drawBgCollision_Polyline(const BasicRideLineBgCollision& bg_collision, const sead::Color4f& color) { drawBgCollision_RideLine(bg_collision, color); }

static inline void drawBgCollision_Polygon(const LoopRideLineBgCollision& bg_collision, const sead::Color4f& color) {
    if (!drawBgCollision_RideLine(bg_collision, color))
        return;

#if COLLISION_DRAW_DIAGONAL
    // If it's a rectangle, draw a diagonal line
    const sead::Buffer<BasicRideLine>& ride_line = bg_collision.getRideLine();
    if (ride_line.size() < 4 || (ride_line.size() & 1))
        return;

    const sead::Vector2f pos(bg_collision.getPosX(), bg_collision.getPosY());
    const f32 z = bg_collision.getFollowArg().position->z;

    const sead::Segment2f& segment1 = ride_line[0].getSegment().getSegment();
    const sead::Segment2f& segment2 = ride_line[ride_line.size() / 2 - 1].getSegment().getSegment();
    drawLine(pos + segment1.getPos0(), pos + segment2.getPos1(), z, color, 1.0f);
#endif // COLLISION_DRAW_DIAGONAL
}

static inline void drawBgCollision_PoleRope(const PoleRopeBgCollision& bg_collision, const sead::Color4f& color) {
    const sead::Buffer<sead::Vector2f>& points = bg_collision.getPoints();

    const s32 point_num = points.size();
    if (point_num < 2)
        return;

    const sead::Vector2f* point_buf = points.getBufferPtr();

    const sead::Vector2f pos(bg_collision.getPosX(), bg_collision.getPosY());
    const f32 z = bg_collision.getFollowArg().position->z;
    const f32 radius = bg_collision.getRange();

    if (sead::Mathf::abs(radius) <= sead::Mathf::epsilon()) {
        for (s32 i = 0; i + 1 < point_num; i++)
            drawLine(pos + point_buf[i], pos + point_buf[i + 1], z, color, 1.0f);
    } else {
        for (s32 i = 0; i + 1 < point_num; i++)
            drawCapsule(pos + point_buf[i], pos + point_buf[i + 1], z, radius, color, 1.0f);
    }
}

static inline void drawBgCollision(const BgCollision& bg_collision, const agl::lyr::RenderInfo& render_info) {
    const u64& bc_data = bg_collision.getBgCheckData();
    if (BgUnitCode::getType(bc_data) == BgUnitCode::cType_None && BgUnitCode::getHitType(bc_data) == BgUnitCode::cHitType_None)
        return;

    const BgScrollMgr& bg_scroll_mgr = *BgScrollMgr::instance();

    const f32 screen_left = bg_scroll_mgr.getScreenLeft();
    const f32 screen_bottom = bg_scroll_mgr.getScreenBottom();
    const f32 screen_w = bg_scroll_mgr.getScreenWidth();
    const f32 screen_h = bg_scroll_mgr.getScreenHeight();

    const sead::FrameBuffer& frame_buffer = *render_info.getFrameBuffer();

    const sead::Vector2f& fb_size = frame_buffer.getVirtualSize();
    const f32 fb_w = frame_buffer.getVirtualSize().x;
    const f32 fb_h = frame_buffer.getVirtualSize().y;

    const f32 w_ratio = fb_w / screen_w;
    const f32 h_ratio = fb_h / screen_h;

    const sead::BoundBox2f& affected_area = bg_collision.getAffectedArea();

    // drawBox(affected_area, 3600, sead::Color4f::cRed, 1.0f);

    const f32 x = ((affected_area.getMin().x - 0.5f) - screen_left) * w_ratio;
    const f32 y = ((affected_area.getMin().y - 0.5f) - screen_bottom) * h_ratio;
    const f32 w = (affected_area.getSizeX() + 1.0f) * w_ratio;
    const f32 h = (affected_area.getSizeY() + 1.0f) * h_ratio;

    { // Apply scissor
        sead::Viewport viewport = sead::Viewport(x, y, w, h);

        sead::Vector2f real_pos;
        viewport.getOnFrameBufferPos(&real_pos, frame_buffer);

        sead::Vector2f real_size;
        viewport.getOnFrameBufferSize(&real_size, frame_buffer);

        const f32 real_pos_max_x = std::ceil(real_pos.x + real_size.x);
        const f32 real_pos_max_y = std::ceil(real_pos.y + real_size.y);

        real_pos.x = sead::Mathf::max(std::floor(real_pos.x), frame_buffer.getPhysicalArea().getMin().x);
        real_pos.y = sead::Mathf::max(std::floor(real_pos.y), frame_buffer.getPhysicalArea().getMin().y);

        real_size.x = sead::Mathf::min(real_pos_max_x, frame_buffer.getPhysicalArea().getMax().x) - real_pos.x;
        real_size.y = sead::Mathf::min(real_pos_max_y, frame_buffer.getPhysicalArea().getMax().y) - real_pos.y;

        // SEAD_ASSERT(frame_buffer.getPhysicalArea().isInside(real_pos) && frame_buffer.getPhysicalArea().isInside(real_pos + real_size));

        real_pos.y = (frame_buffer.getPhysicalArea().getSizeY() - real_size.y) - real_pos.y;

        sead::Graphics::instance()->setScissorRealPosition(real_pos.x, real_pos.y, real_size.x, real_size.y);
    }

    const sead::Color4f& color = getBgCollisionColor(bg_collision.getBgCheckData());

    if (sead::IsDerivedTypes<ActorCircleBgCollision>(&bg_collision))
        drawBgCollision_Circle(static_cast<const ActorCircleBgCollision&>(bg_collision), color);

    else if (sead::IsDerivedTypes<ActorEllipseBgCollision>(&bg_collision))
        drawBgCollision_Ellipse(static_cast<const ActorEllipseBgCollision&>(bg_collision), color);

    else if (sead::IsDerivedTypes<BasicRideLineBgCollision>(&bg_collision))
        drawBgCollision_Polyline(static_cast<const BasicRideLineBgCollision&>(bg_collision), color);

    else if (sead::IsDerivedTypes<LoopRideLineBgCollision>(&bg_collision))
        drawBgCollision_Polygon(static_cast<const LoopRideLineBgCollision&>(bg_collision), color);

    else if (sead::IsDerivedTypes<PoleRopeBgCollision>(&bg_collision))
        drawBgCollision_PoleRope(static_cast<const PoleRopeBgCollision&>(bg_collision), color);

    { // Restore scissor
        const sead::Viewport& viewport = *render_info.getViewport();

        sead::Vector2f real_pos;
        viewport.getOnFrameBufferPos(&real_pos, frame_buffer);

        sead::Vector2f real_size;
        viewport.getOnFrameBufferSize(&real_size, frame_buffer);

        // SEAD_ASSERT(frame_buffer.getPhysicalArea().isInside(real_pos) && frame_buffer.getPhysicalArea().isInside(real_pos + real_size));

        real_pos.y = (frame_buffer.getPhysicalArea().getSizeY() - real_size.y) - real_pos.y;

        sead::Graphics::instance()->setScissorRealPosition(real_pos.x, real_pos.y, real_size.x, real_size.y);
    }
}

#if COLLISION_DRAW_BG

static inline void drawBgUnitCollision(const agl::lyr::RenderInfo& render_info) {
    BgScrollMgr& bg_scroll_mgr = *BgScrollMgr::instance();
    UnitBgCollisionHolder& bg_collision_holder = *UnitBgCollisionHolder::instance();

    const s32 unit_size = 16;
    const s32 delta = unit_size - 1;
    const u32 mask = ~delta;

    const s32 left = s32(std::floor(bg_scroll_mgr.getScreenLeft())) & mask;
    const s32 right = (s32(std::ceil(bg_scroll_mgr.getScreenRight())) + delta) & mask;
    const s32 bottom = (s32(std::ceil(-bg_scroll_mgr.getScreenBottom())) + delta) & mask;
    const s32 top = s32(std::floor(-bg_scroll_mgr.getScreenTop())) & mask;

    for (s32 y = top; y < bottom; y += unit_size) {
        for (s32 x = left; x < right; x += unit_size) {
            bg_collision_holder.setFromBgUnit(sead::Vector2f(static_cast<f32>(x), -static_cast<f32>(y)), // position
                0, // layer 1
                sead::BitFlag8(u8(-1)) // collision mask
            );

            BgCollision* p_bg_collision = bg_collision_holder.getBgCollision();
            if (p_bg_collision == nullptr)
                continue;

            drawBgCollision(*p_bg_collision, render_info);
        }
    }
}

#endif // COLLISION_DRAW_BG

static inline void drawActorBgCollisionCheck(const ActorBgCollisionCheck& bc) {
    const sead::Vector3f* p_position = bc.getFollowArg().position;
    if (p_position == nullptr)
        return;

    const f32 x = p_position->x;
    const f32 y = p_position->y;
    const f32 z = p_position->z;

    for (s32 j = 0; j < 4; j++) {
        const ActorBgCollisionCheck::Sensor* p_sensor = bc.getSensor(j);
        if (p_sensor == nullptr)
            continue;

        f32 p1 = p_sensor->p1;
        f32 p2 = p_sensor->p2;
        const f32 sensor_center_offset = p_sensor->center_offset;

        if (sead::Mathf::abs(p1 - p2) < 1.0f) {
            f32 p = (p1 + p2) * 0.5f;
            p1 = p - 0.5f;
            p2 = p + 0.5f;
        }

        if (j < 2) {
            const sead::Vector2f point1(x + sensor_center_offset, y + p1);
            const sead::Vector2f point2(x + sensor_center_offset, y + p2);

            drawLine(point1, point2, z, sead::Color4f::cYellow, 1.0f);
        } else {
            const sead::Vector2f point1(x + p1, y + sensor_center_offset);
            const sead::Vector2f point2(x + p2, y + sensor_center_offset);

            drawLine(point1, point2, z, (j == 2) ? sead::Color4f::cWhite : sead::Color4f::cCyan, 1.0f);
        }
    }
}

namespace red {

void renderCollisions(const agl::lyr::RenderInfo& render_info) {
    if (render_info.getRenderStep() != RenderObjLayer::cRenderStep_PostFx)
        return;

    sead::GraphicsContext context;
    // context.setDepthEnable(true, true);                         // Automatically set by ctor
    // context.setDepthFunc(sead::Graphics::cDepthFunc_LessEqual); // ^^
    context.setCullingMode(sead::Graphics::cCullingMode_None);
    context.setBlendEnable(false);
    context.apply();

    sead::PrimitiveRenderer::instance()->setCamera(*render_info.getCamera());
    sead::PrimitiveRenderer::instance()->setProjection(*render_info.getProjection());
    sead::PrimitiveRenderer::instance()->begin();

    for (LineNodeMgr<ActorCollisionCheck>::Node* node = ActorCollisionCheckMgr::instance()->getActiveList().front(); node != nullptr; node = node->next) {
        ActorCollisionCheck* p_cc = node->obj;
        if (p_cc == nullptr)
            continue;

        Actor* p_actor = p_cc->getOwner();
        if (p_actor == nullptr || !p_actor->isActive() || p_actor->isRequestedDelete())
            continue;

        if (p_cc->isDisableCallback() || !p_cc->isCollidable())
            continue;

        drawActorCollisionCheck(*p_cc);
    }

    for (LineNodeMgr<ActorCollisionCheck>::Node* node = ActorCollisionCheckMgr::instance()->getTouchDrcCheckList().front(); node != nullptr; node = node->next) {
        ActorCollisionCheck* p_cc = node->obj;
        if (p_cc == nullptr)
            continue;

        Actor* p_actor = p_cc->getOwner();
        if (p_actor == nullptr || !p_actor->isActive() || p_actor->isRequestedDelete())
            continue;

        if (p_cc->getKind() != ActorCollisionCheck::cKind_Unk12)
            if (!p_cc->isDisableCallback() && p_cc->isCollidable())
                continue; // Already rendered this in the previous loop

        if (p_cc->getDrcTouchCallback() == nullptr)
            continue;

        drawActorCollisionCheck(*p_cc);
    }

    for (LineNodeMgr<BgCollision>::Node* node = ActorBgCollisionMgr::instance()->getActiveList().front(); node != nullptr; node = node->next) {
        BgCollision* p_bg_collision = node->obj;
        if (p_bg_collision == nullptr)
            continue;

        drawBgCollision(*p_bg_collision, render_info);
    }

    for (LineNodeMgr<BgCollision>::Node* node = ActorBgCollisionMgr::instance()->getPoleList().front(); node != nullptr; node = node->next) {
        BgCollision* p_bg_collision = node->obj;
        if (p_bg_collision == nullptr)
            continue;

        drawBgCollision(*p_bg_collision, render_info);
    }

#if COLLISION_DRAW_BG
    drawBgUnitCollision(render_info);
#endif // COLLISION_DRAW_BG

    // TODO: impl ActorBgCollisionCheckMgr+hooks
    // for (LineNodeMgr<ActorBgCollisionCheck>::Node* node = ActorBgCollisionCheckMgr::instance()->getDrawList().front(); node != nullptr;)
    // {
    //     LineNodeMgr<ActorBgCollisionCheck>::Node* temp_node = node;
    //     node = node->next;
    //     ActorBgCollisionCheck* bc = temp_node->obj;
    //   //SEAD_ASSERT(p_bc != nullptr);
    //     ActorBgCollisionCheckMgr::instance()->release(*bc);

    //     drawActorBgCollisionCheck(*bc);
    // }

    sead::PrimitiveRenderer::instance()->end();
}

// TODO: Make this an event+callback
void debugDraw(AreaTask* at, const agl::lyr::RenderInfo& render_info) {
    at->drawLayer3D(render_info);
    renderCollisions(render_info);
}

void areaTaskSetDrawDebug() tAssembly(
    lis r9, _ZN3red9debugDrawEP8AreaTaskRKN3agl3lyr10RenderInfoE@ha;
    addi r9, r9, _ZN3red9debugDrawEP8AreaTaskRKN3agl3lyr10RenderInfoE@l;
    blr;
)

}

tBranch(0x024B65E0, red::areaTaskSetDrawDebug, tk::BranchType::bl);
