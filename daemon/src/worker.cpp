#include "worker.h"


#include <sys/types.h>
#include <linux/input.h>
#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#include <QDebug>

#define KEY_RELEASE 0
#define KEY_PRESS 1
typedef struct input_event input_event;

#define ARRAY_SIZE(arr) (sizeof(arr)/sizeof(arr[0]))
#define UNKNOWN_KEY "\0"
#define UK UNKNOWN_KEY

/*
 * The order of the keys is defined in linux/input.h
 */
static char *key_names[] = {
    UK, "<ESC>",
    "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "-", "=",
    "<Backspace>", "<Tab>",
    "q", "w", "e", "r", "t", "y", "u", "i", "o", "p",
    "[", "]", "<Enter>", "<LCtrl>",
    "a", "s", "d", "f", "g", "h", "j", "k", "l", ";",
    "'", "`", "<LShift>",
    "\\", "z", "x", "c", "v", "b", "n", "m", ",", ".", "/",
    "<RShift>",
    "<KP*>",
    "<LAlt>", " ", "<CapsLock>",
    "<F1>", "<F2>", "<F3>", "<F4>", "<F5>", "<F6>", "<F7>", "<F8>", "<F9>", "<F10>",
    "<NumLock>", "<ScrollLock>",
    "<KP7>", "<KP8>", "<KP9>",
    "<KP->",
    "<KP4>", "<KP5>", "<KP6>",
    "<KP+>",
    "<KP1>", "<KP2>", "<KP3>", "<KP0>",
    "<KP.>",
    UK, UK, UK,
    "<F11>", "<F12>",
    UK, UK, UK, UK, UK, UK, UK,
    "<KPEnter>", "<RCtrl>", "<KP/>", "<SysRq>", "<RAlt>", UK,
    "<Home>", "<Up>", "<PageUp>", "<Left>", "<Right>", "<End>", "<Down>",
    "<PageDown>", "<Insert>", "<Delete>"
};

static char *shift_key_names[] = {
    UK, "<ESC>",
    "!", "@", "#", "$", "%", "^", "&", "*", "(", ")", "_", "+",
    "<Backspace>", "<Tab>",
    "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P",
    "{", "}", "<Enter>", "<LCtrl>",
    "A", "S", "D", "F", "G", "H", "J", "K", "L", ":",
    "\"", "~", "<LShift>",
    "|", "Z", "X", "C", "V", "B", "N", "M", "<", ">", "?",
    "<RShift>",
    "<KP*>",
    "<LAlt>", " ", "<CapsLock>",
    "<F1>", "<F2>", "<F3>", "<F4>", "<F5>", "<F6>", "<F7>", "<F8>", "<F9>", "<F10>",
    "<NumLock>", "<ScrollLock>",
    "<KP7>", "<KP8>", "<KP9>",
    "<KP->",
    "<KP4>", "<KP5>", "<KP6>",
    "<KP+>",
    "<KP1>", "<KP2>", "<KP3>", "<KP0>",
    "<KP.>",
    UK, UK, UK,
    "<F11>", "<F12>",
    UK, UK, UK, UK, UK, UK, UK,
    "<KPEnter>", "<RCtrl>", "<KP/>", "<SysRq>", "<RAlt>", UK,
    "<Home>", "<Up>", "<PageUp>", "<Left>", "<Right>", "<End>", "<Down>",
    "<PageDown>", "<Insert>", "<Delete>"
};

#undef UK

/*
*
 * Opens the keyboard device file
 *
 * @param  deviceFile   the path to the keyboard device file
 * @return              the file descriptor on success, error code on failure
 */
static int openKeyboardDeviceFile(char *deviceFile) {
    int kbd_fd = open(deviceFile, O_RDONLY);
    if (kbd_fd == -1) {
        qDebug() <<  strerror(errno);
        exit(-1);
    }

    return kbd_fd;
}

/**
 * Converts a key code to an ascii character. See linux/input.h for more
 * information
 *
 * @param  code the key code to convert
 * @return      the corresponding ascii character
 */
char *getKeyText(uint16_t code, uint8_t shift_pressed) {
    char **arr;
    if (shift_pressed != 0) {
        arr = shift_key_names;
    } else {
        arr = key_names;
    }

    if (code < ARRAY_SIZE(key_names)) {
        return arr[code];
    } else {
        return UNKNOWN_KEY;
    }
}

void Worker::readKeyboard(const QString &device)
{
    qDebug() << "openeing device:" << device;
    int kbd_fd = openKeyboardDeviceFile(device.toLocal8Bit().data());
    //assert(kbd_fd > 0);
    qDebug() << "fd:" << kbd_fd;

    uint8_t shift_pressed = 0;
    input_event event;
    bool alt_pressed = false;

    while (read(kbd_fd, &event, sizeof(input_event)) > 0) {
        if (event.type == EV_KEY) {
            if (event.value == KEY_PRESS) {
                char *name = getKeyText(event.code, shift_pressed);
                qDebug() << "Pressed" << name;
                if (strcmp(name, "<LAlt>") == 0) {
                    alt_pressed = true;
                } else if (strcmp(name, "<Tab>") == 0) {
                    if (alt_pressed) {
                        qDebug() << "alt-tab pressed";
                        emit altTabPressed();
                    }
                }
            } else if (event.value == KEY_RELEASE) {
                char *name = getKeyText(event.code, shift_pressed);
                qDebug() << "Released" << name;
                if (strcmp(name, "<LAlt>") == 0) {
                    alt_pressed = false;
                    emit altReleased();
                }
            }
        }
    }

    close(kbd_fd);

}
