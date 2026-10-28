// This is the source code of VinoGram for Desktop.
//
// We do not and cannot prevent the use of our code,
// but be respectful and credit the original author.
//
// Copyright @Radolyn, 2026

#include "vino/features/streamer_mode/platform/mac/streamer_mode_mac.h"

#include <QtWidgets/QWidget>

#include <Cocoa/Cocoa.h>

namespace VinoFeatures::StreamerMode::Platform {

void SetWindowCaptureExcluded(
		not_null<QWidget*> widget,
		bool excluded) {
	const auto view = reinterpret_cast<NSView*>(widget->winId());
	view.window.sharingType = excluded
		? NSWindowSharingNone
		: NSWindowSharingReadOnly;
}

} // namespace VinoFeatures::StreamerMode::Platform
