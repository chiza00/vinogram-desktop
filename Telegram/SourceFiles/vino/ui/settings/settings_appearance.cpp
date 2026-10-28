// This is the source code of VinoGram for Desktop.
//
// We do not and cannot prevent the use of our code,
// but be respectful and credit the original author.
//
// Copyright @Radolyn, 2026
#include "vino/ui/settings/settings_appearance.h"

#include "lang_auto.h"
#include "vino/vino_settings.h"
#include "vino/vino_ui_settings.h"
#include "vino/ui/boxes/font_selector.h"
#include "vino/ui/components/avatar_corners_preview.h"
#include "vino/ui/components/icon_picker.h"
#include "vino/ui/settings/vino_builder.h"
#include "vino/ui/settings/settings_vino_utils.h"
#include "vino/ui/settings/settings_main.h"
#include "inline_bots/bot_attach_web_view.h"
#include "main/main_session.h"
#include "settings/settings_builder.h"
#include "settings/settings_common.h"
#include "styles/style_vino_icons.h"
#include "styles/style_vino_styles.h"
#include "styles/style_dialogs.h"
#include "styles/style_layers.h"
#include "styles/style_menu_icons.h"
#include "styles/style_settings.h"
#include "ui/painter.h"
#include "ui/widgets/labels.h"
#include "ui/wrap/padding_wrap.h"
#include "ui/wrap/vertical_layout.h"
#include "window/window_session_controller.h"

namespace Settings {

using namespace Builder;
using namespace VinoBuilder;

namespace {

bool HasDrawerBots(not_null<Window::SessionController*> controller) {
	// todo: maybe iterate through all accounts
	const auto bots = &controller->session().attachWebView();
	for (const auto &bot : bots->attachBots()) {
		if (!bot.inMainMenu || !bot.media) {
			continue;
		}
		return true;
	}
	return false;
}

void BuildAppIcon(SectionBuilder &builder, VinoSectionBuilder &vino) {
	builder.addSubsectionTitle({
		.id = u"vino/appIcon"_q,
		.title = tr::vino_AppIconHeader(),
	});

	builder.add([](const WidgetContext &ctx) -> SectionBuilder::WidgetToAdd {
		return {
			.widget = object_ptr<IconPicker>(ctx.container),
			.margin = st::settingsButtonNoIcon.padding,
		};
	});

#if defined Q_OS_WIN || defined Q_OS_MAC
	builder.addDivider();
	builder.addSkip();
	vino.addSettingToggle({
		.id = u"vino/hideNotificationBadge"_q,
		.title = tr::vino_HideNotificationBadge(),
		.getter = &VinoSettings::hideNotificationBadge,
		.setter = &VinoSettings::setHideNotificationBadge,
	});
	builder.addSkip();
	builder.addDividerText(tr::vino_HideNotificationBadgeDescription());
	builder.addSkip();
#else
    builder.addDivider();
    builder.addSkip();
#endif
}

void BuildAvatarCorners(SectionBuilder &builder, VinoSectionBuilder &vino) {
	auto *settings = &VinoSettings::getInstance();
	const auto controller = builder.controller();

	const auto mapRadius = [](int val)
	{
		if (val == 0) {
			return tr::vino_AvatarCornersSquare(tr::now).toUpper();
		} else if (val == VinoUiSettings::kMaxAvatarCorners) {
			return tr::vino_AvatarCornersCircle(tr::now).toUpper();
		}
		return QString::number(val);
	};

	builder.add([=](const WidgetContext &ctx) -> SectionBuilder::WidgetToAdd {
		const auto container = ctx.container;
		auto title = object_ptr<Ui::FlatLabel>(
			container,
			tr::vino_AvatarCorners(),
			st::defaultSubsectionTitle);
		const auto titleRaw = title.data();

		const auto badge = Ui::CreateChild<Ui::PaddingWrap<Ui::FlatLabel>>(
			container,
			object_ptr<Ui::FlatLabel>(
				container,
				settings->avatarCornersValue() | rpl::map(mapRadius),
				st::settingsPremiumNewBadge),
			st::vinoBetaBadgePadding);
		badge->show();
		badge->setAttribute(Qt::WA_TransparentForMouseEvents);
		badge->paintRequest() | rpl::on_next([=] {
			auto p = QPainter(badge);
			auto hq = PainterHighQualityEnabler(p);
			p.setPen(Qt::NoPen);
			p.setBrush(st::windowBgActive);
			const auto r = st::vinoBetaBadgePadding.left();
			p.drawRoundedRect(badge->rect(), r, r);
		}, badge->lifetime());

		titleRaw->geometryValue() | rpl::on_next([=](QRect geometry) {
			badge->moveToLeft(
				geometry.x()
					+ titleRaw->textMaxWidth()
					+ st::settingsPremiumNewBadgePosition.x(),
				geometry.y()
					+ (geometry.height() - badge->height()) / 2);
		}, badge->lifetime());

		return {
			.widget = std::move(title),
			.margin = st::defaultSubsectionTitlePadding,
		};
	}, [] {
		return SearchEntry{
			.id = u"vino/avatarCorners"_q,
			.title = tr::vino_AvatarCorners(tr::now),
		};
	});

	auto *previewRaw = static_cast<AvatarCornersPreview*>(nullptr);
	builder.add([&](const Builder::WidgetContext &ctx) -> SectionBuilder::WidgetToAdd {
		auto preview = object_ptr<AvatarCornersPreview>(
			ctx.container,
			controller);
		previewRaw = preview.data();
		const auto vMargin = st::settingsButtonNoIcon.padding
			- st::defaultDialogRow.padding;
		return {
			.widget = std::move(preview),
			.margin = QMargins(0, vMargin.top(), 0, vMargin.bottom()),
		};
	});

	vino.addSlider({
		.id = u"vino/avatarCornersSlider"_q,
		.title = rpl::single(QString()),
		.showTitle = false,
		.steps = VinoUiSettings::kMaxAvatarCorners + 1,
		.current = settings->avatarCorners(),
		.onChanged = [=](int val) {
			VinoSettings::getInstance().setAvatarCorners(val);
			if (previewRaw) {
				previewRaw->update();
			}
		},
		.onFinalChanged = [=](int val) {
			VinoSettings::getInstance().setAvatarCorners(val);
			ShowRestartPrompt(controller);
		},
	});

	vino.addSettingToggle({
		.id = u"vino/singleCornerRadius"_q,
		.title = tr::vino_SingleCornerRadius(),
		.getter = &VinoSettings::singleCornerRadius,
		.setter = &VinoSettings::setSingleCornerRadius,
	});

	builder.addSkip();
	builder.addDividerText(tr::vino_SingleCornerRadiusDescription());
	builder.addSkip();
}

void BuildAppearance(SectionBuilder &builder, VinoSectionBuilder &vino) {
	auto *settings = &VinoSettings::getInstance();

	builder.addSubsectionTitle(tr::vino_CategoryAppearance());

	vino.addSettingToggle({
		.id = u"vino/materialSwitches"_q,
		.altIds = { u"vino/newSwitchStyle"_q },
		.title = tr::vino_MaterialSwitches(),
		.getter = &VinoSettings::materialSwitches,
		.setter = &VinoSettings::setMaterialSwitches,
	});
	vino.addSettingToggle({
		.id = u"vino/disableCustomBackgrounds"_q,
		.altIds = { u"vino/customThemes"_q },
		.title = tr::vino_DisableCustomBackgrounds(),
		.getter = &VinoSettings::disableCustomBackgrounds,
		.setter = &VinoSettings::setDisableCustomBackgrounds,
	});
	vino.addSettingToggle({
		.id = u"vino/hidePremiumStatuses"_q,
		.title = tr::vino_HidePremiumStatuses(),
		.getter = &VinoSettings::hidePremiumStatuses,
		.setter = &VinoSettings::setHidePremiumStatuses,
	});

	const auto controller = builder.controller();
	builder.addButton({
		.id = u"vino/monoFont"_q,
		.title = tr::vino_MonospaceFont(),
		.st = &st::settingsButtonNoIcon,
		.label = rpl::single(
			settings->monoFont().isEmpty()
				? tr::vino_FontDefault(tr::now)
				: settings->monoFont()),
		.onClick = [=] {
			VinoUi::FontSelectorBox::Show(
				controller,
				[=](const QString &font) {
					VinoSettings::getInstance().setMonoFont(font);
				});
		},
	});

	vino.addSectionDivider();
}

void BuildChatFolders(SectionBuilder &builder, VinoSectionBuilder &vino) {
	builder.addSubsectionTitle(tr::vino_ChatFoldersHeader());

	vino.addSettingToggle({
		.id = u"vino/hideNotificationCounters"_q,
		.altIds = { u"vino/tabCounter"_q },
		.title = tr::vino_HideNotificationCounters(),
		.getter = &VinoSettings::hideNotificationCounters,
		.setter = &VinoSettings::setHideNotificationCounters,
	});
	vino.addSettingToggle({
		.id = u"vino/hideAllChatsFolder"_q,
		.altIds = { u"vino/hideAllChats"_q },
		.title = tr::vino_HideAllChats(),
		.getter = &VinoSettings::hideAllChatsFolder,
		.setter = &VinoSettings::setHideAllChatsFolder,
	});

	vino.addSectionDivider();
}

void BuildTrayElements(SectionBuilder &builder, VinoSectionBuilder &vino) {
	builder.addSubsectionTitle(tr::vino_TrayElementsHeader());

	vino.addSettingToggle({
		.id = u"vino/showGhostToggleInTray"_q,
		.title = tr::vino_EnableGhostModeTray(),
		.getter = &VinoSettings::showGhostToggleInTray,
		.setter = &VinoSettings::setShowGhostToggleInTray,
	});

#if defined Q_OS_WIN || defined Q_OS_MAC
	vino.addSettingToggle({
		.id = u"vino/showStreamerToggleInTray"_q,
		.title = tr::vino_EnableStreamerModeTray(),
		.getter = &VinoSettings::showStreamerToggleInTray,
		.setter = &VinoSettings::setShowStreamerToggleInTray,
	});
#endif

	vino.addSectionDivider();
}

void BuildDrawerElements(SectionBuilder &builder, VinoSectionBuilder &vino) {
	builder.addSubsectionTitle(tr::vino_DrawerElementsHeader());

	vino.addSettingToggle({
		.id = u"vino/showMyProfileInDrawer"_q,
		.title = tr::lng_menu_my_profile(),
		.getter = &VinoSettings::showMyProfileInDrawer,
		.setter = &VinoSettings::setShowMyProfileInDrawer,
		.icon = { &st::menuIconProfile },
	});

	const auto controller = builder.controller();
	if (controller && HasDrawerBots(controller)) {
		vino.addSettingToggle({
			.id = u"vino/showBotsInDrawer"_q,
			.title = tr::lng_filters_type_bots(),
			.getter = &VinoSettings::showBotsInDrawer,
			.setter = &VinoSettings::setShowBotsInDrawer,
			.icon = { &st::menuIconBot },
		});
	}

	vino.addSettingToggle({
		.id = u"vino/showNewGroupInDrawer"_q,
		.title = tr::lng_create_group_title(),
		.getter = &VinoSettings::showNewGroupInDrawer,
		.setter = &VinoSettings::setShowNewGroupInDrawer,
		.icon = { &st::menuIconGroups },
	});
	vino.addSettingToggle({
		.id = u"vino/showNewChannelInDrawer"_q,
		.title = tr::lng_create_channel_title(),
		.getter = &VinoSettings::showNewChannelInDrawer,
		.setter = &VinoSettings::setShowNewChannelInDrawer,
		.icon = { &st::menuIconChannel },
	});
	vino.addSettingToggle({
		.id = u"vino/showContactsInDrawer"_q,
		.title = tr::lng_menu_contacts(),
		.getter = &VinoSettings::showContactsInDrawer,
		.setter = &VinoSettings::setShowContactsInDrawer,
		.icon = { &st::menuIconUserShow },
	});
	vino.addSettingToggle({
		.id = u"vino/showCallsInDrawer"_q,
		.title = tr::lng_menu_calls(),
		.getter = &VinoSettings::showCallsInDrawer,
		.setter = &VinoSettings::setShowCallsInDrawer,
		.icon = { &st::menuIconPhone },
	});
	vino.addSettingToggle({
		.id = u"vino/showSavedMessagesInDrawer"_q,
		.title = tr::lng_saved_messages(),
		.getter = &VinoSettings::showSavedMessagesInDrawer,
		.setter = &VinoSettings::setShowSavedMessagesInDrawer,
		.icon = { &st::menuIconSavedMessages },
	});
	vino.addSettingToggle({
		.id = u"vino/showLReadToggleInDrawer"_q,
		.title = tr::vino_LReadMessages(),
		.getter = &VinoSettings::showLReadToggleInDrawer,
		.setter = &VinoSettings::setShowLReadToggleInDrawer,
		.icon = { &st::vinoLReadMenuIcon },
	});
	vino.addSettingToggle({
		.id = u"vino/showSReadToggleInDrawer"_q,
		.title = tr::vino_SReadMessages(),
		.getter = &VinoSettings::showSReadToggleInDrawer,
		.setter = &VinoSettings::setShowSReadToggleInDrawer,
		.icon = { &st::vinoSReadMenuIcon },
	});
	vino.addSettingToggle({
		.id = u"vino/showNightModeToggleInDrawer"_q,
		.title = tr::lng_menu_night_mode(),
		.getter = &VinoSettings::showNightModeToggleInDrawer,
		.setter = &VinoSettings::setShowNightModeToggleInDrawer,
		.icon = { &st::menuIconNightMode },
	});
	vino.addSettingToggle({
		.id = u"vino/showGhostToggleInDrawer"_q,
		.title = tr::vino_GhostModeToggle(),
		.getter = &VinoSettings::showGhostToggleInDrawer,
		.setter = &VinoSettings::setShowGhostToggleInDrawer,
		.icon = { &st::vinoGhostIcon },
	});

#if defined Q_OS_WIN || defined Q_OS_MAC
	vino.addSettingToggle({
		.id = u"vino/showStreamerToggleInDrawer"_q,
		.title = tr::vino_StreamerModeToggle(),
		.getter = &VinoSettings::showStreamerToggleInDrawer,
		.setter = &VinoSettings::setShowStreamerToggleInDrawer,
		.icon = { &st::vinoStreamerModeMenuIcon },
	});
#endif

	builder.addSkip();
}

const auto kMeta = BuildHelper({
	.id = VinoAppearance::Id(),
	.parentId = VinoMain::Id(),
	.title = &tr::vino_CategoryAppearance,
	.icon = &st::menuIconPalette,
}, [](SectionBuilder &builder) {
	auto vino = VinoSectionBuilder(builder);

	builder.addSkip();
	BuildAppIcon(builder, vino);
	BuildAvatarCorners(builder, vino);
	BuildAppearance(builder, vino);
	BuildChatFolders(builder, vino);
	BuildTrayElements(builder, vino);
	BuildDrawerElements(builder, vino);
	builder.addSkip();
});

} // namespace

rpl::producer<QString> VinoAppearance::title() {
	return tr::vino_CategoryAppearance();
}

VinoAppearance::VinoAppearance(
	QWidget *parent,
	not_null<Window::SessionController*> controller)
: Section(parent, controller) {
	setupContent();
}

void VinoAppearance::setupContent() {
	const auto content = Ui::CreateChild<Ui::VerticalLayout>(this);
	build(content, kMeta.build);
	Ui::ResizeFitChild(this, content);
}

Type VinoAppearanceId() {
	return VinoAppearance::Id();
}

} // namespace Settings
