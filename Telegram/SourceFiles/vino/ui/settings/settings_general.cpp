// This is the source code of VinoGram for Desktop.
//
// We do not and cannot prevent the use of our code,
// but be respectful and credit the original author.
//
// Copyright @Radolyn, 2026
#include "vino/ui/settings/settings_general.h"

#include "lang_auto.h"
#include "vino/vino_settings.h"
#include "vino/ui/settings/vino_builder.h"
#include "vino/ui/settings/settings_vino_utils.h"
#include "vino/ui/settings/settings_main.h"
#include "base/platform/base_platform_info.h"
#include "core/application.h"
#include "lang/lang_text_entity.h"
#include "platform/platform_translate_provider.h"
#include "settings/settings_builder.h"
#include "settings/settings_common.h"
#include "styles/style_menu_icons.h"
#include "styles/style_settings.h"
#include "ui/boxes/single_choice_box.h"
#include "ui/toast/toast.h"
#include "ui/widgets/buttons.h"
#include "ui/wrap/vertical_layout.h"
#include "window/window_controller.h"
#include "window/window_session_controller.h"

namespace Settings {

using namespace Builder;
using namespace VinoBuilder;

namespace {

void BuildTranslator(SectionBuilder &builder, VinoSectionBuilder &vino) {
	builder.addSubsectionTitle(tr::lng_translate_settings_subtitle());

	auto *settings = &VinoSettings::getInstance();

	const auto options = std::vector{
		std::pair(TranslationProvider::Telegram, QString("Telegram")),
		std::pair(TranslationProvider::Google, QString("Google")),
		std::pair(TranslationProvider::Yandex, QString("Yandex")),
	};
	const auto nativeAvailable = Platform::IsTranslateProviderAvailable();
	auto availableOptions = options;
	if (nativeAvailable) {
		availableOptions.push_back(std::pair(
			TranslationProvider::Native,
			[] {
				if constexpr (Platform::IsMac()) {
					return QString("macOS");
				} else if constexpr (Platform::IsWindows()) {
					return QString("Windows");
				} else {
					return QString("Linux");
				}
			}()));
	}
	auto optionLabels = std::vector<QString>();
	optionLabels.reserve(availableOptions.size());
	for (const auto &option : availableOptions) {
		optionLabels.push_back(option.second);
	}

	const auto getIndex = [=](TranslationProvider val) {
		const auto i = ranges::find(
			availableOptions,
			val,
			&std::pair<TranslationProvider, QString>::first);
		return (i != end(availableOptions))
			? int(i - begin(availableOptions))
			: 0;
	};

	auto currentVal = VinoSettings::getInstance().translationProviderValue()
		| rpl::map(getIndex)
		| rpl::map([=](int val) { return availableOptions[val].second; });

	const auto button = builder.addButton({
		.id = u"vino/translationProvider"_q,
		.title = tr::vino_TranslationProvider(),
		.st = &st::settingsButtonNoIcon,
		.label = std::move(currentVal),
		.onClick = [=] {
			if (const auto controller = Core::App().activeWindow()->sessionController()) {
				controller->show(Box(
						[=](not_null<Ui::GenericBox*> box) {
							const auto save = [=](int index) {
								const auto option = availableOptions[index].first;
								VinoSettings::getInstance().setTranslationProvider(option);

								if constexpr (Platform::IsMac()) {
									if (option == TranslationProvider::Native) {
										controller->showToast(Ui::Toast::Config{
											.text = tr::lng_translate_settings_use_platform_mac_about(tr::now, tr::rich),
											.duration = 6 * crl::time(1000)
										});
									}
								}
							};
							SingleChoiceBox(box, {
								.title = tr::vino_TranslationProvider(),
								.options = optionLabels,
								.initialSelection = getIndex(settings->translationProvider()),
								.callback = save,
							});
						}));
			}
		},
	});
	if (button) {
		vino.addBetaBadge(button);
	}
}

void BuildShowPeerId(SectionBuilder &builder) {
	auto *settings = &VinoSettings::getInstance();

	const auto options = std::vector{
		QString(tr::vino_SettingsShowID_Hide(tr::now)),
		QString("Telegram API"),
		QString("Bot API")
	};

	auto currentVal = VinoSettings::getInstance().showPeerIdValue()
		| rpl::map([=](PeerIdDisplay val) {
			return options[static_cast<int>(val)];
		});

	const auto controller = builder.controller();
	builder.addButton({
		.id = u"vino/showPeerId"_q,
		.altIds = { u"vino/showIdAndDc"_q },
		.title = tr::vino_SettingsShowID(),
		.st = &st::settingsButtonNoIcon,
		.label = std::move(currentVal),
		.onClick = [=] {
			controller->show(Box(
				[=](not_null<Ui::GenericBox*> box) {
					const auto save = [=](int index) {
						VinoSettings::getInstance().setShowPeerId(
							static_cast<PeerIdDisplay>(index));
					};
					SingleChoiceBox(box, {
						.title = tr::vino_SettingsShowID(),
						.options = options,
						.initialSelection = static_cast<int>(settings->showPeerId()),
						.callback = save,
					});
				}));
		},
	});
}

void BuildQoLToggles(SectionBuilder &builder, VinoSectionBuilder &vino) {
	auto *settings = &VinoSettings::getInstance();

	BuildTranslator(builder, vino);
	vino.addSectionDivider();

	builder.addSubsectionTitle(tr::vino_CategoryGeneral());

	const auto controller = builder.controller();
	vino.addToggle({
		.id = u"vino/disableStories"_q,
		.altIds = { u"vino/hideStories"_q },
		.title = tr::vino_DisableStories(),
		.getter = [=] { return settings->disableStories(); },
		.setter = [=](bool enabled) {
			VinoSettings::getInstance().setDisableStories(enabled);
			ShowRestartPrompt(controller);
		},
	});

	vino.addSettingToggle({
		.id = u"vino/disableOpenLinkWarning"_q,
		.title = tr::vino_DisableOpenLinkWarning(),
		.getter = &VinoSettings::disableOpenLinkWarning,
		.setter = &VinoSettings::setDisableOpenLinkWarning,
	});

	vino.addCollapsibleToggle({
		.id = u"vino/similarChannels"_q,
		.title = tr::vino_DisableSimilarChannels(),
		.checkboxes = {
			NestedEntry{
				tr::vino_CollapseSimilarChannels(tr::now),
				[] { return VinoSettings::getInstance().collapseSimilarChannels(); },
				[](bool v) { VinoSettings::getInstance().setCollapseSimilarChannels(v); }
			},
			NestedEntry{
				tr::vino_HideSimilarChannelsTab(tr::now),
				[] { return VinoSettings::getInstance().hideSimilarChannels(); },
				[](bool v) { VinoSettings::getInstance().setHideSimilarChannels(v); }
			}
		},
		.toggledWhenAll = true,
	});

	vino.addSettingToggle({
		.id = u"vino/disableNotificationsDelay"_q,
		.title = tr::vino_DisableNotificationsDelay(),
		.getter = &VinoSettings::disableNotificationsDelay,
		.setter = &VinoSettings::setDisableNotificationsDelay,
	});

	vino.addSectionDivider();

	const auto zalgoButton = builder.addButton({
		.id = u"vino/filterZalgo"_q,
		.title = tr::vino_FilterZalgo(),
		.st = &st::settingsButtonNoIcon,
		.toggled = rpl::single(settings->filterZalgo()),
	});
	if (zalgoButton) {
		zalgoButton->toggledValue(
		) | rpl::filter(
			[=](bool enabled) {
				return (enabled != settings->filterZalgo());
			}
		) | on_next(
			[=](bool enabled) {
				VinoSettings::getInstance().setFilterZalgo(enabled);
				ShowRestartPrompt(controller);
			},
			zalgoButton->lifetime());
		vino.addBetaBadge(zalgoButton);
	}

	vino.addSettingToggle({
		.id = u"vino/improveLinkPreviews"_q,
		.title = tr::vino_ImproveLinkPreviews(),
		.getter = &VinoSettings::improveLinkPreviews,
		.setter = &VinoSettings::setImproveLinkPreviews,
	});
	vino.addCollapsibleToggle({
		.id = u"vino/confirmations"_q,
		.title = tr::vino_ConfirmationsTitle(),
		.checkboxes = {
			NestedEntry{
				tr::vino_StickerConfirmation(tr::now),
				[] { return VinoSettings::getInstance().stickerConfirmation(); },
				[](bool v) { VinoSettings::getInstance().setStickerConfirmation(v); }
			},
			NestedEntry{
				tr::vino_GIFConfirmation(tr::now),
				[] { return VinoSettings::getInstance().gifConfirmation(); },
				[](bool v) { VinoSettings::getInstance().setGifConfirmation(v); }
			},
			NestedEntry{
				tr::vino_VoiceConfirmation(tr::now),
				[] { return VinoSettings::getInstance().voiceConfirmation(); },
				[](bool v) { VinoSettings::getInstance().setVoiceConfirmation(v); }
			},
			NestedEntry{
				tr::vino_RoundConfirmation(tr::now),
				[] { return VinoSettings::getInstance().roundConfirmation(); },
				[](bool v) { VinoSettings::getInstance().setRoundConfirmation(v); }
			}
		},
		.toggledWhenAll = false,
	});
	vino.addSettingToggle({
		.id = u"vino/showMessageSeconds"_q,
		.altIds = { u"vino/formatTimeWithSeconds"_q },
		.title = tr::vino_SettingsShowMessageSeconds(),
		.getter = &VinoSettings::showMessageSeconds,
		.setter = &VinoSettings::setShowMessageSeconds,
	});

	BuildShowPeerId(builder);

	vino.addSectionDivider();

	builder.addSubsectionTitle(rpl::single(QString("Webview")));

	vino.addSettingToggle({
		.id = u"vino/spoofWebviewAsAndroid"_q,
		.title = tr::vino_SettingsSpoofWebviewAsAndroid(),
		.getter = &VinoSettings::spoofWebviewAsAndroid,
		.setter = &VinoSettings::setSpoofWebviewAsAndroid,
	});

	vino.addCollapsibleToggle({
		.id = u"vino/biggerWindow"_q,
		.title = tr::vino_SettingsBiggerWindow(),
		.checkboxes = {
			NestedEntry{
				tr::vino_SettingsIncreaseWebviewHeight(tr::now),
				[] { return VinoSettings::getInstance().increaseWebviewHeight(); },
				[](bool v) { VinoSettings::getInstance().setIncreaseWebviewHeight(v); }
			},
			NestedEntry{
				tr::vino_SettingsIncreaseWebviewWidth(tr::now),
				[] { return VinoSettings::getInstance().increaseWebviewWidth(); },
				[](bool v) { VinoSettings::getInstance().setIncreaseWebviewWidth(v); }
			}
		},
		.toggledWhenAll = false,
	});
}

const auto kMeta = BuildHelper({
	.id = VinoGeneral::Id(),
	.parentId = VinoMain::Id(),
	.title = &tr::vino_CategoryGeneral,
	.icon = &st::menuIconShowAll,
}, [](SectionBuilder &builder) {
	auto vino = VinoSectionBuilder(builder);

	builder.addSkip();
	BuildQoLToggles(builder, vino);
	builder.addSkip();
});

} // namespace

rpl::producer<QString> VinoGeneral::title() {
	return tr::vino_CategoryGeneral();
}

VinoGeneral::VinoGeneral(
	QWidget *parent,
	not_null<Window::SessionController*> controller)
: Section(parent, controller) {
	setupContent();
}

void VinoGeneral::setupContent() {
	const auto content = Ui::CreateChild<Ui::VerticalLayout>(this);
	build(content, kMeta.build);
	Ui::ResizeFitChild(this, content);
}

Type VinoGeneralId() {
	return VinoGeneral::Id();
}

} // namespace Settings
