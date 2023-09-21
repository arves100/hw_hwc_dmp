/*!
    @file hwc2layer.cpp
    @brief Hardware compositor layer
    @author Arves100
    @date 19/04/2022
*/
#include "StdAfx.h"
#include "hwc2layer.h"

Hwc2Layer::Hwc2Layer(uint32_t display)
{
    this->display = display;
    layerTransform = 0;
    layerBlendMode = 256;
    acquireFence = -1;
    releaseFd = -1;
    layerBuffer = nullptr;
    layerSidebandStream = nullptr;
    memset(&layerDisplayFrame, 0, sizeof(layerDisplayFrame));
    compositionType = 0;
    layerCompositionType = HWC::CompositionType::Invalid;
    flags = LAYER_FLAGS_NONE;
    layerBufferChanged = false;
    changedColor = 0;
    layerPlaneAlpha = 255;
    zorder = 0;
    layerDataspace = HAL_DATASPACE_UNKNOWN;
    memset(&color, 0, sizeof(color));
    layer = 0;
}

uint32_t Hwc2Layer::GetUint32Color(hwc_color const& color)
{
    return (color->a << 24) | (color->r << 16) | (color->g << 8) | color->b;
}

uint32_t Hwc2Layer::GetFormat()
{
    if (!layerBuffer)
        return 0xFFFFFFFF; // -1

    return layerBuffer->data[3]; // WTF?
}

void Hwc2Layer::SetLayerBlendMode(uint32_t mode)
{
    if (mode - 1 <= 2) // why -1? // why 2
    {
        auto blendMode = g_dwLayerBlendModes[mode - 1]; // is index 1 ?
        if (blendMode != layerBlendMode)
        {
            layerBlendMode = blendMode;
            flags |= LAYER_BLEND_MODE;
        }
    }
}

void Hwc2Layer::SetLayerBuffer(native_handle* buffer, int acquireFence)
{
    if (!buffer && HwcConfig::GetDebugLevel() >= 1) // TODO: macro this 1
    {
        HWC_WARN("[Disp%d]: layer[%d]: Invalid buffer handle: %p.", display, layer, 0);
        return;
    }

    if (!acquireFence && HwcConfig::GetDebugLevel() >= 1)
    {
        HWC_WARN("[Disp%d]: layer[%d]: acquireFence is zero", display, layer);
        return;
    }

    if (this->acquireFence)
    {
        if (HwcConfig::GetDebugLevel() >= 2)
            HWC_INFO("[Disp%d]: layer[%d]: close acquireFence fd=%d", display, layer, this->acquireFence);

        close(this->acquireFence);
    }

    if (layerBuffer != buffer)
    {
        layerBufferChanged = true;
        layerBuffer = buffer;
    }
}

void Hwc2Layer::SetLayerColor(hwc_color color)
{
    this->color = color.color;
    if (color.a == 255 && layerCompositionType == HWC2::Composition::SolidColor)
    {
        changedColor != 0x100;
        if (HwcConfig::GetDebugLevel() >= 2)
            HWC_INFO("[Disp%d]: layer[%d]: is DIM layer!\n", display, layer);
    }
}

int Hwc2Layer::SetCompositionType(HWC2::Composition ct)
{
    if (ct == HWC::Composition::Invalid)
    {
        // TODO: refactor this...
        if ( (HwcConfig::GetDebugLevel() & 0x80000000) == 0 )
            __hi_log_print(
                6LL,
                "DISPLAY",
                "HWC",
                "[Disp%d]: type is invalid! at (%s:%d - %s)",
                layer->display,
                "vendor/hisi/ap/hardware/hwcomposer/libhwc/Hwc2Layer.cpp",
                183LL,
                "SetLayerCompositionType");

        return 4LL;
    }

    if (layerCompositionType == ct)
        return 0;
    
    layerCompositionType = ct;
    auto disp = HwcConfig::GetDisplay(display);
    disp->unk2[0] = 0; // TODO: figure out what this is

    return 0;
}

void Hwc2Layer::SetLayerDataspace(android_dataspace_t dt)
{
    if (layerDataspace != dt)
    {
        layerDataspace = dt;
        flags |= LAYER_DATASPACE;
    }
}

void Hwc2Layer::SetLayerDisplayFrame(hwc_rect r)
{
    if (r.left != layerDisplayFrame.left || r.right != layerDisplayFrame.right || r.top != layerDisplayFrame.top || r.bottom != layerDisplayFrame.bottom)
    {
        flags |= LAYER_DISPLAY_FRAME;
        layerDisplayFrame = r;

        if (HwcConfig::GetDebugLevel() >= 4)
            HWC_DEBUG("[Disp%d]: layer[%d]: DisplayFrame[%d, %d, %d, %d].\n", display, layer, r.left, r.top, r.right, r.bottom);
    }    
}

void Hwc2Layer::SetPlayerPlaneAlpha(float alpha)
{
    auto fp = vcvtas_u32_f32(alpha * 255.0f);
    if (layerPlaneAlpha != fp)
    {
        layerPlaneAlpha = fp;
        flags |= LAYER_PLANE_ALPHA;
    }
}

void Hwc2Layer::SetLayerSidebandStream(native_handle* ss)
{
    layerSidebandStream = ss;
}

void Hwc2Layer::SetLayerSourceCrop(hwc_frect frect)
{
    if (frect.left != layerSourceCrop.left || frect.top != layerSourceCrop.top || frect.right != layerSourceCrop.right || frect.bottom != layerSourceCrop.bottom)
    {
        flags |= LAYER_SOURCE_CROP;
        layerSourceCrop = frect;

        if (HwcConfig::GetDebugLevel() >= 4)
            HWC_DEBUG("layer[%d]: SourceCrop[%f, %f, %f, %f].\n", layer, frect.left, frect.topm frect.right, frect.bottom);
    }
}

void Hwc2Layer::SetLayerSurfaceDamage(hwc_region r)
{
   layerSurfaceDamage.clear();

   if (r.numRects)
   {
       for (size_t i = 0; i < r.numRects; i++)
       {
           android::sp<hwcAreaInfo> sp(new hwcAreaInfo());
           sp->rect = r.rects[i];
           if (HwcConfig::GetDebugLevel() >= 4)
               HWC_DEBUG("[Disp%d]: layer[%d]: Damage_Rect[%d, %d, %d, %d].\n", display, layer, sp->rect.top, sp->rect.left, sp->rect.right, sp->rect.bottom);

            layerSurfaceDamage.push_back(sp);
       }
   }
   else
   {
        android::sp<hwcAreaInfo> sp(new hwcAreaInfo());
        sp->rect.top = int(layerSourceCrop.top);
        sp->rect.bottom = int(layerSourceCrop.bottom);
        sp->rect.left = int(layerSourceCrop.left);
        sp->rect.right = int(layerSourceCrop.right);

        if (HwcConfig::GetDebugLevel() >= 4)
            HWC_DEBUG("[Disp%d]: layer[%d]: Damage_Rect[%d, %d, %d, %d].\n", display, layer, sp->rect.top, sp->rect.left, sp->rect.right, sp->rect.bottom);

       layerSurfaceDamage.push_back(sp);
   }
}

void Hwc2Layer::SetLayerTransform(int transform)
{
    if (layerTransform != transform)
    {
        flags |= LAYER_TRANSFORM;
        layerTransform = transform;
    }
}

int Hwc2Layer::SetLayerVisibleRegion(hwc_region r)
{
   if (!r.numRects)
        return 4; // 4 again?

   layerVisible.clear();

    for (size_t i = 0; i < r.numRects; i++)
    {
        android::sp<hwcAreaInfo> sp(new hwcAreaInfo());
        sp->rect = r.rects[i];
        if (HwcConfig::GetDebugLevel() >= 4)
            HWC_DEBUG("[Disp%d]: layer[%d]: visibleRegion[%d, %d, %d, %d].].\n", display, layer, sp->rect.top, sp->rect.left, sp->rect.right, sp->rect.bottom);

        layerVisible.push_back(sp);
    }

    return 0;
}

void Hwc2Layer::SetLayerZOrder(uint32_t zorder)
{
    if (zroder != this->zorder)
    {
        this->zorder = zorder;
        flags |= LAYER_Z_ORDER;
    }
}

bool Hwc2Layer::hasAlpha()
{
    if (layerPlaneAlpha != 255)
        return true;

    uint32_t ret = -2;

    if (layerBuffer)
        ret = layerBuffer->data[3] - 1;

    if (ret > 42)
        return false;
    
    return (0x40000200011 >> ret) & 1;
}

void Hwc2Layer::setCompositionType(bool flag)
{
    compositionType = flag ? 2 : 1; // TODO: make macro
}

void Hwc2Layer::init(uint32_t display)
{
    layerTransform = 0;
    layerBlendMode = 0x100;
    layerCompositionType = HWC2::Composition::Invalid;
    this->display = display;
    layerDataspace = HAL_DATASPACE_UNKNOWN;
    layerBuffer = nullptr;
    layerSidebandStream = nullptr;
    memset(&layerDisplayFrame, 0, sizeof(layerDisplayFrame));
    memset(&layerSourceCrop, 0, sizeof(layerSourceCrop));
    layerBufferChanged = false;
    compositionType = 0;
    flags = LAYER_FLAGS_NONE;
    zorder = 0;
    acquireFence = -1;
    releaseFd = -1;
    memset(&color, 0, sizeof(color));
}
