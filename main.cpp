#include <windows.h>
#include <string>

extern "C" {
#include <libavformat/avformat.h>
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmd, int nShow) {
    // Инициализация FFmpeg
    avformat_network_init();
    
    AVFormatContext* pFormatCtx = nullptr;
    const char* filename = "test.mp4"; // Файл должен лежать рядом с exe

    // Пытаемся открыть файл
    if (avformat_open_input(&pFormatCtx, filename, NULL, NULL) == 0) {
        if (avformat_find_stream_info(pFormatCtx, NULL) >= 0) {
            int duration = (int)(pFormatCtx->duration / 1000000);
            std::string msg = "Успех! Видео найдено. Длительность: " + std::to_string(duration) + " сек.";
            MessageBox(NULL, msg.c_str(), "VS Systems", MB_OK);
        }
        avformat_close_input(&pFormatCtx);
    } else {
        MessageBox(NULL, "Файл test.mp4 не найден в папке с программой!", "Ошибка", MB_ICONERROR);
    }

    return 0;
}