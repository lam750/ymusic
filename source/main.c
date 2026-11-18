// Step 1: проверка, что Switch ходит в Яндекс Музыку напрямую по HTTPS с OAuth-токеном.
// Токен: sdmc:/config/ymusic/token.txt (одна строка)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <switch.h>
#include <curl/curl.h>

typedef struct { char *data; size_t len; } Buf;

static size_t on_write(void *p, size_t sz, size_t n, void *ud) {
    Buf *b = ud; size_t add = sz * n;
    char *t = realloc(b->data, b->len + add + 1);
    if (!t) return 0;
    b->data = t; memcpy(b->data + b->len, p, add);
    b->len += add; b->data[b->len] = 0;
    return add;
}

static int read_token(char *out, size_t cap) {
    FILE *f = fopen("sdmc:/config/ymusic/token.txt", "r");
    if (!f) return 0;
    if (!fgets(out, cap, f)) { fclose(f); return 0; }
    fclose(f);
    out[strcspn(out, "\r\n ")] = 0;
    return out[0] != 0;
}

int main(void) {
    consoleInit(NULL);
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);
    PadState pad; padInitializeDefault(&pad);

    Result rc = socketInitializeDefault();
    printf("Yandex Music Switch - step 1\n\n");
    if (R_FAILED(rc)) printf("socketInit failed: 0x%x\n", rc);

    char token[256] = {0};
    if (!read_token(token, sizeof token)) {
        printf("Нет токена: положи его в sdmc:/config/ymusic/token.txt\n");
    } else {
        curl_global_init(CURL_GLOBAL_DEFAULT);
        CURL *c = curl_easy_init();
        Buf b = {0};
        char auth[320];
        snprintf(auth, sizeof auth, "Authorization: OAuth %s", token);
        struct curl_slist *h = NULL;
        h = curl_slist_append(h, auth);
        h = curl_slist_append(h, "X-Yandex-Music-Client: YandexMusicAndroid/24023231");
        curl_easy_setopt(c, CURLOPT_URL, "https://api.music.yandex.net/account/status");
        curl_easy_setopt(c, CURLOPT_HTTPHEADER, h);
        curl_easy_setopt(c, CURLOPT_WRITEFUNCTION, on_write);
        curl_easy_setopt(c, CURLOPT_WRITEDATA, &b);
        curl_easy_setopt(c, CURLOPT_USERAGENT, "Yandex-Music-API");
        // TODO: на шаге 2 положить cacert.pem в romfs и включить проверку
        curl_easy_setopt(c, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(c, CURLOPT_SSL_VERIFYHOST, 0L);

        CURLcode r = curl_easy_perform(c);
        long code = 0; curl_easy_getinfo(c, CURLINFO_RESPONSE_CODE, &code);
        printf("curl: %s, HTTP %ld\n\n", curl_easy_strerror(r), code);
        if (b.data) printf("%.900s\n", b.data);
        free(b.data);
        curl_slist_free_all(h);
        curl_easy_cleanup(c);
        curl_global_cleanup();
    }

    printf("\n+ для выхода\n");
    consoleUpdate(NULL);
    while (appletMainLoop()) {
        padUpdate(&pad);
        if (padGetButtonsDown(&pad) & HidNpadButton_Plus) break;
        consoleUpdate(NULL);
    }
    socketExit();
    consoleExit(NULL);
    return 0;
}
