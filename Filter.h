#pragma once

#include "Rgba.h"
#include "Exception.h"

std::vector<Rgba> Filter(const std::vector<Rgba>& srcBuf, UINT width, UINT height, bool includeAlpha = false) throw(...);
