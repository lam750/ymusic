# Шаг 1 — «Switch → Yandex Music API»

1. Установи devkitPro + switch-dev: `sudo dkp-pacman -S switch-dev switch-curl switch-mbedtls switch-zlib`
2. Скопируй шаблон: `cp -r /opt/devkitpro/examples/switch/templates/application/* ymusic-switch/`
   (не затирая `source/main.c` из этого архива)
3. В `Makefile` замени строку LIBS на:
   `LIBS := -lcurl -lmbedtls -lmbedx509 -lmbedcrypto -lz -lnx`
4. `make` -> получится `.nro`
5. Положи OAuth-токен в `sdmc:/config/ymusic/token.txt`, запусти .nro через hbmenu (в режиме applet-override, т.е. через альбом, не через игру — иначе мало RAM)
6. Ожидаемый результат: `HTTP 200` и JSON со статусом аккаунта.

## Сборка через GitHub Actions (без установки devkitPro)
1. Создай репозиторий на GitHub, залей СОДЕРЖИМОЕ этой папки в корень (вместе с `.github/`).
2. Вкладка Actions -> build-nro -> дождись зелёной галочки.
3. Внизу страницы запуска в Artifacts скачай `ymusic-switch-nro` -> внутри `.nro`.
4. Если упало — открой лог шага Build и скинь ошибку мне.
