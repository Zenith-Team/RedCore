#include <gfx/seadPrimitiveRenderer.h>

namespace sead {

void PrimitiveRenderer::drawBox(const QuadArg& arg)
{
    // TODO: The actual decompilation from sead triggers a compiler bug

    Matrix34f mtx;

    const u8 do_rotate = (u8)(arg.isHorizontal());
    const u8 no_rotate = do_rotate ^ 1;

    const Vector2f& s = arg.getSize();
    const Vector3f& t = arg.getCenter();

    const f32 sx = s.x;
    const f32 sy = s.y;

    const f32 sinV = (f32)(do_rotate);
    const f32 cosV = (f32)(no_rotate);

    mtx(0, 0) = sx *  cosV;
    mtx(1, 0) = sx *  sinV;
    mtx(2, 0) = 0.0f;

    mtx(0, 1) = sy * -sinV;
    mtx(1, 1) = sy *  cosV;
    mtx(2, 1) = 0.0f;

    mtx(0, 2) = 0.0f;
    mtx(1, 2) = 0.0f;
    mtx(2, 2) = 1.0f;

    mtx(0, 3) = t.x;
    mtx(1, 3) = t.y;
    mtx(2, 3) = t.z;

    Matrix34f outMtx;
    outMtx.setMul(mModelMtx, mtx);

    mRendererImpl->drawBoxImpl(outMtx, arg.getColor0(), arg.getColor1());
}

void PrimitiveRenderer::drawLine(const Vector3f& from, const Vector3f& to, const Color4f& c0, const Color4f& c1)
{
    Vector3f dir = to - from;

    Matrix34f mtxS;
    mtxS.makeS(dir.length(), 1.0f, 1.0f);

    dir.normalize();

    Quatf q;
    q.makeVectorRotation(Vector3f(1.0f, 0.0f, 0.0f), dir);

    Matrix34f mtxR;
    mtxR.fromQuat(q);

    Matrix34f mtx;
    mtx.setMul(mtxR, mtxS);

    dir = to - from;
    dir.multScalar(0.5f);
    dir += from;
    mtx.setTranslation(dir);

    Matrix34f outMtx;
    outMtx.setMul(mModelMtx, mtx);

    mRendererImpl->drawLineImpl(outMtx, c0, c1);
}

void PrimitiveRenderer::drawCircle16(const Vector3f& pos, f32 radius, const Color4f& color)
{
    f32 diameter = 2.0f * radius;
    Vector3f scale(diameter, diameter, diameter);

    Matrix34f mtx;
    mtx.makeST(scale, pos);

    Matrix34f outMtx;
    outMtx.setMul(mModelMtx, mtx);

    mRendererImpl->drawCircle16Impl(outMtx, color);
}

void PrimitiveRenderer::drawCircle32(const Vector3f& pos, f32 radius, const Color4f& color)
{
    f32 diameter = 2.0f * radius;
    Vector3f scale(diameter, diameter, diameter);

    Matrix34f mtx;
    mtx.makeST(scale, pos);

    Matrix34f outMtx;
    outMtx.setMul(mModelMtx, mtx);

    mRendererImpl->drawCircle32Impl(outMtx, color);
}

PrimitiveRenderer::QuadArg&
PrimitiveRenderer::QuadArg::setBoundBox(const BoundBox2f& box, f32 z)
{
    Vector2f p;
    box.getCenter(&p);

    mCenter.set(p.x, p.y, z);
    mSize.set(box.getSizeX(), box.getSizeY());
    return *this;
}

}
