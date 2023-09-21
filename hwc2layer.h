/*!
    @file hwc2layer.h
    @brief Hardware compositor layer
    @author Arves100
    @date 19/04/2022
*/

#include "hwcAreaInfo.h"

#pragma once

class Hwc2Layer
{
public:
    Hwc2Layer(uint32_t display);
    ~Hwc2Layer();

    uint32_t GetFormat();
    uint32_t GetUint32Color(hwc_color const& color);
    bool hasAlpha();

    void SetLayerBlendMode(uint32_t mode);
    void SetLayerBuffer(native_handle const* buffer, int acquireFence);
    void SetLayerColor(hwc_color color);
    int SetlayerCompositionType(HWC2::Composition type);
    void SetLayerDataspace(android_dataspace_t dataspace);
    void SetLayerDisplayFrame(hwc_rect rect);
    void SetLayerPlaneAlpha(float alpha);
    void SetLayerSidebandStream(native_handle* handle);
    void SetLayerSourceCrop(hwc_frect frect);
    void SetLayerSurfaceDamage(hwc_region region);
    void SetLayerTransform(int transform);
    int SetLayerVisibleRegion(hwc_region region);
    void SetLayerZOrder(uint order);
    void setCompositionType(bool ct);

    void copyFrom(Hwc2Layer const* src);

protected:
    void init(ulong display);

    native_handle* layerBuffer;
    native_handle* layerSidebandStream;
    int layerTransform;
    uint32_t layerBlendMode;
    hwc_frect layerSourceCrop;
    hwc_rect layerDisplayFrame;
    android::List<android::sp<hwcAreaInfo>> layerSurfaceDamage; // sp should be reference pointer hehe, not sure which type was it tbh
    android::List<android::sp<hwcAreaInfo>> layerVisible;
    android_dataspace_t layerDataspace;
    hwc_color color;
    uint32_t layerPlaneAlpha;
    uint32_t zorder;
    int acquireFence;
    uint32_t releaseFd;
    uint32_t compositionType;
    HWC::Composition layerCompositionType;
    uint32_t display;
    uint32_t layer;
    HwcLayerFlags flags;
    bool layerBufferChanged;
    uint32_t changedColor;
};
