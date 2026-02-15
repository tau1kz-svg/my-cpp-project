extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
}

#include <iostream>
#include <windows.h>
#include <string>

extern "C" {
#include <libavformat/avformat.h>
}

void CheckVideo(const char* filename) {
    AVFormatContext* pFormatCtx = avformat_alloc_context();
    
    // Пытаемся открыть файл
    if (avformat_open_input(&pFormatCtx, filename, NULL, NULL) == 0) {
        // Читаем данные о потоках (видео, аудио)
        if (avformat_find_stream_info(pFormatCtx, NULL) >= 0) {
            std::string info = "Файл открыт успешно!\nДлительность: ";
            info += std::to_string(pFormatCtx->duration / 1000000);
            info += " сек.";
            MessageBox(NULL, info.c_str(), "VS Systems Video Player", MB_OK);
        }
        avformat_close_input(&pFormatCtx);
    } else {
        MessageBox(NULL, "Не удалось найти или открыть файл test.mp4", "Ошибка", MB_ICONERROR);
    }
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmd, int nShow) {
    // Вызываем проверку перед созданием окна
    CheckVideo("test.mp4");

    // Твой текущий код создания окна (CreateWindowEx...)
    // ...
    return 0;
}