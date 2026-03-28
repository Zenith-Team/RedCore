#include <gfx/seadPrimitiveRenderer.h>

// TODO: Should we just #include the decomp here? (How does this interact with the vanilla funcs from syms)

static sead::Matrix34f* ASM_MTXMultiply(sead::Matrix34f* out, sead::Matrix34f* p1, sead::Matrix34f* p2) {
    ASM_MTXConcat(p1->m, p2->m, out->m);
    return out;
}

namespace sead {

void PrimitiveRenderer::drawQuad(const Color4f& colorL, const Color4f& colorR) {
    mRendererImpl->drawQuadImpl(mModelMtx, colorL, colorR);
}

void PrimitiveRenderer::drawCube(const Color4f& c0, const Color4f& c1) {
    mRendererImpl->drawCubeImpl(mModelMtx, c0, c1);
}

PrimitiveRenderer::QuadArg& PrimitiveRenderer::QuadArg::setBoundBox(const BoundBox2f& box, f32 z) {
    Vector2f p;
    box.getCenter(&p);

    mCenter.set(p.x, p.y, z);
    mSize.set(box.getSizeX(), box.getSizeY());
    return *this;
}

void PrimitiveRenderer::drawBox(const QuadArg& arg) {
    sead::Matrix34f mtx;

    if (arg.isHorizontal()) {
        const sead::Vector2f& size = arg.getSize();
        sead::Vector3f scale(size.x, size.y, 1.0f); //? No flipping of x and y. Bug?
        sead::Vector3f rotation(0.0f, 0.0f, sead::Mathf::deg2rad(90));

        mtx.makeSRT(scale, rotation, arg.getCenter());
    } else {
        const sead::Vector2f& size = arg.getSize();
        sead::Vector3f scale(size.x, size.y, 1.0f);

        mtx.makeST(scale, arg.getCenter());
    }

    sead::Matrix34f outMtx;
    ASM_MTXMultiply(&outMtx, &mModelMtx, &mtx);

    mRendererImpl->drawBoxImpl(outMtx, arg.getColor0(), arg.getColor1());
}

void PrimitiveRenderer::drawCube(const CubeArg& arg) {
    sead::Matrix34f mtx;
    mtx.makeST(arg.getSize(), arg.getCenter());

    sead::Matrix34f outMtx;
    ASM_MTXMultiply(&outMtx, &mModelMtx, &mtx);

    mRendererImpl->drawCubeImpl(outMtx, arg.getColor0(), arg.getColor1());
}

void PrimitiveRenderer::drawWireCube(const CubeArg& arg) {
    sead::Matrix34f mtx;
    mtx.makeST(arg.getSize(), arg.getCenter());

    sead::Matrix34f outMtx;
    ASM_MTXMultiply(&outMtx, &mModelMtx, &mtx);
    mRendererImpl->drawWireCubeImpl(outMtx, arg.getColor0(), arg.getColor1());
}

void PrimitiveRenderer::drawLine(const sead::Vector3f& from, const sead::Vector3f& to, const Color4f& c0, const Color4f& c1) {
    sead::Vector3f dir = to - from;

    sead::Matrix34f mtxS;
    mtxS.makeS(dir.length(), 1.0f, 1.0f);

    dir.normalize();

    Quatf q;
    q.makeVectorRotation(sead::Vector3f(1.0f, 0.0f, 0.0f), dir);

    sead::Matrix34f mtxR;
    mtxR.fromQuat(q);

    sead::Matrix34f mtx;
    ASM_MTXMultiply(&mtx, &mtxR, &mtxS);

    dir = to - from;
    dir.multScalar(0.5f);
    dir += from;
    mtx.setTranslation(dir);

    sead::Matrix34f outMtx;
    ASM_MTXMultiply(&outMtx, &mModelMtx, &mtx);

    mRendererImpl->drawLineImpl(outMtx, c0, c1);
}

void PrimitiveRenderer::drawCircle16(const sead::Vector3f& position, f32 radius, const Color4f& color) {
    f32 diameter = radius + radius;
    sead::Vector3f scale(diameter, diameter, diameter);

    sead::Matrix34f mtx;
    mtx.makeST(scale, position);

    sead::Matrix34f outMtx;
    ASM_MTXMultiply(&outMtx, &mModelMtx, &mtx);

    mRendererImpl->drawCircle16Impl(outMtx, color);
}

void PrimitiveRenderer::drawCircle32(const sead::Vector3f& position, f32 radius, const Color4f& color) {
    f32 diameter = radius + radius;
    sead::Vector3f scale(diameter, diameter, diameter);

    sead::Matrix34f mtx;
    mtx.makeST(scale, position);

    sead::Matrix34f outMtx;
    ASM_MTXMultiply(&outMtx, &mModelMtx, &mtx);

    mRendererImpl->drawCircle32Impl(outMtx, color);
}

void PrimitiveRenderer::drawQuad(const QuadArg& arg) {
    Matrix34f mtx;
    if (arg.isHorizontal())
        mtx.makeSRT(
            Vector3f(arg.getSize().y, arg.getSize().x, 1.0f),
            Vector3f(0.0f, 0.0f, Mathf::deg2rad(90)),
            arg.getCenter()
        );
    else
        mtx.makeST(
            Vector3f(arg.getSize().x, arg.getSize().y, 1.0f),
            arg.getCenter()
        );

    Matrix34f outMtx;
    outMtx.setMul(mModelMtx, mtx);

    mRendererImpl->drawQuadImpl(outMtx, arg.getColor0(), arg.getColor1());
}

}
