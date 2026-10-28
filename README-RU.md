# VinoGram

![VinoGram Лого](.github/VinoGram.png)

[ [English](README.md)  | Русский ]

## Функции и Фишки

- Полный режим призрака (настраиваемый)
- История удалений и изменений сообщений
- Кастомизация шрифта
- Режим Стримера
- Локальный телеграм премиум
- Переводчик
- Превью медиа и быстрая реакция при сильном нажатии на тачпад (macOS)
- Улучшенный вид

И многое другое. Посмотрите нашу [Документацию](https://docs.vinogram.one/desktop/) для более подробной информации.

<h3>
  <details>
    <summary>Превью</summary>
    <table>
      <tr>
        <td><img src='.github/demos/demo1.png' width='268' alt='Preferences'></td>
        <td><img src='.github/demos/demo2.png' width='268' alt='VinoGram Options'></td>
        <td><img src='.github/demos/demo3.png' width='268' alt='Message Filters'></td>
      </tr>
      <tr>
        <td><img src='.github/demos/demo4.png' width='268' alt='Appearance'></td>
        <td><img src='.github/demos/demo5.png' width='268' alt='Chats'></td>
      </tr>
    </table>
  </details>
</h3>

## Установка

### Windows

#### Официальная версия

Вы можете скачать готовый бинарный файл со вкладки [Releases](https://github.com/VinoGram/VinoGramDesktop/releases) или из
[Телеграм канала](https://t.me/VinoGramReleases).

#### Winget

```bash
winget install VinoGram.VinoGramDesktop
```

#### Scoop

```bash
scoop bucket add extras
scoop install vinogram
```

#### Сборка вручную

Следуйте [официальному руководству](https://github.com/VinoGram/VinoGramDesktop/blob/dev/docs/building-win-x64.md), если
вы хотите собрать VinoGram сами.

### macOS

#### Официальная версия

Вы можете скачать подписанный пакет со вкладки [Releases](https://github.com/VinoGram/VinoGramDesktop/releases).

#### Homebrew

```bash
brew install --cask vinogram
```

### Arch Linux

#### Из исходников (рекомендованный способ)

Установите `vinogram-desktop` из [AUR](https://aur.archlinux.org/packages/vinogram-desktop).

#### Готовые бинарники

Установите `vinogram-desktop-bin` из [AUR](https://aur.archlinux.org/packages/vinogram-desktop-bin).

Примечание: данный пакет собирается не нами.

### NixOS

#### Флейк (рекомендуется)

Установите `vinogram-desktop` из [ndfined-crp/vinogram-desktop](https://github.com/ndfined-crp/vinogram-desktop)

#### Nixpkgs

Установите `vinogram-desktop` из [nixpkgs](https://search.nixos.org/packages?channel=unstable&show=vinogram-desktop)

### ALT Linux

[Sisyphus](https://packages.altlinux.org/en/sisyphus/srpms/vinogram-desktop/)

### Gentoo Linux

Инструкцию по установке можно найти в [этом репозитории](https://codeberg.org/OverLessArtem/vinogram-ebuild-gentoo).

### Void Linux
Инструкцию по установке можно найти в [этом репозитории](https://codeberg.org/OverLessArtem/vinogram-template-void)

### EPM

`epm play vinogram`

### Fedora

Из репозитория [RPM Fusion](https://admin.rpmfusion.org/pkgdb/package/free/vinogram-desktop/).

```bash
dnf install vinogram-desktop
```

### Любой другой Линукс дистрибутив

Flatpak: https://github.com/0FL01/VinoGramDesktop-flatpak

Или следуйте [официальному руководству](https://github.com/VinoGram/VinoGramDesktop/blob/dev/docs/building-linux.md).

## Пожертвования

Вам нравится использовать **VinoGram**? Оставьте нам чаевые!

[Здесь доступные варианты.](https://docs.vinogram.one/donate/)

## Использованные материалы

### Телеграм клиенты

- [Telegram Desktop](https://github.com/telegramdesktop/tdesktop)
- [Kotatogram](https://github.com/kotatogram/kotatogram-desktop)
- [64Gram](https://github.com/TDesktop-x64/tdesktop)
- [Forkgram](https://github.com/forkgram/tdesktop)

### Использованные библиотеки

- [JSON for Modern C++](https://github.com/nlohmann/json)
- [SQLite](https://github.com/sqlite/sqlite)
- [sqlite_orm](https://github.com/fnc12/sqlite_orm)

### Иконки

- [Solar Icon Set](https://www.figma.com/community/file/1166831539721848736)

### Боты

- [TelegramDB](https://t.me/tgdatabase) для получения юзернейма по ID (до закрытия бесплатной версии 2 апреля 2026)
