// This is the source code of VinoGram for Desktop.
//
// We do not and cannot prevent the use of our code,
// but be respectful and credit the original author.
//
// Copyright @Radolyn, 2026
#pragma once

#include <QString>

namespace VinoUiSettings {

// WHY: the low-level paint and font code cannot reach VinoSettings
// (no session there), so the values are mirrored into these globals
// from VinoInfra on startup and on every change.
constexpr auto kMaxAvatarCorners = 24;

[[nodiscard]] inline int &AvatarCornersValue() {
	static auto value = kMaxAvatarCorners;
	return value;
}

[[nodiscard]] inline int getAvatarCorners() {
	return AvatarCornersValue();
}

inline void setAvatarCorners(int corners) {
	AvatarCornersValue() = corners;
}

[[nodiscard]] inline QString &MonoFontValue() {
	static auto value = QString();
	return value;
}

[[nodiscard]] inline QString getMonoFont() {
	return MonoFontValue();
}

inline void setMonoFont(const QString &font) {
	MonoFontValue() = font;
}

[[nodiscard]] inline double &WideMultiplierValue() {
	static auto value = 1.0;
	return value;
}

[[nodiscard]] inline double getWideMultiplier() {
	return WideMultiplierValue();
}

inline void setWideMultiplier(double multiplier) {
	WideMultiplierValue() = multiplier;
}

[[nodiscard]] inline bool &MaterialSwitchesValue() {
	static auto value = true;
	return value;
}

[[nodiscard]] inline bool getMaterialSwitches() {
	return MaterialSwitchesValue();
}

inline void setMaterialSwitches(bool material) {
	MaterialSwitchesValue() = material;
}

} // namespace VinoUiSettings
