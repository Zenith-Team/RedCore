#include <dynamic_libs/gx2_functions.h>
#include <gfx/cafe/seadPrimitiveRendererCafe.h>
#include <telkin/Telkin.h>

void sead::PrimitiveRendererCafe::drawLines_(const Matrix34f& model_mtx, const Color4f& c0, const Color4f& c1, PrimitiveRendererUtil::Vertex* vtx, u32 vtx_num, u16* idx, u32 idx_num) {
    GX2SetVertexUniformReg(mParamUserOffset, 12, &model_mtx);
    GX2SetVertexUniformReg(mParamColor0Offset, 4, &c0);
    GX2SetVertexUniformReg(mParamColor1Offset, 4, &c1);
    GX2SetPixelUniformReg(mParamRateOffset, 4, &Vector4f::zero);
    GX2SetAttribBuffer(0, vtx_num * sizeof(PrimitiveRendererUtil::Vertex), sizeof(PrimitiveRendererUtil::Vertex), vtx);
    GX2DrawIndexedEx(GX2_PRIMITIVE_LINE_LOOP, idx_num, GX2_INDEX_FORMAT_U16, idx, 0, 1);
}

void sead::PrimitiveRendererCafe::drawBoxImpl(const Matrix34f& model_mtx, const Color4f& colorL, const Color4f& colorR) {
    drawLines_(model_mtx, colorL, colorR, mQuadVertexBuf, 4, mBoxIndexBuf, 4);
}
tPointer(0x10190F44, sead::PrimitiveRendererCafe::drawBoxImpl, false);

void sead::PrimitiveRendererCafe::drawLineImpl(const Matrix34f& model_mtx, const Color4f& c0, const Color4f& c1) {
    drawLines_(model_mtx, c0, c1, mLineVertexBuf, 2, mLineIndexBuf, 2);
}
tPointer(0x10190F5C, sead::PrimitiveRendererCafe::drawLineImpl, false);

void sead::PrimitiveRendererCafe::drawCircle16Impl(const Matrix34f& model_mtx, const Color4f& edge) {
    drawLines_(model_mtx, edge, edge, mDiskSVertexBuf, 17, mCircleSIndexBuf, 16);
}
tPointer(0x10190F84, sead::PrimitiveRendererCafe::drawCircle16Impl, false);

void sead::PrimitiveRendererCafe::drawCircle32Impl(const Matrix34f& model_mtx, const Color4f& edge) {
    drawLines_(model_mtx, edge, edge, mDiskLVertexBuf, 33, mCircleLIndexBuf, 32);
}
tPointer(0x10190F8C, sead::PrimitiveRendererCafe::drawCircle32Impl, false);
