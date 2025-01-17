// based off https://github.com/jostrander/mouse-forward-back

#include <nan.h>

#ifdef _WIN32
#include <windows.h>
#endif

#ifdef linux
#include <X11/Xlib.h>
#endif

using namespace v8;
using Nan::AsyncResource;

namespace MouseDetect {
    uv_loop_t *loop;
    uv_async_t async;
    // uv_work_t req;
    Nan::Callback(cb);

#ifdef _WIN32
    LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
        if (nCode == HC_ACTION) {
            if (wParam == WM_XBUTTONUP) {
              MSLLHOOKSTRUCT *pMouseStruct = (MSLLHOOKSTRUCT *)lParam;
              if (pMouseStruct != NULL) {
                if (HIWORD(pMouseStruct->mouseData) == XBUTTON1) {
                  async.data = (void *)8;
                  uv_async_send(&async);
                } else if (HIWORD(pMouseStruct->mouseData) == XBUTTON2) {
                  async.data = (void *)9;
                  uv_async_send(&async);
                }
              }
            }
        }

        return CallNextHookEx(NULL, nCode, wParam, lParam);
    }

    void hook() {
      HHOOK mouseHook = SetWindowsHookEx(WH_MOUSE_LL, LowLevelMouseProc, NULL, 0);

      if (!mouseHook) {
        fprintf(stderr, "Failed to install mouse hook!\n");
        return;
      }

      MSG msg;
      while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
      }

      UnhookWindowsHookEx(mouseHook);
    }
#endif // _WIN32

#ifdef linux

    int X_ErrorHandler(Display *d, XErrorEvent *e) {
        char msg[80];
        XGetErrorText(d, e->error_code, msg, sizeof(msg));

        fprintf(stderr, "Error %d (%s): request %d.%d\n", e->error_code, msg, e->request_code, e->minor_code);
        return 0;
    }

    void hook() {
        Display *display;
        XEvent xevent;

        XSetErrorHandler(X_ErrorHandler);

        if( (display = XOpenDisplay(NULL)) == NULL ) {
            return;
        }

        XAllowEvents(display, AsyncBoth, CurrentTime);

        XGrabButton(display, 8, AnyModifier, window, True,  ButtonPressMask | ButtonReleaseMask, GrabModeAsync, GrabModeAsync, None, None);
        XGrabButton(display, 9, AnyModifier, window, True,  ButtonPressMask | ButtonReleaseMask, GrabModeAsync, GrabModeAsync, None, None);

        // Loop for events.
        while (1) {
        XNextEvent(display, &xevent);
        switch (xevent.type) {
            case ButtonRelease:
                // Sends data back to node
                async.data = (void*) &xevent.xbutton.button;
                uv_async_send(&async);
                break;
        }
        }
    }
#endif // linux

    void events_callback(uv_async_t *handle) {
        Nan::HandleScope scope;
        uint buttonNumber = *((uint*) handle->data);

        Local<Value> arg = Nan::New(buttonNumber);

        cb.Call(1, &arg);
    }

    void OnFatalError(const char* errorMessage, const char* more) {
        printf("%s :: %s", errorMessage, more);
    }

    NAN_METHOD(GetEvents) {
        Isolate* isolate = info.GetIsolate();
        isolate->SetFatalErrorHandler(OnFatalError);

        // Callback
        cb.Reset(info[0].As<Function>());

#ifdef linux
        // Window handle
        Local<Value> value = info[1];
        if (node::Buffer::HasInstance(value)) {
            Window* data = reinterpret_cast<Window*>(node::Buffer::Data(value));
            window = *data;
        }
#endif // linux

        // Start thread
        loop = uv_default_loop();

        int param = 0;
        uv_thread_t t_id;
        uv_thread_cb uvcb = (uv_thread_cb) hook;

        uv_async_init(loop, &async, events_callback);
        uv_thread_create(&t_id, uvcb, &param);
    }

    NAN_MODULE_INIT(Init)
    {
        Nan::SetMethod(target, "register", GetEvents);
    }

#ifdef _WIN32
    NODE_MODULE(mouseDetectWIN, Init)
#endif // _WIN32

#ifdef linux
    NODE_MODULE(mouseDetectLINUX, Init)
#endif // linux

}
