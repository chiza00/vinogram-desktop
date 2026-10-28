// This is the source code of VinoGram for Desktop.
//
// We do not and cannot prevent the use of our code,
// but be respectful and credit the original author.
//
// Copyright @Radolyn, 2026
#include "vino/ui/settings/settings_main.h"

#include "settings/sections/settings_main.h"
#include "lang_auto.h"
#include "vino/vino_settings.h"
#include "vino/ui/vino_logo.h"
#include "vino/ui/settings/settings_appearance.h"
#include "vino/ui/settings/settings_vino.h"
#include "vino/ui/settings/settings_chats.h"
#include "vino/ui/settings/settings_filters.h"
#include "vino/ui/settings/settings_general.h"
#include "vino/ui/settings/settings_other.h"
#include "core/version.h"
#include "settings/settings_builder.h"
#include "settings/settings_common.h"
#include "styles/style_vino_settings.h"
#include "styles/style_layers.h"
#include "styles/style_menu_icons.h"
#include "styles/style_settings.h"
#include "ui/painter.h"
#include "ui/widgets/buttons.h"
#include "ui/widgets/labels.h"
#include "ui/wrap/vertical_layout.h"
#include "window/window_session_controller.h"
#include "window/window_session_controller_link_info.h"

#include <QDesktopServices>

namespace Settings {

using namespace Builder;

namespace {

void BuildLogo(SectionBuilder &builder) {
	builder.add([](const WidgetContext &ctx) -> SectionBuilder::WidgetToAdd {
		auto logo = object_ptr<Ui::RpWidget>(ctx.container);
		const auto logoRaw = logo.data();
		logoRaw->resize(
			QSize(st::settingsCloudPasswordIconSize,
				st::settingsCloudPasswordIconSize));
		logoRaw->setNaturalWidth(st::settingsCloudPasswordIconSize);
		logoRaw->paintRequest(
		) | rpl::on_next([=] {
			auto p = QPainter(logoRaw);
			const auto image = VinoAssets::currentAppLogoPad();
			if (!image.isNull()) {
				const auto size = st::settingsCloudPasswordIconSize;
				const auto scaled = image.scaled(
					size * style::DevicePixelRatio(),
					size * style::DevicePixelRatio(),
					Qt::KeepAspectRatio,
					Qt::SmoothTransformation);
				p.drawImage(QRect(0, 0, size, size), scaled);
			}
		}, logoRaw->lifetime());
		return { .widget = std::move(logo), .align = style::al_top };
	});
}

void BuildVersionInfo(SectionBuilder &builder) {
	builder.add([](const WidgetContext &ctx) -> SectionBuilder::WidgetToAdd {
		return {
			.widget = object_ptr<Ui::FlatLabel>(
				ctx.container,
				rpl::single(
					QString("VinoGram Desktop v")
					+ QString::fromLatin1(AppVersionStr)),
				st::boxTitle),
			.align = style::al_top,
		};
	});

	builder.addSkip();

	builder.add([](const WidgetContext &ctx) -> SectionBuilder::WidgetToAdd {
		return {
			.widget = object_ptr<Ui::FlatLabel>(
				ctx.container,
				tr::vino_SettingsDescription(),
				st::centeredBoxLabel),
			.align = style::al_top,
		};
	});
}

void BuildCategories(SectionBuilder &builder) {
	builder.addSkip();
	builder.addSkip();
	builder.addSkip();
	builder.addSkip();
	builder.addDivider();
	builder.addSkip();

	builder.addSubsectionTitle(tr::vino_CategoriesHeader());

	builder.addSectionButton({
		.title = rpl::single(QString("VinoGram")),
		.targetSection = VinoGhost::Id(),
		.icon = { &st::menuIconGroupReactions },
	});
	builder.addSectionButton({
		.title = tr::vino_CategoryFilters(),
		.targetSection = VinoFilters::Id(),
		.icon = { &st::menuIconTagFilter },
	});
	builder.addSectionButton({
		.title = tr::vino_CategoryGeneral(),
		.targetSection = VinoGeneral::Id(),
		.icon = { &st::menuIconShowAll },
	});
	builder.addSectionButton({
		.title = tr::vino_CategoryAppearance(),
		.targetSection = VinoAppearance::Id(),
		.icon = { &st::menuIconPalette },
	});
	builder.addSectionButton({
		.title = tr::vino_CategoryChats(),
		.targetSection = VinoChats::Id(),
		.icon = { &st::menuIconChatBubble },
	});
	builder.addSectionButton({
		.title = tr::vino_CategoryOther(),
		.targetSection = VinoOther::Id(),
		.icon = { &st::menuIconFave },
	});
}

void BuildLinks(SectionBuilder &builder) {
	builder.addSkip();
	builder.addDivider();
	builder.addSkip();

	builder.addSubsectionTitle(tr::vino_LinksHeader());

	const auto controller = builder.controller();

	builder.addButton({
		.id = u"vino/channel"_q,
		.title = tr::vino_LinksChannel(),
		.icon = { &st::menuIconChannel },
		.label = rpl::single(QString("@vinogram")),
		.onClick = [=] {
			controller->showPeerByLink(Window::PeerByLinkInfo{
				.usernameOrId = QString("vinogram"),
			});
		},
	});
	builder.addButton({
		.id = u"vino/chat"_q,
		.title = tr::vino_LinksChats(),
		.icon = { &st::menuIconChats },
		.label = rpl::single(QString("@vinogramchat")),
		.onClick = [=] {
			controller->showPeerByLink(Window::PeerByLinkInfo{
				.usernameOrId = QString("vinogramchat"),
			});
		},
	});
	builder.addButton({
		.id = u"vino/crowdin"_q,
		.title = tr::vino_LinksTranslate(),
		.icon = { &st::menuIconTranslate },
		.label = rpl::single(QString("Crowdin")),
		.onClick = [=] {
			QDesktopServices::openUrl(
				QString("https://translate.vinogram.one"));
		},
	});
	builder.addButton({
		.id = u"vino/website"_q,
		.title = tr::vino_LinksDocumentation(),
		.icon = { &st::menuIconIpAddress },
		.label = rpl::single(QString("docs.vinogram.one")),
		.onClick = [=] {
			QDesktopServices::openUrl(
				QString("https://docs.vinogram.one"));
		},
	});

	builder.addSkip();
}

const auto kMeta = BuildHelper({
	.id = VinoMain::Id(),
	.parentId = MainId(),
	.title = &tr::vino_Preferences,
	.icon = &st::menuIconPremium,
}, [](SectionBuilder &builder) {
	BuildLogo(builder);
	builder.addSkip();
	BuildVersionInfo(builder);
	BuildCategories(builder);
	BuildLinks(builder);
});

} // namespace

rpl::producer<QString> VinoMain::title() {
	return rpl::single(QString(""));
}

VinoMain::VinoMain(
	QWidget *parent,
	not_null<Window::SessionController*> controller)
: Section(parent, controller) {
	setupContent();
}

void VinoMain::setupContent() {
	const auto content = Ui::CreateChild<Ui::VerticalLayout>(this);
	build(content, kMeta.build);
	Ui::ResizeFitChild(this, content);
}

Type VinoMainId() {
	return VinoMain::Id();
}

} // namespace Settings
