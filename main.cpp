#include <windows.h>
extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
}

// Глобальные объекты для работы
AVFormatContext* pFormatCtx = nullptr;
AVCodecContext* pCodecCtx = nullptr;
SwsContext* sws_ctx = nullptr;
AVFrame *pFrame = nullptr, *g_pFrameRGB = nullptr;
AVPacket* pkt = nullptr;
int vIdx = -1, g_width = 0, g_height = 0;
bool g_isPaused = false; 

// Функция для получения следующего кадра
bool GetNextFrame() {
    while (av_read_frame(pFormatCtx, pkt) >= 0) {
        if (pkt->stream_index == vIdx) {
            if (avcodec_send_packet(pCodecCtx, pkt) == 0) {
                if (avcodec_receive_frame(pCodecCtx, pFrame) == 0) {
                    sws_scale(sws_ctx, pFrame->data, pFrame->linesize, 0, g_height, 
                              g_pFrameRGB->data, g_pFrameRGB->linesize);
                    av_packet_unref(pkt);
                    return true;
                }
            }
        }
        av_packet_unref(pkt);
    }
    return false;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CREATE:
            SetTimer(hwnd, 1, 33, NULL); 
            return 0;

        case WM_KEYDOWN:
            if (wParam == VK_SPACE) { // Если нажат Пробел
                g_isPaused = !g_isPaused; // Переключаем состояние
                
                // Меняем заголовок окна для наглядности
                if (g_isPaused) SetWindowText(hwnd, "VS Systems Player [PAUSED]");
                else SetWindowText(hwnd, "VS Systems Player");
            }
            return 0;

        case WM_TIMER:
            if (!g_isPaused) { // Если не на паузе — тянем следующий кадр
                if (GetNextFrame()) {
                    InvalidateRect(hwnd, NULL, FALSE);
                } else {
                    KillTimer(hwnd, 1);
                }
            }
            return 0;
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            if (g_pFrameRGB) {
                BITMAPINFO bmi = {0};
                bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
                bmi.bmiHeader.biWidth = g_width;
                bmi.bmiHeader.biHeight = -g_height;
                bmi.bmiHeader.biPlanes = 1;
                bmi.bmiHeader.biBitCount = 32;
                bmi.bmiHeader.biCompression = BI_RGB;
                StretchDIBits(hdc, 0, 0, g_width, g_height, 0, 0, g_width, g_height, 
                              g_pFrameRGB->data[0], &bmi, DIB_RGB_COLORS, SRCCOPY);
            }
            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_DESTROY: PostQuitMessage(0); return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmd, int nShow) {
    // Весь твой код инициализации FFmpeg из прошлого шага остается здесь
    avformat_network_init();
    if (avformat_open_input(&pFormatCtx, "test.mp4", NULL, NULL) != 0) return 0;
    avformat_find_stream_info(pFormatCtx, NULL);

    for (int i = 0; i < pFormatCtx->nb_streams; i++) 
        if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) { vIdx = i; break; }

    const AVCodec* pCodec = avcodec_find_decoder(pFormatCtx->streams[vIdx]->codecpar->codec_id);
    pCodecCtx = avcodec_alloc_context3(pCodec);
    avcodec_parameters_to_context(pCodecCtx, pFormatCtx->streams[vIdx]->codecpar);
    avcodec_open2(pCodecCtx, pCodec, NULL);

    g_width = pCodecCtx->width; g_height = pCodecCtx->height;
    g_pFrameRGB = av_frame_alloc();
    int numBytes = av_image_get_buffer_size(AV_PIX_FMT_BGRA, g_width, g_height, 1);
    uint8_t* buffer = (uint8_t*)av_malloc(numBytes);
    av_image_fill_arrays(g_pFrameRGB->data, g_pFrameRGB->linesize, buffer, AV_PIX_FMT_BGRA, g_width, g_height, 1);
    sws_ctx = sws_getContext(g_width, g_height, pCodecCtx->pix_fmt, g_width, g_height, AV_PIX_FMT_BGRA, SWS_BILINEAR, NULL, NULL, NULL);

    pkt = av_packet_alloc();
    pFrame = av_frame_alloc();

    WNDCLASS wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInst;
    wc.lpszClassName = "VSVideoPlayer";
    RegisterClass(&wc);
    HWND hwnd = CreateWindow("VSVideoPlayer", "VS Systems Player", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 
                             CW_USEDEFAULT, CW_USEDEFAULT, g_width, g_height, NULL, NULL, hInst, NULL);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) { TranslateMessage(&msg); DispatchMessage(&msg); }
    return 0;
}