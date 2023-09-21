/*!
    @file StdAfx.h
    @brief Precompiler header/shared inclusions
    @author Arves100
    @date 19/04/2022
*/
#pragma once

#include <hardware/hwcomposer_defs.h>
#include <hardware/hwcomposer2.h>

#include <utils/StrongPointer.h>
#include <utils/List.h>

#define HWC_WARN(fmt, ...) __android_log_print(ANDROID_LOG_WARN, "hwcomposer", "%s%d: " fmt, __FILE__, __LINE__, ...)
#define HWC_INFO(fmt, ...) __android_log_print(ANDROID_LOG_INFO, "hwcomposer", "%s%d: " fmt, __FILE__, __LINE__, ...)
#define HWC_DEBUG(fmt, ...) __android_log_print(ANDROID_LOG_DEBUG, "hwcomposer", "%s%d: " fmt, __FILE__, __LINE__, ...)
