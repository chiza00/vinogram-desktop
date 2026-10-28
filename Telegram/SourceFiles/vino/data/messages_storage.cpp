// This is the source code of VinoGram for Desktop.
//
// We do not and cannot prevent the use of our code,
// but be respectful and credit the original author.
//
// Copyright @Radolyn, 2026
#include "vino/data/messages_storage.h"

#include "vino/data/vino_database.h"
#include "vino/utils/vino_mapper.h"
#include "vino/utils/telegram_helpers.h"
#include "base/unixtime.h"
#include "data/data_forum_topic.h"
#include "data/data_session.h"
#include "history/history.h"
#include "history/history_item.h"
#include "history/history_item_components.h"
#include "main/main_session.h"

namespace VinoMessages {

template<typename DerivedMessage>
std::vector<VinoMessageBase> convertToBase(const std::vector<DerivedMessage> &messages) {
	std::vector<VinoMessageBase> based;
	based.reserve(messages.size());
	for (const auto &msg : messages) {
		based.push_back(static_cast<VinoMessageBase>(msg));
	}
	return based;
}

void map(not_null<HistoryItem*> item, VinoMessageBase &message) {
	const ID userId = item->history()->owner().session().userId().bare & PeerId::kChatTypeMask;

	message.userId = userId;
	message.dialogId = getDialogIdFromPeer(item->history()->peer);
	message.groupedId = item->groupId().raw();
	message.peerId = item->history()->peer->id.value & PeerId::kChatTypeMask;
	message.fromId = item->from()->id.value & PeerId::kChatTypeMask;
	if (item->topic()) {
		message.topicId = item->topicRootId().bare;
	} else {
		message.topicId = 0;
	}
	message.messageId = item->id.bare;
	message.date = item->date();
	message.flags = VinoMapper::mapItemFlagsToMTPFlags(item);

	if (const auto edited = item->Get<HistoryMessageEdited>()) {
		message.editDate = edited->date;
	} else {
		message.editDate = base::unixtime::now();
	}

	message.views = item->viewsCount();
	message.fwdFlags = 0;
	message.fwdFromId = 0;
	// message.fwdName
	message.fwdDate = 0;
	// message.fwdPostAuthor
	if (const auto msgsigned = item->Get<HistoryMessageSigned>()) {
		message.postAuthor = msgsigned->author.toStdString();
	}
	message.replyFlags = 0;
	message.replyMessageId = 0;
	message.replyPeerId = 0;
	message.replyTopId = 0;
	message.replyForumTopic = false;
	// message.replySerialized
	// message.replyMarkupSerialized
	message.entityCreateDate = base::unixtime::now();

	auto serializedText = VinoMapper::serializeTextWithEntities(item);
	message.text = serializedText.first;
	message.textEntities = serializedText.second;

	// todo: implement mapping
	message.mediaPath = "/";
	// message.hqThumbPath
	message.documentType = 0; // document type none
	// message.documentSerialized
	// message.thumbsSerialized
	// message.documentAttributesSerialized
	// message.mimeType
}

void addEditedMessage(not_null<HistoryItem *> item) {
	EditedMessage message;
	map(item, message);

	if (message.text.empty()) {
		return;
	}

	VinoDatabase::addEditedMessage(message);
}

std::vector<VinoMessageBase> getEditedMessages(not_null<HistoryItem*> item, ID minId, ID maxId, int totalLimit) {
	const ID userId = item->history()->owner().session().userId().bare & PeerId::kChatTypeMask;
	const auto dialogId = getDialogIdFromPeer(item->history()->peer);
	const auto msgId = item->id.bare;

	return convertToBase(VinoDatabase::getEditedMessages(userId, dialogId, msgId, minId, maxId, totalLimit));
}

bool hasRevisions(not_null<HistoryItem*> item) {
	const ID userId = item->history()->owner().session().userId().bare & PeerId::kChatTypeMask;
	const auto dialogId = getDialogIdFromPeer(item->history()->peer);
	const auto msgId = item->id.bare;

	return VinoDatabase::hasRevisions(userId, dialogId, msgId);
}

void addDeletedMessage(not_null<HistoryItem*> item) {
	DeletedMessage message;
	map(item, message);

	if (message.text.empty()) {
		return;
	}

	VinoDatabase::addDeletedMessage(message);
}

std::vector<VinoMessageBase>
getDeletedMessages(not_null<PeerData*> peer, ID topicId, ID minId, ID maxId, int totalLimit, const QString &searchQuery) {
	const ID userId = peer->session().userId().bare & PeerId::kChatTypeMask;
	return convertToBase(
		VinoDatabase::getDeletedMessages(userId, getDialogIdFromPeer(peer), topicId, minId, maxId, totalLimit, searchQuery.toStdString()));
}

bool hasDeletedMessages(not_null<PeerData*> peer, ID topicId) {
	const ID userId = peer->session().userId().bare & PeerId::kChatTypeMask;
	return VinoDatabase::hasDeletedMessages(userId, getDialogIdFromPeer(peer), topicId);
}

void removeDeletedMessage(not_null<HistoryItem*> item) {
	const auto peer = item->history()->peer;
	const ID userId = peer->session().userId().bare & PeerId::kChatTypeMask;
	VinoDatabase::removeDeletedMessage(userId, getDialogIdFromPeer(peer), item->id.bare);
}

void clearDeletedMessages(not_null<PeerData*> peer, ID topicId) {
	const ID userId = peer->session().userId().bare & PeerId::kChatTypeMask;
	VinoDatabase::clearDeletedMessages(userId, getDialogIdFromPeer(peer), topicId);
}

}
