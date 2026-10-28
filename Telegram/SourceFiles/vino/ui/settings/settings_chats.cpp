// This is the source code of VinoGram for Desktop.
//
// We do not and cannot prevent the use of our code,
// but be respectful and credit the original author.
//
// Copyright @Radolyn, 2026
#include "vino/ui/settings/settings_chats.h"

#include "lang_auto.h"
#include "vino/vino_settings.h"
#include "vino/ui/boxes/edit_mark_box.h"
#include "vino/ui/components/message_preview.h"
#include "vino/ui/settings/vino_builder.h"
#include "vino/ui/settings/settings_vino_utils.h"
#include "vino/ui/settings/settings_main.h"
#include "settings/settings_builder.h"
#include "settings/settings_common.h"
#include "styles/style_vino_icons.h"
#include "styles/style_menu_icons.h"
#include "styles/style_settings.h"
#include "ui/wrap/vertical_layout.h"
#include "window/window_session_controller.h"

#include <memory>

namespace Settings {

using namespace Builder;
using namespace VinoBuilder;

namespace {

struct PreviewState {
	MessagePreview *widget = nullptr;
};

void BuildStickersAndEmoji(SectionBuilder &builder, VinoSectionBuilder &vino) {
	builder.addSubsectionTitle(tr::lng_settings_stickers_emoji());

	vino.addSettingToggle({
		.id = u"vino/showOnlyAddedEmojisAndStickers"_q,
		.title = tr::vino_ShowOnlyAddedEmojisAndStickers(),
		.getter = &VinoSettings::showOnlyAddedEmojisAndStickers,
		.setter = &VinoSettings::setShowOnlyAddedEmojisAndStickers,
	});

	vino.addSettingToggle({
		.id = u"vino/unlimitedRecentStickers"_q,
		.altIds = { u"vino/recentStickersCount"_q },
		.title = tr::vino_SettingsUnlimitedRecentStickers(),
		.getter = &VinoSettings::unlimitedRecentStickers,
		.setter = &VinoSettings::setUnlimitedRecentStickers,
	});

	vino.addCollapsibleToggle({
		.id = u"vino/hideReactions"_q,
		.title = tr::vino_HideReactions(),
		.checkboxes = {
			NestedEntry{
				tr::vino_HideReactionsInChannels(tr::now),
				[] { return !VinoSettings::getInstance().showChannelReactions(); },
				[](bool v) { VinoSettings::getInstance().setShowChannelReactions(!v); }
			},
			NestedEntry{
				tr::vino_HideReactionsInGroups(tr::now),
				[] { return !VinoSettings::getInstance().showGroupReactions(); },
				[](bool v) { VinoSettings::getInstance().setShowGroupReactions(!v); }
			},
			NestedEntry{
				tr::vino_HideReactionsInPrivateChats(tr::now),
				[] { return !VinoSettings::getInstance().showPrivateChatReactions(); },
				[](bool v) { VinoSettings::getInstance().setShowPrivateChatReactions(!v); }
			}
		},
		.toggledWhenAll = false,
	});

	vino.addSectionDivider();
}

void BuildGroupsAndChannels(SectionBuilder &builder, VinoSectionBuilder &vino) {
	auto *settings = &VinoSettings::getInstance();

	builder.addSubsectionTitle(tr::lng_premium_double_limits_subtitle_channels());

	vino.addChooseButton({
		.id = u"vino/channelBottomButton"_q,
		.altIds = { u"vino/bottomButton"_q },
		.title = tr::vino_ChannelBottomButton(),
		.boxTitle = tr::vino_ChannelBottomButton(),
		.initialSelection = static_cast<int>(settings->channelBottomButton()),
		.options = {
			tr::vino_ChannelBottomButtonHide(tr::now),
			tr::vino_ChannelBottomButtonMute(tr::now),
			tr::vino_ChannelBottomButtonDiscuss(tr::now),
		},
		.setter = [](int index) {
			VinoSettings::getInstance().setChannelBottomButton(
				static_cast<ChannelBottomButton>(index));
		},
	});

	vino.addSettingToggle({
		.id = u"vino/quickAdminShortcuts"_q,
		.title = tr::vino_QuickAdminShortcuts(),
		.getter = &VinoSettings::quickAdminShortcuts,
		.setter = &VinoSettings::setQuickAdminShortcuts,
	});
	vino.addSettingToggle({
		.id = u"vino/disableGreetingSticker"_q,
		.title = tr::vino_DisableGreetingSticker(),
		.getter = &VinoSettings::disableGreetingSticker,
		.setter = &VinoSettings::setDisableGreetingSticker,
	});
	vino.addSettingToggle({
		.id = u"vino/showMessageShot"_q,
		.title = tr::vino_SettingsShowMessageShot(),
		.getter = &VinoSettings::showMessageShot,
		.setter = &VinoSettings::setShowMessageShot,
	});

	builder.addSkip();
	builder.addDividerText(tr::vino_SettingsShowMessageShotDescription());
	builder.addSkip();
}

void BuildMarks(
		SectionBuilder &builder,
		VinoSectionBuilder &vino,
		std::shared_ptr<PreviewState> previewState) {
	auto *settings = &VinoSettings::getInstance();
	const auto controller = builder.controller();

	builder.addSubsectionTitle(tr::lng_settings_messages());

	builder.add([=](const WidgetContext &ctx) -> SectionBuilder::WidgetToAdd {
		auto preview = object_ptr<MessagePreview>(ctx.container, controller);
		previewState->widget = preview.data();
		return {
			.widget = std::move(preview),
			.margin = style::margins(
				0,
				st::defaultVerticalListSkip,
				0,
				st::settingsPrivacySkipTop),
		};
	});

	vino.addSettingToggle({
		.id = u"vino/replaceBottomInfoWithIcons"_q,
		.altIds = { u"vino/replaceEditedWithIcon"_q },
		.title = tr::vino_ReplaceMarksWithIcons(),
		.getter = &VinoSettings::replaceBottomInfoWithIcons,
		.setter = &VinoSettings::setReplaceBottomInfoWithIcons,
	});

	builder.scope([&] {
		builder.addButton({
			.id = u"vino/deletedMark"_q,
			.title = tr::vino_DeletedMarkText(),
			.st = &st::settingsButtonNoIcon,
			.label = VinoSettings::getInstance().deletedMarkValue(),
			.onClick = [=] {
				auto box = Box<EditMarkBox>(
					tr::vino_DeletedMarkText(),
					settings->deletedMark(),
					QString("🧹"),
					[=](const QString &value) {
						VinoSettings::getInstance().setDeletedMark(value);
					});
				Ui::show(std::move(box));
			},
		});

		builder.addButton({
			.id = u"vino/editedMark"_q,
			.title = tr::vino_EditedMarkText(),
			.st = &st::settingsButtonNoIcon,
			.label = VinoSettings::getInstance().editedMarkValue(),
			.onClick = [=] {
				auto box = Box<EditMarkBox>(
					tr::vino_EditedMarkText(),
					settings->editedMark(),
					tr::lng_edited(tr::now),
					[=](const QString &value) {
						VinoSettings::getInstance().setEditedMark(value);
					});
				Ui::show(std::move(box));
			},
		});
	}, VinoSettings::getInstance().replaceBottomInfoWithIconsValue()
		| rpl::map([](bool v) { return !v; }));

	vino.addSettingToggle({
		.id = u"vino/removeMessageTail"_q,
		.title = tr::vino_RemoveMessageTail(),
		.getter = &VinoSettings::removeMessageTail,
		.setter = &VinoSettings::setRemoveMessageTail,
	});

	vino.addSettingToggle({
		.id = u"vino/hideFastShare"_q,
		.altIds = { u"vino/hideShareButton"_q },
		.title = tr::vino_HideShareButton(),
		.getter = &VinoSettings::hideFastShare,
		.setter = &VinoSettings::setHideFastShare,
	});
	vino.addSettingToggle({
		.id = u"vino/simpleQuotesAndReplies"_q,
		.altIds = { u"vino/disableColorfulReplies"_q, u"vino/replyElements"_q },
		.title = tr::vino_SimpleQuotesAndReplies(),
		.getter = &VinoSettings::simpleQuotesAndReplies,
		.setter = &VinoSettings::setSimpleQuotesAndReplies,
	});

	const auto semiTransparent = vino.addSettingToggle({
		.id = u"vino/semiTransparentDeletedMessages"_q,
		.altIds = { u"vino/translucentDeletedMessages"_q },
		.title = tr::vino_SemiTransparentDeletedMessages(),
		.getter = &VinoSettings::semiTransparentDeletedMessages,
		.setter = &VinoSettings::setSemiTransparentDeletedMessages,
	});
	if (semiTransparent) {
		vino.addBetaBadge(semiTransparent);
	}

	vino.addSectionDivider();
}

void BuildWideMessagesMultiplier(
		SectionBuilder &builder,
		VinoSectionBuilder &vino,
		std::shared_ptr<PreviewState> previewState) {
	auto *settings = &VinoSettings::getInstance();

	constexpr auto kMinSize = 1.00;
	constexpr auto kStep = 0.05;

	const auto valueToIndex = [=](double value) {
		return static_cast<int>(std::round((value - kMinSize) / kStep));
	};

	const auto controller = builder.controller();
	vino.addSlider({
		.id = u"vino/messageBubbleRadius"_q,
		.title = tr::vino_MessageBubbleRadius(),
		.steps = 17,
		.current = settings->messageBubbleRadius(),
		.indexToValue = [](int index) { return index; },
		.onChanged = [=](int index) {
			if (previewState->widget) {
				previewState->widget->setBubbleRadius(index);
			}
		},
		.onFinalChanged = [=](int index) {
			if (previewState->widget) {
				previewState->widget->setBubbleRadius(index);
			}
			VinoSettings::getInstance().setMessageBubbleRadius(index);
			ShowRestartPrompt(controller);
		},
		.formatLabel = [](int index) {
			return QString::number(index);
		},
	});

	vino.addSectionDivider();

	vino.addSlider({
		.id = u"vino/wideMultiplier"_q,
		.title = tr::vino_SettingsWideMultiplier(),
		.steps = 61, // (4.00 - 1.00) / 0.05 + 1
		.current = valueToIndex(settings->wideMultiplier()),
		.indexToValue = [](int index) { return index; },
		.onChanged = nullptr,
		.onFinalChanged = [=](int index) {
			VinoSettings::getInstance().setWideMultiplier(
				kMinSize + index * kStep);
			ShowRestartPrompt(controller);
		},
		.formatLabel = [=](int index) {
			return QString::number(kMinSize + index * kStep, 'f', 2);
		},
	});

	builder.addSkip();
	builder.addDividerText(tr::vino_SettingsWideMultiplierDescription());
	builder.addSkip();
}

void BuildContextMenuElements(SectionBuilder &builder, VinoSectionBuilder &vino) {
	auto *settings = &VinoSettings::getInstance();

	builder.addSubsectionTitle(tr::vino_ContextMenuElementsHeader());

	const auto options = std::vector{
		tr::vino_SettingsContextMenuItemHidden(tr::now),
		tr::vino_SettingsContextMenuItemShown(tr::now),
		tr::vino_SettingsContextMenuItemExtended(tr::now),
	};

	vino.addChooseButton({
		.id = u"vino/showReactionsPanelInContextMenu"_q,
		.title = tr::vino_SettingsContextMenuReactionsPanel(),
		.boxTitle = tr::vino_SettingsContextMenuTitle(),
		.initialSelection = static_cast<int>(settings->showReactionsPanelInContextMenu()),
		.options = options,
		.setter = [](int i) { VinoSettings::getInstance().setShowReactionsPanelInContextMenu(static_cast<ContextMenuVisibility>(i)); },
		.icon = { &st::menuIconReactions },
	});
	vino.addChooseButton({
		.id = u"vino/showViewsPanelInContextMenu"_q,
		.title = tr::vino_SettingsContextMenuViewsPanel(),
		.boxTitle = tr::vino_SettingsContextMenuTitle(),
		.initialSelection = static_cast<int>(settings->showViewsPanelInContextMenu()),
		.options = options,
		.setter = [](int i) { VinoSettings::getInstance().setShowViewsPanelInContextMenu(static_cast<ContextMenuVisibility>(i)); },
		.icon = { &st::menuIconShowInChat },
	});
	vino.addChooseButton({
		.id = u"vino/showHideMessageInContextMenu"_q,
		.title = tr::vino_ContextHideMessage(),
		.boxTitle = tr::vino_SettingsContextMenuTitle(),
		.initialSelection = static_cast<int>(settings->showHideMessageInContextMenu()),
		.options = options,
		.setter = [](int i) { VinoSettings::getInstance().setShowHideMessageInContextMenu(static_cast<ContextMenuVisibility>(i)); },
		.icon = { &st::menuIconClear },
	});
	vino.addChooseButton({
		.id = u"vino/showUserMessagesInContextMenu"_q,
		.title = tr::vino_UserMessagesMenuText(),
		.boxTitle = tr::vino_SettingsContextMenuTitle(),
		.initialSelection = static_cast<int>(settings->showUserMessagesInContextMenu()),
		.options = options,
		.setter = [](int i) { VinoSettings::getInstance().setShowUserMessagesInContextMenu(static_cast<ContextMenuVisibility>(i)); },
		.icon = { &st::menuIconTTL },
	});
	vino.addChooseButton({
		.id = u"vino/showMessageDetailsInContextMenu"_q,
		.title = tr::vino_MessageDetailsPC(),
		.boxTitle = tr::vino_SettingsContextMenuTitle(),
		.initialSelection = static_cast<int>(settings->showMessageDetailsInContextMenu()),
		.options = options,
		.setter = [](int i) { VinoSettings::getInstance().setShowMessageDetailsInContextMenu(static_cast<ContextMenuVisibility>(i)); },
		.icon = { &st::menuIconInfo },
	});
	vino.addChooseButton({
		.id = u"vino/showRepeatMessageInContextMenu"_q,
		.title = tr::vino_RepeatMessage(),
		.boxTitle = tr::vino_SettingsContextMenuTitle(),
		.initialSelection = static_cast<int>(settings->showRepeatMessageInContextMenu()),
		.options = options,
		.setter = [](int i) { VinoSettings::getInstance().setShowRepeatMessageInContextMenu(static_cast<ContextMenuVisibility>(i)); },
		.icon = { &st::vinoRepeatMenuIcon },
	});
	if (settings->filtersEnabled()) {
		vino.addChooseButton({
			.id = u"vino/showAddFilterInContextMenu"_q,
			.title = tr::vino_RegexFilterQuickAdd(),
			.boxTitle = tr::vino_SettingsContextMenuTitle(),
			.initialSelection = static_cast<int>(settings->showAddFilterInContextMenu()),
			.options = options,
			.setter = [](int i) { VinoSettings::getInstance().setShowAddFilterInContextMenu(static_cast<ContextMenuVisibility>(i)); },
			.icon = { &st::menuIconAddToFolder },
		});
	}

	builder.addSkip();
	builder.addDividerText(tr::vino_SettingsContextMenuDescription());
	builder.addSkip();
}

void BuildMessageFieldElements(SectionBuilder &builder, VinoSectionBuilder &vino) {
	builder.addSubsectionTitle(tr::vino_MessageFieldElementsHeader());

	vino.addSettingToggle({
		.id = u"vino/showAttachButtonInMessageField"_q,
		.title = tr::vino_MessageFieldElementAttach(),
		.getter = &VinoSettings::showAttachButtonInMessageField,
		.setter = &VinoSettings::setShowAttachButtonInMessageField,
		.icon = { &st::messageFieldAttachIcon },
	});
	vino.addSettingToggle({
		.id = u"vino/showCommandsButtonInMessageField"_q,
		.title = tr::vino_MessageFieldElementCommands(),
		.getter = &VinoSettings::showCommandsButtonInMessageField,
		.setter = &VinoSettings::setShowCommandsButtonInMessageField,
		.icon = { &st::messageFieldCommandsIcon },
	});
	vino.addSettingToggle({
		.id = u"vino/showAutoDeleteButtonInMessageField"_q,
		.title = tr::vino_MessageFieldElementTTL(),
		.getter = &VinoSettings::showAutoDeleteButtonInMessageField,
		.setter = &VinoSettings::setShowAutoDeleteButtonInMessageField,
		.icon = { &st::messageFieldTTLIcon },
	});
	vino.addSettingToggle({
		.id = u"vino/showEmojiButtonInMessageField"_q,
		.title = tr::vino_MessageFieldElementEmoji(),
		.getter = &VinoSettings::showEmojiButtonInMessageField,
		.setter = &VinoSettings::setShowEmojiButtonInMessageField,
		.icon = { &st::messageFieldEmojiIcon },
	});
	vino.addSettingToggle({
		.id = u"vino/showMicrophoneButtonInMessageField"_q,
		.title = tr::vino_MessageFieldElementVoice(),
		.getter = &VinoSettings::showMicrophoneButtonInMessageField,
		.setter = &VinoSettings::setShowMicrophoneButtonInMessageField,
		.icon = { &st::messageFieldVoiceIcon },
	});
	vino.addSettingToggle({
		.id = u"vino/showGiftButtonInMessageField"_q,
		.title = tr::lng_profile_action_short_gift(),
		.getter = &VinoSettings::showGiftButtonInMessageField,
		.setter = &VinoSettings::setShowGiftButtonInMessageField,
		.icon = { &st::settingsButtonIconGift },
	});
	vino.addSettingToggle({
		.id = u"vino/showAiEditorButtonInMessageField"_q,
		.title = tr::lng_ai_compose_title(),
		.getter = &VinoSettings::showAiEditorButtonInMessageField,
		.setter = &VinoSettings::setShowAiEditorButtonInMessageField,
		.icon = { &st::messageFieldCocoonAiIcon },
	});

	vino.addSectionDivider();
}

void BuildMessageFieldPopups(SectionBuilder &builder, VinoSectionBuilder &vino) {
	builder.addSubsectionTitle(tr::vino_MessageFieldPopupsHeader());

	vino.addSettingToggle({
		.id = u"vino/showAttachPopup"_q,
		.title = tr::vino_MessageFieldElementAttach(),
		.getter = &VinoSettings::showAttachPopup,
		.setter = &VinoSettings::setShowAttachPopup,
		.icon = { &st::messageFieldAttachIcon },
	});
	vino.addSettingToggle({
		.id = u"vino/showEmojiPopup"_q,
		.title = tr::vino_MessageFieldElementEmoji(),
		.getter = &VinoSettings::showEmojiPopup,
		.setter = &VinoSettings::setShowEmojiPopup,
		.icon = { &st::messageFieldEmojiIcon },
	});
}

const auto kMeta = BuildHelper({
	.id = VinoChats::Id(),
	.parentId = VinoMain::Id(),
	.title = &tr::vino_CategoryChats,
	.icon = &st::menuIconChatBubble,
}, [](SectionBuilder &builder) {
	auto vino = VinoSectionBuilder(builder);
	const auto previewState = std::make_shared<PreviewState>();

	builder.addSkip();
	BuildStickersAndEmoji(builder, vino);
	BuildGroupsAndChannels(builder, vino);
	BuildMarks(builder, vino, previewState);
	BuildWideMessagesMultiplier(builder, vino, previewState);
	BuildContextMenuElements(builder, vino);
	BuildMessageFieldElements(builder, vino);
	BuildMessageFieldPopups(builder, vino);
	builder.addSkip();
});

} // namespace

rpl::producer<QString> VinoChats::title() {
	return tr::vino_CategoryChats();
}

VinoChats::VinoChats(
	QWidget *parent,
	not_null<Window::SessionController*> controller)
: Section(parent, controller) {
	setupContent();
}

void VinoChats::setupContent() {
	const auto content = Ui::CreateChild<Ui::VerticalLayout>(this);
	build(content, kMeta.build);
	Ui::ResizeFitChild(this, content);
}

Type VinoChatsId() {
	return VinoChats::Id();
}

} // namespace Settings
