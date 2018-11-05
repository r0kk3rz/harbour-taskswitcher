/*
 * (C) 2014 Kimmo Lindholm <kimmo.lindholm@gmail.com> Kimmoli
 *
 * Main, Daemon functions
 *
 */

#include <stdio.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <linux/input.h>

#include <QtCore/QCoreApplication>
#include <QDBusConnection>
#include <QDBusError>
#include <QDBusInterface>

#include <QDebug>

static void daemonize();
static void signalHandler(int sig);

#define KEY_RELEASE 0
#define KEY_PRESS 1
typedef struct input_event input_event;
#define SERVICE_NAME "com.kimmoli.tohkbd2user"

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

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    //daemonize();

    setlinebuf(stdout);
    setlinebuf(stderr);

    printf("Starting taskswitcher daemon version %s\n", APPVERSION);

    /* Check that we can connect to dbus systemBus and sessionBus */

    QDBusConnection dbusSystemBus = QDBusConnection::systemBus();
    if (!dbusSystemBus.isConnected())
    {
        printf("Cannot connect to the D-Bus systemBus\n%s\n",
               qPrintable(dbusSystemBus.lastError().message()));
        sleep(3);
        exit(EXIT_FAILURE);
    }
    printf("Connected to D-Bus systembus\n");

    QDBusConnection dbusSessionBus = QDBusConnection::sessionBus();
    if (!dbusSessionBus.isConnected())
    {
        printf("Cannot connect to the D-Bus sessionBus\n%s\n",
               qPrintable(dbusSessionBus.lastError().message()));
        sleep(3);
        exit(EXIT_FAILURE);
    }

    printf("Connected to D-Bus sessionbus\n");

    int kbd_fd = openKeyboardDeviceFile("/dev/input/event12");
    //assert(kbd_fd > 0);
    qDebug() << "fd:" << kbd_fd;

    uint8_t shift_pressed = 0;
    input_event event;
    bool alt_pressed = false;
    bool taskSwitcherVisible  = false;
    
    QDBusInterface iface(SERVICE_NAME, "/", "", QDBusConnection::sessionBus());
    
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

                        if (!taskSwitcherVisible)
                        {
                            /* show taskswitcher and advance one app */
                            taskSwitcherVisible = true;
                            iface.call("nextAppTaskSwitcher");
                            iface.call("showTaskSwitcher");
                        }
                        else
                        {
                            /* Toggle to next app */
                            iface.call("showTaskSwitcher");
                        }
                    }
                }
            } else if (event.value == KEY_RELEASE) {
                char *name = getKeyText(event.code, shift_pressed);
                qDebug() << "Released" << name;
                if (strcmp(name, "<LAlt>") == 0) {
                    alt_pressed = false;
                }
            }
        }
    }

    close(kbd_fd);
    return 0;

    //    Tohkbd tohkbd;

    //    /* Register to dbus systemBus */
    //    new Tohkbd2Adaptor(&tohkbd);
    //    if (!tohkbd.registerDBus())
    //    {
    //        printf("Failed to register to dbus\n");
    //        sleep(3);
    //        exit(EXIT_FAILURE);
    //    }

    //    if (!tohkbd.init())
    //    {
    //        printf("Failed to initialise tohkbd daemon\n");
    //        sleep(3);
    //        exit(EXIT_FAILURE);
    //    }

    //    /* Nokia MCE display_status_ind
    //     * used to enable and disable keyboard when display is on or off
    //     */
    //    dbusSystemBus.connect("com.nokia.mce", "/com/nokia/mce/signal", "com.nokia.mce.signal", "display_status_ind",
    //                          &tohkbd, SLOT(handleDisplayStatus(const QDBusMessage&)));

    
    return app.exec();
}

static void daemonize()
{
    /* Change the file mode mask */
    umask(0);

    /* Change the current working directory */
    if ((chdir("/tmp")) < 0)
        exit(EXIT_FAILURE);

    /* register signals to monitor / ignore */
    signal(SIGCHLD,SIG_IGN); /* ignore child */
    signal(SIGTSTP,SIG_IGN); /* ignore tty signals */
    signal(SIGTTOU,SIG_IGN);
    signal(SIGTTIN,SIG_IGN);
    signal(SIGHUP,signalHandler); /* catch hangup signal */
    signal(SIGTERM,signalHandler); /* catch kill signal */
}


static void signalHandler(int sig) /* signal handler function */
{
    switch(sig)
    {
    case SIGHUP:
        printf("Received signal SIGHUP\n");
        break;

    case SIGTERM:
        printf("Received signal SIGTERM\n");
        QCoreApplication::quit();
        break;
    }
}
