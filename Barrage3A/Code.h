/*
    Code.h
*/
// マウス MouseCode enum class
enum class MouseCode : int {
    Left = MOUSE_INPUT_LEFT,        // 左ボタン.
    Right = MOUSE_INPUT_RIGHT,      // 右ボタン.
    Middle = MOUSE_INPUT_MIDDLE,    // 中央ボタン（ホイールクリック）.
    XButton1 = MOUSE_INPUT_1,       // 拡張ボタン1.
    XButton2 = MOUSE_INPUT_2,       // 拡張ボタン2.
};
// キーボード KeyCode enum class
enum class KeyCode : int {
    // キー 数字.
    NUM_0 = KEY_INPUT_0,    // 0 Key.
    NUM_1 = KEY_INPUT_1,    // 1 Key.
    NUM_2 = KEY_INPUT_2,    // 2 Key.
    NUM_3 = KEY_INPUT_3,    // 3 Key.
    NUM_4 = KEY_INPUT_4,    // 4 Key.
    NUM_5 = KEY_INPUT_5,    // 5 Key.
    NUM_6 = KEY_INPUT_6,    // 6 Key.
    NUM_7 = KEY_INPUT_7,    // 7 Key.
    NUM_8 = KEY_INPUT_8,    // 8 Key.
    NUM_9 = KEY_INPUT_9,    // 9 Key

    // テンキー数字.
    PAD_0 = KEY_INPUT_NUMPAD0, // Keypad 0.
    PAD_1 = KEY_INPUT_NUMPAD1, // Keypad 1.
    PAD_2 = KEY_INPUT_NUMPAD2, // Keypad 2.
    PAD_3 = KEY_INPUT_NUMPAD3, // Keypad 3.
    PAD_4 = KEY_INPUT_NUMPAD4, // Keypad 4.
    PAD_5 = KEY_INPUT_NUMPAD5, // Keypad 5.
    PAD_6 = KEY_INPUT_NUMPAD6, // Keypad 6.
    PAD_7 = KEY_INPUT_NUMPAD7, // Keypad 7.
    PAD_8 = KEY_INPUT_NUMPAD8, // Keypad 8.
    PAD_9 = KEY_INPUT_NUMPAD9, // Keypad 9.

    // アルファベットキー.
    A = KEY_INPUT_A,        // A Key.
    B = KEY_INPUT_B,        // B Key.
    C = KEY_INPUT_C,        // C Key.
    D = KEY_INPUT_D,        // D Key.
    E = KEY_INPUT_E,        // E Key.
    F = KEY_INPUT_F,        // F Key.
    G = KEY_INPUT_G,        // G Key.
    H = KEY_INPUT_H,        // H Key.
    I = KEY_INPUT_I,        // I Key.
    J = KEY_INPUT_J,        // J Key.
    K = KEY_INPUT_K,        // K Key.
    L = KEY_INPUT_L,        // L Key.
    M = KEY_INPUT_M,        // M Key.
    N = KEY_INPUT_N,        // N Key.
    O = KEY_INPUT_O,        // O Key.
    P = KEY_INPUT_P,        // P Key.
    Q = KEY_INPUT_Q,        // Q Key.
    R = KEY_INPUT_R,        // R Key.
    S = KEY_INPUT_S,        // S Key.
    T = KEY_INPUT_T,        // T Key.
    U = KEY_INPUT_U,        // U Key.
    V = KEY_INPUT_V,        // V Key.
    W = KEY_INPUT_W,        // W Key.
    X = KEY_INPUT_X,        // S Key.
    Y = KEY_INPUT_Y,        // Y Key.
    Z = KEY_INPUT_Z,        // Z Key.
    
    // 矢印キー.
    LeftArrow = KEY_INPUT_LEFT,     // LeftArrow  ←.
    RightArrow = KEY_INPUT_RIGHT,   // RightArrow →.
    UpArrow = KEY_INPUT_UP,         // UpArrow    ↑.
    DownArrow = KEY_INPUT_DOWN,     // DownArrow  ↓.

    // 機能キー.
    F1 = KEY_INPUT_F1,  // F1 key.
    F2 = KEY_INPUT_F2,  // F2 key.
    F3 = KEY_INPUT_F3,  // F3 key.
    F4 = KEY_INPUT_F4,  // F4 key.
    F5 = KEY_INPUT_F5,  // F5 key.
    F6 = KEY_INPUT_F6,  // F6 key.
    F7 = KEY_INPUT_F7,  // F7 key.
    F8 = KEY_INPUT_F8,  // F8 key.
    F9 = KEY_INPUT_F9,  // F9 key.
    F10 = KEY_INPUT_F10,// F10 key.
    F11 = KEY_INPUT_F11,// F11 key.
    F12 = KEY_INPUT_F12,// F12 key.

    // 特殊キー.
    ESCAPE = KEY_INPUT_ESCAPE,          // ESCAPE(Esc) Key.
    SPACE = KEY_INPUT_SPACE,            // SPACE Key.
    ENTER = KEY_INPUT_RETURN,           // ENTER Key.
    BACKSPACE = KEY_INPUT_BACK,         // BackSpeace Key.
    TAB = KEY_INPUT_TAB,                // TAB Key.
    LeftShift = KEY_INPUT_LSHIFT,       // LeftShift Key.
    RshiftShift = KEY_INPUT_RSHIFT,     // LeftShift Key.
    LeftControl = KEY_INPUT_LCONTROL,   // LeftControl Key.
    RshiftControl = KEY_INPUT_RCONTROL, // RshiftControl Key.
    LeftALT = KEY_INPUT_LALT,           // LeftALT Key.
    RshiftALT = KEY_INPUT_RALT,         // RshiftALT key.

    // その他のキー.
    CAPSLOCK = KEY_INPUT_CAPSLOCK,      // CastLock Key.
    NUMLOCK = KEY_INPUT_NUMLOCK,        // NumLock  Key.
    SCROLLLOCK = KEY_INPUT_SCROLL,      // ScrollLock key.
    INSERT = KEY_INPUT_INSERT,          // Insert Key.
    HOME = KEY_INPUT_HOME,              // Home Key.
    END = KEY_INPUT_END,                // End Key.
    PAGEUP = KEY_INPUT_PGUP,            // PageUp Key.
    PAGEDOWN = KEY_INPUT_PGDN,          // PageDown Key.
    PAUSE = KEY_INPUT_PAUSE,            // PauseBreak Key.
    MINUS = KEY_INPUT_MINUS,            // Minus Key.
    YEN = KEY_INPUT_YEN,                // Yen Key.
    PREVTRACK = KEY_INPUT_PREVTRACK,    // PrevTrack Key.
    PERIOD = KEY_INPUT_PERIOD,          // Period Key.
    SLASH = KEY_INPUT_SLASH,            // Slash Key.
    SEMICOLON = KEY_INPUT_SEMICOLON,    // Semicolon Key.
    COLON = KEY_INPUT_COLON,            // Colon Key.
    LBRACKET = KEY_INPUT_LBRACKET,      // LeftBracket Key.
    RBRACKET = KEY_INPUT_RBRACKET,      // RightBracket Key.
    AT = KEY_INPUT_AT,                  // At Key.
    BACKSLASH = KEY_INPUT_BACKSLASH,    // Backslash Key.
    COMMA = KEY_INPUT_COMMA,            // Comma Key.
    KANJI = KEY_INPUT_KANJI,            // Kanji Key.
    CONVERT = KEY_INPUT_CONVERT,        // Convert Key.
    NOCONVERT = KEY_INPUT_NOCONVERT,    // NoConvert Key.
    KANA = KEY_INPUT_KANA,              // Kana Key.
    APPS = KEY_INPUT_APPS,              // Apps Key.    

    None = 0,                          // 無効,
};

// XInput ボタンのトリガー定義.
#define XINPUT_BUTTON_LT                     (10)   // 左トリガー（アナログ値で取得）.
#define XINPUT_BUTTON_RT                     (11)   // 右トリガー（アナログ値で取得）.
// Xboxコントローラー PadCode enum class.
enum class PadCode {
    Up = XINPUT_BUTTON_DPAD_UP,                 // 十字ボタン 上. ↑Button.
    Down = XINPUT_BUTTON_DPAD_DOWN,             // 十字ボタン 下. ↓Button.
    Left = XINPUT_BUTTON_DPAD_LEFT,             // 十字ボタン 左. ←Button.
    Right = XINPUT_BUTTON_DPAD_RIGHT,           // 十字ボタン 右. →Button.
    Start = XINPUT_BUTTON_START,                // スタートボタン.StartButton.
    Back = XINPUT_BUTTON_BACK,                  // バックボタン.  BackButton.
    LeftThumb = XINPUT_BUTTON_LEFT_THUMB,       // 左ステック押し込み.←LeftStick Button.
    RightThumb = XINPUT_BUTTON_RIGHT_THUMB,     // 右ステック押し込み.→RightStick Button.
    LeftBumper = XINPUT_BUTTON_LEFT_SHOULDER,   // 左ボタン.←LeftButton.
    RightBumper = XINPUT_BUTTON_RIGHT_SHOULDER, // 右ボタン.→RightButton.
    LT = XINPUT_BUTTON_LT,                      // 左トリガー.←LeftTrigger  <アマログ値 0 ～ 255>
    RT = XINPUT_BUTTON_RT,                      // 右トリガー.→RightTrigger <アマログ値 0 ～ 255>
    A = XINPUT_BUTTON_A,                        // Aボタン. A_Button.
    B = XINPUT_BUTTON_B,                        // Bボタン. B_Button.
    X = XINPUT_BUTTON_X,                        // Xボタン. X_Button.
    Y = XINPUT_BUTTON_Y,                        // Zボタン. Z_Button.

    PadCode_Max,                                // ボタンの最大数.
    PadCode_Invalid = -1,                       // 無効な値値.
    PadCode_BEGIN = 0                           // 最小値.
};
#define IS_VALID_PadCode(no) ((PadCode::PadCode_BEGIN <= (no)) && ((no) < PadCode::PadCode_Max))