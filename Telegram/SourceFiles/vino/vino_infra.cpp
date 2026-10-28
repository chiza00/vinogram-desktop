// This is the source code of VinoGram for Desktop.
//
// We do not and cannot prevent the use of our code,
// but be respectful and credit the original author.
//
// Copyright @Radolyn, 2026
#include "vino/vino_infra.h"

#include "vino/vino_lang.h"
#include "vino/vino_settings.h"
#include "vino/vino_ui_settings.h"
#include "vino/vino_worker.h"
#include "vino/data/vino_database.h"
#include "vino/ui/vino_logo.h"
#include "features/translator/vino_translator.h"
#include "lang/lang_instance.h"
#include "ui/chat/chat_style_radius.h"
#include "utils/rc_manager.h"

#ifdef Q_OS_WIN
#include "vino/utils/windows_utils.h"
#endif

namespace VinoInfra {

void initLang() {
	QString id = Lang::GetInstance().id();
	QString baseId = Lang::GetInstance().baseId();
	if (id.isEmpty()) {
		LOG(("Language is not loaded"));
		return;
	}
	VinoLanguage::init();
	VinoLanguage::currentInstance()->fetchLanguage(id, baseId);
}

void initUiSettings() {
	const auto &settings = VinoSettings::getInstance();

	VinoUiSettings::setMonoFont(settings.monoFont());
	VinoUiSettings::setWideMultiplier(settings.wideMultiplier());
	VinoUiSettings::setMaterialSwitches(settings.materialSwitches());
	VinoUiSettings::setAvatarCorners(settings.avatarCorners());
	Ui::SetAppliedBubbleRadius(settings.messageBubbleRadius());
}

void initDatabase() {
	VinoDatabase::initialize();
}

void initWorker() {
	VinoWorker::initialize();
}

void initRCManager() {
	RCManager::getInstance().start();
}

void initTranslator() {
	Vino::Translator::TranslateManager::init();
}

void initIcon() {
#ifdef Q_OS_WIN
	VinoAssets::loadAppIco();
	reloadAppIconFromTaskBar();
#endif
}

void init() {
	initLang();
	initDatabase();
	initUiSettings();
	initIcon();
	initWorker();
	initRCManager();
	initTranslator();
}

}
